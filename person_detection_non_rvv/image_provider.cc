/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image_provider.h"

#include "model_settings.h"


extern uint32_t g_image_buf[];

TfLiteStatus GetImage(int image_width, int image_height, int channels,
                      int8_t* image_data) {
  /*for (int i = 0; i < image_width * image_height * channels; ++i) {
    image_data[i] = g_image_buf[i];
  }*/
  memcpy(image_data, g_image_buf, image_width * image_height * channels);
  return kTfLiteOk;
}
