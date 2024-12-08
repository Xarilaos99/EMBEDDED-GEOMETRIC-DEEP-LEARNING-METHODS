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
#include "DetectorPostProcessing.hpp"
#include "PlatformMath.hpp"



#include <cmath>


//My Addition
// #include "../../camera/camera_layer.c"
//#include "AlgoCodes/Algos.c"

//#include "QrDetected.h"
// #include <inttypes.h>



// extern uint8_t GrayScale[];


extern "C" {




    extern uint8_t bgr_cropped[];



   
    extern uint8_t correct_digit;

    // object_detection::Box temp;
    extern struct Box FinalBox[];

    // extern int CaptureFlag;
    extern uint8_t bsp_camera_out_buffer[];

    extern int strLen;

}





namespace arm {
namespace app {

DetectorPostProcess::DetectorPostProcess(
        TfLiteTensor* modelOutput0,
        TfLiteTensor* modelOutput1,
        
        const object_detection::PostProcessParams& postProcessParams)
        :   m_outputTensor0{modelOutput0},
            m_outputTensor1{modelOutput1},
//            m_results{results},
            m_postProcessParams{postProcessParams}
{
    /* Init PostProcessing */
    this->m_net = object_detection::Network{
        .inputWidth  = postProcessParams.inputImgCols,
        .inputHeight = postProcessParams.inputImgRows,
        .numClasses  = postProcessParams.numClasses,
		.det_threshold = postProcessParams.threshold,
        .nms_threshold = postProcessParams.nms,
        .branches =
            {object_detection::Branch{
                                      .modelOutput = this->m_outputTensor0->data.f,
                                      .size = this->m_outputTensor0->bytes},

            },
	    .topN = 0
        };
}




//*/

bool DetectorPostProcess::DoPostProcess()
{

    
    correct_digit=10;
    float pred=0.0f;


    for(uint8_t i=0;i<10;i++){
        if(this->m_net.branches[0].modelOutput[i]>pred){
            correct_digit=i;
            pred=this->m_net.branches[0].modelOutput[i];

        }
//         e_printf("%d , %f\n",i,this->m_net.branches[0].modelOutput[i]);
    }

//    if(pred>0.0f){
//        e_printf("Prediction is %d \n",correct_digit);
//
//    }
//    else{
//        e_printf("No Detection!");
//    }








    
    return true;
}









} /* namespace app */
} /* namespace arm */
