/** 
 * Copyright 2024 All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *  http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/

#include <stdio.h>

#include "main_functions.h"
#include "image_provider.h"
#include "model_settings.h"
#include "person_detect_model_data.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

// Globals, used for compatibility with Arduino-style sketches.
namespace {
tflite::ErrorReporter* error_reporter = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
const tflite::Model* model = nullptr;
TfLiteTensor* input = nullptr;

// In order to use optimized tensorflow lite kernels, a signed int8_t quantized
// model is preferred over the legacy unsigned model format. This means that
// throughout this project, input images must be converted from unisgned to
// signed format. The easiest and quickest way to convert from unsigned to
// signed 8-bit integers is to subtract 128 from the unsigned value to get a
// signed value.

// An area of memory to use for input, output, and intermediate arrays.
constexpr int kTensorArenaSize = 136 * 1024;
__attribute__((aligned(16))) static uint8_t tensor_arena[kTensorArenaSize];
}  // namespace

// The name of this function is important for Arduino compatibility.

/**
 * Initializes all data needed for the person detection example.
 * 
 * @param none
 * 
 * @return none
 */
void init_model(void)
{
    tflite::InitializeTarget();
    // Set up logging. Google style is to avoid globals or statics because of
    // lifetime uncertainty, but since this has a trivial destructor it's okay.
    // NOLINTNEXTLINE(runtime-global-variables)
    static tflite::MicroErrorReporter micro_error_reporter;
    error_reporter = &micro_error_reporter;
    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    model = tflite::GetModel(g_person_detect_model_data);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        printf(
            "Model provided is schema version %d not equal "
            "to supported version %d.\r\n",
            model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }

    // Pull in only the operation implementations we need.
    // This relies on a complete list of all the ops needed by this graph.

    // NOLINTNEXTLINE(runtime-global-variables)
    static tflite::MicroMutableOpResolver<5> micro_op_resolver;
    micro_op_resolver.AddAveragePool2D();
    micro_op_resolver.AddConv2D(tflite::Register_CONV_2D());
    micro_op_resolver.AddDepthwiseConv2D();
    micro_op_resolver.AddReshape();
    micro_op_resolver.AddSoftmax(tflite::Register_SOFTMAX());

    // Build an interpreter to run the model with.
    // NOLINTNEXTLINE(runtime-global-variables)
    static tflite::MicroInterpreter static_interpreter(model, micro_op_resolver, tensor_arena, kTensorArenaSize,
                                                       error_reporter);
    interpreter = &static_interpreter;

    // Allocate memory from the tensor_arena for the model's tensors.
    TfLiteStatus allocate_status = interpreter->AllocateTensors();
    if (allocate_status != kTfLiteOk) {
        printf("AllocateTensors() failed\r\n");
        return;
    }

    // Get information about the memory area to use for the model's input.
    input = interpreter->input(0);
}

/**
 * Run one iteration of inference on single frame taken from camera feed. 
 * This should be called repeatedly from the application code.
 * 
 * @param person_score Pointer to store person scrore.
 * @param no_person_score Pointer to store no person scrore.
 * 
 * @return 0 if successfull and else -1.
 */
int8_t run_model(int8_t* person_score, int8_t* no_person_score)
{
    if ( (person_score == NULL) || (no_person_score == NULL) )
    {
        printf("Invalid Arguments\r\n");
        return -1;
    }
    // Get image from provider.
    if (kTfLiteOk != GetImage(kNumCols, kNumRows, kNumChannels, input->data.int8)) {
        MicroPrintf("Image capture failed.");
    }

    // Run the model on this input and make sure it succeeds.
    if (kTfLiteOk != interpreter->Invoke()) {
        printf("Invoke failed.\r\n");
    }

    TfLiteTensor* output = interpreter->output(0);

    // Process the inference results.
    *person_score = output->data.uint8[kPersonIndex];
    *no_person_score = output->data.uint8[kNotAPersonIndex];

    return 0;
}

/**
 * Run model on image.
 * 
 * @param test_image Pointer to test image.
 * @param person_score Pointer to store person scrore.
 * @param no_person_score Pointer to store no person scrore.
 * 
 * @return 0 if successfull, -1 for invalid arguments and -2 for invalid image.
 */
int8_t image_tester(const uint8_t* test_image, int8_t* person_score, int8_t* no_person_score)
{
    if ( (person_score == NULL) || (no_person_score == NULL) )
    {
        printf("Invalid Arguments\r\n");
        return -1;
    }

    if (strlen((const char*) test_image) <= 0)
    {
        printf("Invalid Test Image\r\n");
        return -2;
    }
    // get test image
    memcpy(input->data.int8, test_image, kNumCols * kNumRows * kNumChannels);
    // Run the model on this input and make sure it succeeds.
    if (kTfLiteOk != interpreter->Invoke())
    {
        printf("Invoke failed.\r\n");
    }

    TfLiteTensor *output = interpreter->output(0);

    // Process the inference results.
    *person_score = output->data.uint8[kPersonIndex];
    *no_person_score = output->data.uint8[kNotAPersonIndex];

    return 0;
}
