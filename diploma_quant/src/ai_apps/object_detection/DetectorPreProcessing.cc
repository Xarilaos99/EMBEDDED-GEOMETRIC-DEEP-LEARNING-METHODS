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
#include "DetectorPreProcessing.hpp"
#include "ImageUtils.hpp"
#include "log_macros.h"
#include "hal_data.h"






uint8_t cropped_input [224*224 ] BSP_PLACE_IN_SECTION(".sdram") BSP_ALIGN_VARIABLE(8);
uint8_t output_img [28*28 ] BSP_PLACE_IN_SECTION(".sdram") BSP_ALIGN_VARIABLE(8);
uint16_t hist[256] BSP_PLACE_IN_SECTION(".sdram") BSP_ALIGN_VARIABLE(8);

uint32_t summer;
namespace arm {
namespace app {

    DetectorPreProcess::DetectorPreProcess(TfLiteTensor* inputTensor,bool rgb2Gray, bool resize, bool convertToInt8)
    :   m_inputTensor{inputTensor},
		m_rgb2Gray{rgb2Gray},
        m_resize{resize},
        m_convertToInt8{convertToInt8}
    {};

    bool DetectorPreProcess::DoPreProcess(const void* data, size_t inputSize) {
        if (data == nullptr) {
            error("Data pointer is null");
        }

        auto input = static_cast< const uint8_t*>(data);\
        uint8_t src_width = 224;
        uint8_t src_height = 224;
        uint8_t dst_width = 28;
        uint8_t dst_height = 28;


        for(uint16_t i=0;i<256;i++){
            hist[i]=0;
        }

        //this implementation is for rotated image

        // summer=0;
        // uint16_t start=8*240+8; 
        // for(int i=0;i<224;i++){
        //     for(int j=0;j<224;j++){
        //         uint8_t color_value=input[240*i+j+start];

        //         //pass the correct value to the cropped image
        //         cropped_input[i*224+j]=color_value;
        //         summer+=color_value;
        //         //this is for creating the histogram
        //     }
        // }

        //this implementation is for wXh 320X240
        uint16_t start=320*8;
        for(int i=0;i<224;i++){
            for(int j=0;j<224;j++){
                cropped_input[i*224+j]=input[start+i*320+48+j];
            }
        } 



       

        

        image::resize_bilinear(cropped_input, output_img, src_width, src_height, dst_width, dst_height);

        //Otsu's method for finding the best threshold

        //create histogram
        for(uint16_t i=0;i<28*28;i++ ){
            hist[output_img[i]]++;

        }

        // Calculate the probability distribution
        uint32_t sum=0,sumb=0;
        for (int t = 0; t < 256; t++) {
            sum += t * hist[t];
        }
        


        // declare the vars for main loop
        uint8_t threshold=0;
        uint16_t q1=0,q2=0;
        float64_t var_max=0.0f,m1=0.0f,m2=0.0f,varBetween=0.0f;


        //main for loop
        for (uint16_t t = 0; t < 256; t++) {
            q1+=hist[t];
            if(q1==0){
                continue;
            }

            q2=28*28-q1;
            if (q2==0){
                break;
            }

            sumb+=t * hist[t];
            m1=sumb/q1;
            m2=(sum-sumb)/q2;

            varBetween = q1 * q2 * (m1 - m2) * (m1 - m2);

            if(varBetween>var_max){
                threshold=t;
                var_max=varBetween;

            }

        }
                

        //change the values of the small image
        for(uint16_t t=0;t<28*28;t++){
            if(output_img[t]>threshold){
                output_img[t]=0;
            }
            else{
                output_img[t]=255;
            }
        }




        //this preprocess it is not needed evaluate


        if (this->m_resize){

            for(int i=0;i<28*28;i++){
                this->m_inputTensor->data.f[i]=output_img[i]/255.f;
            }

        }
        else {
            std::memcpy(this->m_inputTensor->data.data, input, this->m_inputTensor->bytes);
        }

        
        return true;
    }

} /* namespace app */
} /* namespace arm */
