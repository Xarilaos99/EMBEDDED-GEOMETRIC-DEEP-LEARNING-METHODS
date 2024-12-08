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
#ifndef DETECTOR_POST_PROCESSING_HPP
#define DETECTOR_POST_PROCESSING_HPP

#include <ai_apps/common/BaseProcessing.hpp>
#include <ai_apps/common/ImageUtils.hpp>
//#include <ai_apps/object_detection/DetectionResult.hpp>
#include <ai_apps/object_detection/YoloFastestModel.hpp>
#include <forward_list>
#include "camera_layer.h"


namespace arm {
namespace app {
namespace object_detection {

    struct PostProcessParams {
        int inputImgRows{};
        int inputImgCols{};
        int originalImageSize{};
        // const float* anchor1;
        // const float* anchor2;
        float threshold = 0.15f;

        //change nms for not find a lot of boxes
        float nms = 0.35f;
        int numClasses = 2;
        int topN = 0;
	PostProcessParams(int iptRows, int iptCols, int size):
	    inputImgRows(iptRows), inputImgCols(iptCols), originalImageSize(size){}
		
    };

    struct Branch {
        float* modelOutput;
        // float scale;
        // int zeroPoint;
        size_t size;
    };


    struct Network {
        int inputWidth;
        int inputHeight;
        int numClasses;
        float det_threshold;
        float nms_threshold;
        std::vector<Branch> branches;
        int topN;
    };
} /* namespace object_detection */

    /**
     * @brief   Post-processing class for Object Detection use case.
     *          Implements methods declared by BasePostProcess and anything else needed
     *          to populate result vector.
     */
    class DetectorPostProcess : public BasePostProcess {
    public:
        /**
         * @brief        Constructor.
         * @param[in]    outputTensor0       Pointer to the TFLite Micro output Tensor at index 0.
         * @param[in]    outputTensor1       Pointer to the TFLite Micro output Tensor at index 1.
         * @param[out]   results             Vector of detected results.
         * @param[in]    postProcessParams   Struct of various parameters used in post-processing.
         **/
        explicit DetectorPostProcess(TfLiteTensor* outputTensor0,
                                     TfLiteTensor* outputTensor1,
                                    
                                     const object_detection::PostProcessParams& postProcessParams);

        /**
         * @brief    Should perform YOLO post-processing of the result of inference then
         *           populate Detection result data for any later use.
         * @return   true if successful, false otherwise.
         **/
        bool DoPostProcess() override;

        

    private:
        TfLiteTensor* m_outputTensor0;                                   /* Output tensor index 0 */
        TfLiteTensor* m_outputTensor1;                                   /* Output tensor index 1 */
//        std::vector<object_detection::DetectionResult>& m_results;       /* Single inference results. */
        const object_detection::PostProcessParams& m_postProcessParams;  /* Post processing param struct. */
        object_detection::Network m_net;                                 /* YOLO network object. */

        /**
         * @brief       Insert the given Detection in the list.
         * @param[in]   detections   List of detections.
         * @param[in]   det          Detection to be inserted.
         **/
        // void InsertTopNDetections(std::forward_list<image::Detection>& detections, image::Detection& det);

        


    };

} /* namespace app */
} /* namespace arm */

#endif /* DETECTOR_POST_PROCESSING_HPP */
