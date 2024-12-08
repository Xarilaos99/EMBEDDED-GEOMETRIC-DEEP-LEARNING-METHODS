/*
 * SPDX-FileCopyrightText: Copyright 2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <ai_apps/common/ImageUtils.hpp>
#include "log_macros.h"
#include <limits>
#include <cmath>

namespace arm {
namespace app {
namespace image {

    
    


    
    uint8_t bilinear_interpolate(uint8_t *image, int width, int height, float x, float y) {
        int x1 = (int)x;
        int y1 = (int)y;
        int x2 = x1 + 1;
        int y2 = y1 + 1;

        // Boundary conditions
        if (x2 >= width) x2 = width - 1;
        if (y2 >= height) y2 = height - 1;

        // Pixel values at four nearest neighbors
        uint8_t f11 = image[y1 * width + x1];
        uint8_t f12 = image[y2 * width + x1];
        uint8_t f21 = image[y1 * width + x2];
        uint8_t f22 = image[y2 * width + x2];

        // Bilinear interpolation formula
        float dx = x - x1;
        float dy = y - y1;

        float result = f11 * (1 - dx) * (1 - dy) +
                    f21 * dx * (1 - dy) +
                    f12 * (1 - dx) * dy +
                    f22 * dx * dy;

        return (uint8_t)(result + 0.5);  // Adding 0.5 for rounding
    }


    void resize_bilinear(uint8_t *input_image, uint8_t *output_image, int src_width, int src_height, int dst_width, int dst_height) {
    // float x_ratio = (float)(src_width - 1) / (dst_width - 1);
    // float y_ratio = (float)(src_height - 1) / (dst_height - 1);
    float x_ratio = (float)(src_width) / (dst_width);
    float y_ratio = (float)(src_height) / (dst_height);

    //  e_printf("x: %f",x_ratio);
    //  e_printf("y: %f",y_ratio);

    for (int j = 0; j < dst_height; j++) {
        for (int i = 0; i < dst_width; i++) {
            float x = i * x_ratio;
            float y = j * y_ratio;
            //this is for rotated image
            // output_image[(dst_width-i)*dst_width+j] = bilinear_interpolate(input_image, src_width, src_height, x, y);

            output_image[j*dst_width+i] = bilinear_interpolate(input_image, src_width, src_height, x, y);
        }
    }
}


} /* namespace image */
} /* namespace app */
} /* namespace arm */
