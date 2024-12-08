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
#include <InputFiles_obj.hpp>
#include <UseCaseHandler_obj.hpp>
#include "YoloFastestModel.hpp"
#include "UseCaseCommonUtils.hpp"
#include "DetectorPostProcessing.hpp"
#include "DetectorPreProcessing.hpp"
#include "log_macros.h"
#include "camera_layer.h"

#include "semphr.h"
// #include "mnist_image0.h"
#include "temp2.h"

extern "C" {
#include "timer.h"
void update_detection_result(signed short index, signed short  x, signed short  y, signed short  w, signed short  h);
}

#include <cinttypes>

// extern uint8_t bsp_camera_out_buffer888[];
// extern uint8_t bsp_det_model_ip_buffer888[];
// extern uint8_t bsp_det_rot_model_ip_buffer888[];
// extern uint8_t bsp_det_crop_model_ip_buffer888[];
// extern uint8_t bsp_camera_out_rot_buffer565[];
// extern uint8_t bsp_camera_out_buffer565[];
extern uint8_t GrayScale[];
extern uint8_t bsp_camera_out_buffer888[];
extern int CaptureFlag;
#define DST_HEIGHT 144
#define DST_WIDTH  144
#define SRC_HEIGHT 240
#define SRC_WIDTH  240
#define img_channel 1
#define crop_dim 240
#define WIDTH 240
#define HEIGHT 320



//WE YOU CHANGE THIS REMEMBER TO CHANGE THE RESIZE FACTORS
#define DEBUG 1

int timeC=0;
extern uint8_t bgr_cropped[];
// uint8_t rgb[ BSP_CAM_WIDTH  * BSP_CAM_HEIGHT * 3 ] BSP_PLACE_IN_SECTION(".sdram") BSP_ALIGN_VARIABLE(8);

extern SemaphoreHandle_t CameraFlag;



extern "C" {
	extern uint64_t face_detection_inference_time;
    // extern void bsp_camera_yuv422_to_rgb888(const void* inbuf, void* outbuf, uint16_t width, uint16_t height);

}






namespace arm {
namespace app {

    /**
     * @brief           Presents inference results along using the data presentation
     *                  object.
     * @param[in]       results            Vector of detection results to be displayed.
     * @return          true if successful, false otherwise.
     **/
//    static bool PresentInferenceResult(const std::vector<object_detection::DetectionResult>& results);

    /**
     * @brief           Draw boxes directly on the LCD for all detected objects.
     * @param[in]       results            Vector of detection results to be displayed.
     * @param[in]       imageStartX        X coordinate where the image starts on the LCD.
     * @param[in]       imageStartY        Y coordinate where the image starts on the LCD.
     * @param[in]       imgDownscaleFactor How much image has been downscaled on LCD.
     **/

    /* Object detection inference handler. */
    bool ObjectDetectionHandler(ApplicationContext& ctx, uint32_t imgIndex, bool runAll)
    {
        auto& model = ctx.Get<Model&>("model");
        
        if (!model.IsInited()) {
             error("Model is not initialized! Terminating processing.\n");
            return false;
        }

        TfLiteTensor* inputTensor = model.GetInputTensor(0);
        TfLiteTensor* outputTensor0 = model.GetOutputTensor(0);
        TfLiteTensor* outputTensor1 = model.GetOutputTensor(1);

        if (!inputTensor->dims) {
            error("Invalid input tensor dims\n");
            return false;
        } else if (inputTensor->dims->size < 3) {
            error("Input tensor dimension should be >= 3\n");
            return false;
        }

        TfLiteIntArray* inputShape = model.GetInputShape(0);

        const int inputImgCols = inputShape->data[YoloFastestModel::ms_inputColsIdx];
        const int inputImgRows = inputShape->data[YoloFastestModel::ms_inputRowsIdx];
        // const int inputImgRows = inputShape->data[YoloFastestModel::ms_inputChannelsIdx];


        #if DEBUG ==1
            DetectorPreProcess preProcess = DetectorPreProcess(inputTensor, false,true, model.IsDataSigned());
        #else 
            DetectorPreProcess preProcess = DetectorPreProcess(inputTensor, false,true, model.IsDataSigned());
        #endif

        // std::vector<object_detection::DetectionResult> results;
        
        arm::app::object_detection::PostProcessParams postProcessParams {
            inputImgRows, inputImgCols, object_detection::originalImageSize,
            
        };

        DetectorPostProcess postProcess = DetectorPostProcess(outputTensor0, outputTensor1,
                postProcessParams);
        {
            /* Ensure there are no results leftover from previous inference when running all. */
            // results.clear();


            // const uint8_t* currImage = GrayScale;


            


        const uint8_t* currImage;

        #if DEBUG ==1

                // currImage=bsp_camera_out_buffer888;
                // currImage = BufferImage0;
                // currImage = BufferImage1;
                currImage = GrayScale;

        #else 
             currImage = bsp_camera_out_buffer888;
        #endif

            
            timeC++;

            if(timeC>20){
                // myPrintf(bgr_cropped,240*240);
                e_printf("xasame");
                timeC=0;
                // CaptureFlag=0;

            }



            

             if(!CaptureFlag){


    
                //*
                if (!preProcess.DoPreProcess(currImage, 256*256*3)) {
                    error("Pre-processing failed.");
                    return false;
                }


                // Run inference over this image. 
//                SCB_CleanDCache();

                uint64_t t1 = get_timestamp();
                if (!RunInference(model)) {
                    error("Inference failed.");
                    return false;
                }

                // e_printf("t1");
                face_detection_inference_time = get_timestamp() - t1;
//                SCB_CleanDCache();

                // SCB_CleanDCache();
                if (!postProcess.DoPostProcess()) {
                    error("Post-processing failed.");
                    return false;
                }
                // SCB_CleanDCache();
                //*/
                


               

                // e_printf("camera flag12 %d %d",g_start_menu_binary_semaphore,xSemaphoreTake(g_start_menu_binary_semaphore, pdMS_TO_TICKS ( 500u )));
                if(g_start_menu_binary_semaphore!=NULL){
                   

                    if(xSemaphoreTake(g_start_menu_binary_semaphore, pdMS_TO_TICKS ( 500u )) == pdTRUE){

                        CaptureFlag=1;
                        // e_printf("inside after post");


                        
                        // vTaskDelay(30);


                        xSemaphoreGive(g_start_menu_binary_semaphore);
                        SCB_CleanDCache();
                        vTaskDelay(200);

                        // SCB_DisableDCache();
                        // vTaskDelay(450);


                        
                        // vTaskDelay(100);
    //                    xSemaphoreGiveFromISR(g_start_menu_binary_semaphore , pdFALSE );

                    }
                }

                    
                    // e_printf("Tf");
                    timeC=0;
                
            }
            

            e_printf("\r\nInference time: %.1f ms", face_detection_inference_time/1000.f);
        
        // e_printf("Tfq");
            

            





            
            // if (!PresentInferenceResult(results)) {
            //     return false;
            // }

        
        return true;
    }



    }
} /* namespace app */
} /* namespace arm */




