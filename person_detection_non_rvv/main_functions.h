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

#ifndef MAIN_FUNCTIONS_H_
#define MAIN_FUNCTIONS_H_

// Expose a C friendly interface for main functions.
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes all data needed for the person detection example.
 * 
 * @param none
 * 
 * @return none
 */
void init_model(void);

/**
 * Run one iteration of inference on single frame taken from camera feed. 
 * This should be called repeatedly from the application code.
 * 
 * @param person_score Pointer to store person scrore.
 * @param no_person_score Pointer to store no person scrore.
 * 
 * @return 0 if successfull and else -1.
 */
int8_t run_model(int8_t* person_score, int8_t* no_person_score);

/**
 * Run model on image.
 * 
 * @param test_image Pointer to test image.
 * @param person_score Pointer to store person scrore.
 * @param no_person_score Pointer to store no person scrore.
 * 
 * @return 0 if successfull, -1 for invalid arguments and -2 for invalid image.
 */
int8_t image_tester(const uint8_t* test_image, int8_t* person_score, int8_t* no_person_score);

#ifdef __cplusplus
}
#endif

#endif  // MAIN_FUNCTIONS_H_
