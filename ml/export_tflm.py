import numpy as np
import tensorflow as tf
from tensorflow import keras
from pathlib import Path

OUT_DIR = Path(__file__).resolve().parent

def rule(dv, dh, pos1, pos2, soma):
    limiar = 50.0
    vel = 2.0
    d1 = 0.0; d2 = 0.0
    if abs(dv) > limiar: d1 = vel if dv > 0 else -vel
    if abs(dh) > limiar: d2 = vel if dh > 0 else -vel
    return d1, d2

def synth_dataset(N=50000):
    rng = np.random.default_rng(42)
    ldrTop = rng.integers(0,4096,N)
    ldrBottom = rng.integers(0,4096,N)
    ldrLeft = rng.integers(0,4096,N)
    ldrRight = rng.integers(0,4096,N)
    dv = (ldrTop - ldrBottom).astype(np.float32)
    dh = (ldrLeft - ldrRight).astype(np.float32)
    soma = (ldrTop + ldrBottom + ldrLeft + ldrRight).astype(np.float32)
    pos1 = rng.integers(0,181,N).astype(np.float32)
    pos2 = rng.integers(0,181,N).astype(np.float32)
    x = np.stack([
        dv / 4095.0,
        dh / 4095.0,
        (pos1 - 90.0)/90.0,
        (pos2 - 90.0)/90.0,
        soma / (4*4095.0)
    ], axis=1).astype(np.float32)
    y = np.zeros((N,2), np.float32)
    for i in range(N):
        y[i] = rule(dv[i], dh[i], pos1[i], pos2[i], soma[i])
    y /= 3.0
    return x, y

def build_model():
    inputs = keras.Input(shape=(5,), dtype=tf.float32)
    h = keras.layers.Dense(16, activation="relu")(inputs)
    h = keras.layers.Dense(16, activation="relu")(h)
    outs = keras.layers.Dense(2, activation="tanh")(h)
    model = keras.Model(inputs, outs)
    model.compile(optimizer="adam", loss="mse")
    return model

def convert_int8(model, x):
    def rep_ds():
        for i in range(100):
            yield [x[np.random.randint(0, x.shape[0], 256)]]
    conv = tf.lite.TFLiteConverter.from_keras_model(model)
    conv.optimizations = [tf.lite.Optimize.DEFAULT]
    conv.representative_dataset = rep_ds
    conv.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
    conv.inference_input_type = tf.int8
    conv.inference_output_type = tf.int8
    return conv.convert()

def write_model_h(tfl_bytes):
    path = OUT_DIR / "model.h"
    hexarr = ", ".join(hex(b) for b in tfl_bytes)
    path.write_text(f"const unsigned char model_tflite[] = {{{hexarr}}};\nconst int model_tflite_len = {len(tfl_bytes)};\n")
    print(f"[OK] model.h gerado ({len(tfl_bytes)} bytes)")

def main():
    x, y = synth_dataset()
    model = build_model()
    model.fit(x, y, epochs=6, batch_size=256, verbose=2)
    tfl = convert_int8(model, x)
    (OUT_DIR / "model.tflite").write_bytes(tfl)
    write_model_h(tfl)
    print("[OK] modelo exportado com sucesso")

if __name__ == "__main__":
    main()
