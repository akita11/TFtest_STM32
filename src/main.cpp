#include <Arduino.h>

#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

// TensorFlow Liteモデルのインクルード
extern const unsigned char model_tflite[];
extern const int model_tflite_len;

const int kTensorArenaSize = 2 * 1024;
uint8_t tensor_arena[kTensorArenaSize];

void setup() {
  // Set up logging.
  static tflite::MicroErrorReporter micro_error_reporter;
  tflite::ErrorReporter* error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure.
  const tflite::Model* model = tflite::GetModel(model_tflite);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report("Model provided is schema version %d not equal "
                           "to supported version %d.",
                           model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // This pulls in all the operation implementations we need.
  static tflite::AllOpsResolver resolver;

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  tflite::MicroInterpreter* interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    error_reporter->Report("AllocateTensors() failed");
    return;
  }

  // Obtain a pointer to the model's input tensor.
  TfLiteTensor* input = interpreter->input(0);
  input->data.f[0] = Ton;
  input->data.f[1] = v0;
  input->data.f[2] = v1;

  // Run inference, and report any error.
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    error_reporter->Report("Invoke failed");
    return;
  }

  // Obtain the output tensor.
  TfLiteTensor* output = interpreter->output(0);
  float pos = output->data.f[0];

  // Output the result.
  Serial.println(pos);
}

void loop() {
  // Do nothing here.
}

