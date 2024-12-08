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
#include "YoloFastestModel.hpp"

#include "log_macros.h"

const tflite::MicroOpResolver& arm::app::YoloFastestModel::GetOpResolver()
{
    return this->m_opResolver;
}

bool arm::app::YoloFastestModel::EnlistOperations()
{

    /*
//    this->m_opResolver.AddDepthwiseConv2D();
   this->m_opResolver.AddConv2D();
   this->m_opResolver.AddAdd();
//    this->m_opResolver.AddAdd();
//    this->m_opResolver.AddAddN();
//    this->m_opResolver.AddResizeNearestNeighbor();
   
   this->m_opResolver.AddPad();
//    this->m_opResolver.AddMaxPool2D();
   this->m_opResolver.AddConcatenation();
   this->m_opResolver.AddMul();

   
//    this->m_opResolver.AddL2Normalization();
   

   //My Code
   this->m_opResolver.AddTranspose();
   this->m_opResolver.AddSoftmax();
   this->m_opResolver.AddReshape();
   this->m_opResolver.AddQuantize();
*/
   //this is for yolo
   /*These are needed for UT to work, not needed on FVP */
   this->m_opResolver.AddQuantize();
   this->m_opResolver.AddShape();
   this->m_opResolver.AddStridedSlice();
   this->m_opResolver.AddPack();
   this->m_opResolver.AddReshape();
   this->m_opResolver.AddConv2D();
   this->m_opResolver.AddMaxPool2D();
   this->m_opResolver.AddFullyConnected();
   this->m_opResolver.AddDequantize();



    if (kTfLiteOk == this->m_opResolver.AddEthosU()) {
        info("Added %s support to op resolver\n",
            tflite::GetString_ETHOSU());
    } else {
        error("Failed to add Arm NPU support to op resolver.");
        return false;
    }
    return true;
}
