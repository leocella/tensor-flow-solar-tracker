#pragma once
#include <EloquentTinyML.h>
#include <eloquent_tinyml/tensorflow.h>
#include "../ml/model.h"

// Entradas: [difVertical, difHorizontal, posServo1, posServo2, soma_norm]
// Saídas: [delta1, delta2] em -1..1, convertidas para -3..+3 graus

#define N_INPUTS  5
#define N_OUTPUTS 2
#define TENSOR_ARENA_SIZE (20 * 1024)

static Eloquent::TinyML::TensorFlow::Model<N_INPUTS, N_OUTPUTS, TENSOR_ARENA_SIZE> policy;

inline void ml_begin() {
  policy.begin(model_tflite, model_tflite_len);
}

inline void ml_infer(float dv, float dh, float pos1, float pos2, float soma_norm, float &d1, float &d2) {
  float in[N_INPUTS] = {
    dv / 4095.0f,
    dh / 4095.0f,
    (pos1 - 90.0f) / 90.0f,
    (pos2 - 90.0f) / 90.0f,
    soma_norm
  };
  float out[N_OUTPUTS] = {0, 0};
  policy.predict(in, out);

  d1 = out[0] * 3.0f;
  d2 = out[1] * 3.0f;
}
