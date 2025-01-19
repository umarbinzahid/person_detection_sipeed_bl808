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
#include <string.h>
#include "main_functions.h"
#include <bl_cam.h>
#include <bl_timer.h>
#include <imgtool/bilinear_interpolation.h>
#include <imgtool/rgb_cvt.h>
/* FreeRTOS */
#include <FreeRTOS.h>
#include <task.h>
/* LCD */
#include <lcd.h>
/* Test Images */
#include "test_image_data.h"

// Camera
#define CAMERA_W (400)
#define CAMERA_H (300)
#define TARGET_WH (CAMERA_H)
#define IMG_W (96)
#define IMG_H (96)
// LCD
#define BLACK_COLOR (0x000000)
#define WHITE_COLOR (0xFFFFFF)
#define X_OFFSET (20 + 16)
#define Y_OFFSET (16)

// Global Destructor for C++, which we're not using.
// See https://alex-robenko.gitbook.io/bare_metal_cpp/compiler_output/static#custom-destructors
void *__dso_handle = NULL;

uint8_t g_image_buf[IMG_W * IMG_H] = {0};

int main()
{
    uint8_t* picture = NULL;
    uint32_t length = 0;
    uint32_t start_time = 0;
    uint32_t end_time = 0;
    static uint32_t g_image_buf_temp[IMG_W * IMG_H] = {0};
    uint8_t lcd_buff[128] = {0};
    int8_t person_score = 0;
    int8_t no_person_score = 0;

    // init lcd
    if (lcd_init() == 0) {
        lcd_clear(WHITE_COLOR); // clear lcd with all black
        lcd_set_dir(1, 0); // set direction
    } else {
        printf("LCD init failed\r\n");
    }

    // init model
    init_model();
    printf("model load successfully!!\r\n");

#ifndef RUN_MODEL_ON_TEST_IMAGES
    // init camera
    while (0 != bl_cam_mipi_yuv_init()) {
        printf("bl cam init fail!\r\n");
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    printf("Camera initialized!\r\n");

    while (1) 
    {
        printf("===>\r\n");
        start_time = bl_timer_now_us() / 1000;
        printf("Time in millisecond before acquirng frame: %d\r\n", start_time);
        // get frame
        while (0 != bl_cam_mipi_rgb_frame_get(&picture, &length)) {
        }
        // copy the bytes
        for (uint16_t ih = 0; ih < CAMERA_H; ih++) {
            memcpy(picture + sizeof(uint32_t) * ih * TARGET_WH,
                   picture + sizeof(uint32_t) * (ih * CAMERA_W + (CAMERA_W - CAMERA_H) / 2),
                   sizeof(uint32_t) * TARGET_WH);
        }
        // resize the image
        BilinearInterpolation_RGBA8888((uint32_t*)picture, TARGET_WH, TARGET_WH, g_image_buf_temp, IMG_W, IMG_H);
        // convert it to gray
        RGBA88882GRAY(g_image_buf_temp, g_image_buf, IMG_W, IMG_H);
        // run model
        if ( run_model(&person_score, &no_person_score) != 0)
        {
            break;
        }
        // capture end time
        end_time = bl_timer_now_us() / 1000;

        // write on cli
        printf("person score:%d no person score %d\r\n", person_score, no_person_score);
        printf("Time in millisecond after running model: %d\r\n", end_time);
        printf("Time difference: %d\r\n", end_time - start_time);
        printf("<===\r\n");

        // write on lcd
        lcd_clear(BLACK_COLOR); // clear lcd with all black
        memset(lcd_buff, 0, sizeof(lcd_buff));
        sprintf ( (char*) lcd_buff, 
                "Person Score: %d\r\nNo Person Score: %d\r\nTime Taken: %d", 
                person_score, no_person_score, (end_time - start_time));
        lcd_draw_str_ascii16(X_OFFSET, Y_OFFSET, (lcd_color_t) WHITE_COLOR, (lcd_color_t) BLACK_COLOR, lcd_buff, 128);

        // pop the new frame
        bl_cam_mipi_frame_pop();
        // wait
        vTaskDelay(5);
    }

    bl_cam_mipi_yuv_deinit();

#else /* RUN_MODEL_ON_TEST_IMAGES */
        printf("===>\r\n");
        start_time = bl_timer_now_us() / 1000;
        printf("Time in millisecond before acquirng frame: %d\r\n", start_time);

        // update test image data in "test_image_data.cc"
        if ( image_tester(g_test_image_data, &person_score, &no_person_score) != 0)
        {
            bl_cam_mipi_yuv_deinit();
            return 0;
        }
        // capture end time
        end_time = bl_timer_now_us() / 1000;

        // write on cli
        printf("person score:%d no person score %d\r\n", person_score, no_person_score);
        printf("Time in millisecond after running model: %d\r\n", end_time);
        printf("Time difference: %d\r\n", end_time - start_time);
        printf("<===\r\n");

        // write on lcd
        lcd_clear(WHITE_COLOR); // clear lcd with all black
        memset(lcd_buff, 0, sizeof(lcd_buff));
        sprintf ( (char*) lcd_buff, 
                "Person Score: %d\r\nNo Person Score: %d\r\nTime Taken: %d", 
                person_score, no_person_score, (end_time - start_time));
        lcd_draw_str_ascii16(X_OFFSET, Y_OFFSET, (lcd_color_t) BLACK_COLOR, (lcd_color_t) WHITE_COLOR, lcd_buff, 128);

#endif /* RUN_MODEL_ON_TEST_IMAGES */

    return 0;
}
