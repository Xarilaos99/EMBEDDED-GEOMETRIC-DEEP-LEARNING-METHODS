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
#ifndef IMAGE_UTILS_HPP
#define IMAGE_UTILS_HPP

#include <cstddef>
#include <cstdint>
#include <forward_list>
#include <vector>

/* Helper macro to convert RGB888 to RGB565 format. */
#define RGB888_TO_RGB565(R8,G8,B8)  ((((R8>>3) & 0x1F) << 11) |     \
                                     (((G8>>2) & 0x3F) << 5)  |     \
                                     ((B8>>3) & 0x1F))

constexpr uint16_t COLOR_BLACK  = 0;
constexpr uint16_t COLOR_GREEN  = RGB888_TO_RGB565(  0, 255,  0); // 2016;
constexpr uint16_t COLOR_YELLOW = RGB888_TO_RGB565(255, 255,  0); // 65504;


namespace arm {
namespace app {
namespace image {

    
    

    

    uint8_t bilinear_interpolate(uint8_t *image, int width, int height, float x, float y);
    void resize_bilinear(uint8_t *input_image, uint8_t *output_image, int src_width, int src_height, int dst_width, int dst_height);






} /* namespace image */
} /* namespace app */
} /* namespace arm */

#endif /* IMAGE_UTILS_HPP */
