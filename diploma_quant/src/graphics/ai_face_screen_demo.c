/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER Escreen_offset_xpRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE Escreen_offset_xpRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : led_screen_demo.c
 * Version      : .
 * Description  : The led demo screen display.
 *********************************************************************************************************************/

#include <math.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"

#include "common_utils.h"
#include "common_init.h"
#include "menu_camview.h"
#include "jlink_console.h"

#include "r_ioport.h"
#include "r_mipi_dsi_api.h"

#include "hal_data.h"
#include "dsi_layer.h"

#include "camera_layer.h"
#include "graphics.h"

#include "r_glcdc.h"
#include "r_glcdc_cfg.h"

#include "touch_ft5x06.h"
#include "gimp.h"

#include "font_ai_face_digit.h"
#include "bg_font_18_full.h"
// #include "../ai_apps/object_detection/mnist_image0.h"
#include "../ai_apps/object_detection/temp2.h"
#include "../conf.h"
extern uint32_t get_image_data(st_image_data_t ref);
extern bool_t in_transition(void);

extern char_t g_print_buffer[];

extern void show_menu_icon();
extern void process_main_menu();
// extern uint8_t 			 bsp_camera_out_rot_buffer565 [ BSP_CAM_WIDTH  * BSP_CAM_HEIGHT * BSP_CAM_BYTE_PER_PIXEL ];
// extern st_ai_detection_point_t g_ai_detection[5];

uint64_t face_detection_inference_time;
// uint64_t face_detection_inference_time;
extern char BufferMessage[];
extern uint8_t MyBuMes[];


// uint8_t TempMes[32];
char TempMes[32];

extern struct Box FinalBox[];

extern uint8_t GrayScale[];


extern uint8_t correct_digit;
int strLen;
int temLen=0;
uint8_t digit;

extern uint8_t bgr_cropped[];

// extern uint8_t BufferImageQR[];

//for rgb565
// extern uint8_t 	bsp_camera_out_rot_buffer565 [ BSP_CAM_WIDTH  * BSP_CAM_HEIGHT * BSP_CAM_BYTE_PER_PIXEL ];

int CaptureFlag=0;



extern SemaphoreHandle_t CameraFlag;


d2_point xmin[10]={0};
d2_point ymin[10]={0};
d2_point xmax[10]={0};
d2_point ymax[10]={0};
// d2_point xmin=0;
// d2_point ymin=0;
// d2_point xmax=0;
// d2_point ymax=0;
uint64_t time;


// uint8_t RevImg2[28*28] BSP_ALIGN_VARIABLE(8)  BSP_PLACE_IN_SECTION(".sdram") = {0};
uint8_t RevImg2[240*240] BSP_ALIGN_VARIABLE(8)  BSP_PLACE_IN_SECTION(".sdram") = {0};


//irida labs logo

#include "VVRLogoSmall.h"

void  do_face_reconition_screen(void);

/*
 * SIMULATED AI uses MAB image and should no longer be used
 */
// #define USE_SIMULATED_AI_DATA

#ifdef  USE_SIMULATED_AI_DATA


#else // USE_SIMULATED_AI_DATA

void  do_face_reconition_screen(void)
{

    // SCB_CleanDCache();
    // SCB_EnableDCache();
//    st_gimp_image_t * img = NULL;
    // static uint64_t t1 = 0;
    /* Wait for vertical blanking period */
    // e_printf("BeforeVsync");
    graphics_wait_vsync();
    // e_printf("Beforeframe");
    graphics_start_frame();
    // e_printf("AfterFram");
    // e_printf("CaptureLcd123: %d",CaptureFlag);







    if(in_transition())
    {


       
        d2_setcolor(d2_handle, 0, 0xFFFF00);
      

    }
    else
    {

        

//        vTaskDelay(3);
        // SCB_CleanDCache();
        int qr_count = 0;


        // d2_clear(d2_handle,0xFF66B2FF);
        d2_clear(d2_handle,0xFFFFFF);

        // d2_setblitsrc(d2_handle, Logo, 170, 170, 170, d2_mode_rgb565);
        d2_setblitsrc(d2_handle, VVRLogosmall, 200, 200, 200, d2_mode_rgb565);
 


        d2_blitcopy(d2_handle,
            200, 200,  // Source width/height
            (d2_blitpos) 0, 0, // Source position
            (d2_width) ((250) << 4), (d2_width) ((250) << 4), // Destination size width/height
            (d2_width) ((280) << 4), (d2_width) ((480+62) << 4), // Destination offset position

            d2_tm_filter);
  
        

        //*



        
        
        //Here is where I print the output of the Camera =============================================================

        //static image
        // d2_setblitsrc(d2_handle, RevImg2, 28, 28,28, 0);

        //camera input
        d2_setblitsrc(d2_handle, RevImg2, 240, 240,240, 0);
        



   

        //*
        //this is 480*600 left===========================================================================
        // SCB_CleanDCache();
        // d2_blitcopy(d2_handle,
        //         28, 28,  // Source width/height
        //         (d2_blitpos) 0, 0, // Source position
        //         (d2_width) ((480) << 4), (d2_width) ((480) << 4), // Destination size width/height
        //         (d2_width) ((0) << 4), (d2_width) ((0) << 4), // Destination offset position
        //         d2_tm_filter);
        d2_blitcopy(d2_handle,
                240, 240,  // Source width/height
                (d2_blitpos) 0, 0, // Source position
                (d2_width) ((480) << 4), (d2_width) ((480) << 4), // Destination size width/height
                (d2_width) ((0) << 4), (d2_width) ((0) << 4), // Destination offset position
                d2_tm_filter);




        if(digit<10){
            char detect[] = "Digit Detected: 0\0";
            detect[16]+=digit;
            print_bg_font_18(d2_handle, 140,  480+62,  (char*)detect);



        }
        else{
            char detect[] = "No Detection!!";
            print_bg_font_18(d2_handle, 140,  480+62,  (char*)detect);

        }

       

#if QUANT && HELIUM

        char time_str[7] = {'0', '0', '0', ' ', 'u', 's', '\0'};
        time_str[0] += (time / 100);
        time_str[1] += (time / 10) % 10;
        time_str[2] += time % 10;
        print_bg_font_18(d2_handle, 170,480+62+80,  (char*)time_str);

        char infTime[11] = "Inf. Time:\0";
        print_bg_font_18(d2_handle, 170, 480+62,  (char*)infTime);
#else

        char time_str[7] = {'0', '0', '0', ' ', 'm', 's', '\0'};
        time_str[0] += (time / 100);
        time_str[1] += (time / 10) % 10;
        time_str[2] += time % 10;
        print_bg_font_18(d2_handle, 170,480+62+80,  (char*)time_str);

        char infTime[11] = "Inf. Time:\0";
        print_bg_font_18(d2_handle, 170, 480+62,  (char*)infTime);

#endif 
        


#if QUANT
        char select_mod[] = "Quantized Model For Handwritten\0";
#else
        char select_mod[] = "Normal Model For Handwritten\0";
#endif   


#if HELIUM
        char select_mod0[] = "Helium Enabled\0";
#else
        char select_mod0[] = "Helium Disabled\0";
#endif      

        char ek_ra8d1[] = "Digits on EK-RA8D1\0";
        // char a[2]="A\0";
        // print_bg_font_18(d2_handle, 350, 670,  (char*)a);s

        print_bg_font_18(d2_handle, 300, 480+62,  (char*)select_mod);

        // vTaskDelay(4);

        print_bg_font_18(d2_handle, 270, 480+62,  (char*)ek_ra8d1);
        print_bg_font_18(d2_handle, 220, 480+62,  (char*)select_mod0);
        // bsp_camera_capture_image();






            

        
    }




    vTaskDelay(30); 
    if(g_start_menu_binary_semaphore!=NULL){
    
            // e_printf("Inside Camera1");
            if(xSemaphoreTake(g_start_menu_binary_semaphore, pdMS_TO_TICKS ( 500u ))==pdTRUE  )   {
               
                // SCB_EnableDCache();
                // e_printf("Inside Capture");
                

                if(CaptureFlag){

                    // e_printf("Inside Camera2");


                    // vTaskDelay(3); 

                    digit=correct_digit;
                    // vTaskDelay(3); 





                    
                    


                    //this is for the static image 

                    /*
                    //this is wrong orientation
                    // for(int i=0;i<28*28;i++){
                    //     RevImg2[i]=BufferImage0[i];
                    // }

                    for(int i=0;i<28;i++){
                        for(int j=0;j<28;j++){
                            RevImg2[j*28+28-i]=BufferImage1[28*i+j];
                        }
                    }
                    //*/

                //    this code is for the camera input
                     int start=40*240;
                     for(int i=0;i<240;i++){
                         for(int j=0;j<240;j++){
                             // RevImg2[(239-j)*240+i]=GrayScale[240*i+j+start];

                             //this is for rotated grayscale image
                            //  RevImg2[240*i+j]=GrayScale[240*i+j+start];
                             RevImg2[240*i+j]=GrayScale[320*(239-j)+40+i];
                         }
                     }

                    
#if QUANT && HELIUM
                    time = face_detection_inference_time; // ms
#else
    time = face_detection_inference_time / 1000; // ms

#endif 
                    

                    

                    // xSemaphoreGiveFromISR(g_start_menu_binary_semaphore , pdFALSE );

                    

                    


                    
                    // e_printf("%d Capture",CaptureFlag);
                    // SCB_CleanDCache();
                    bsp_camera_capture_image();

                    CaptureFlag=0;




                
                


                }

                xSemaphoreGive(g_start_menu_binary_semaphore);
                SCB_CleanDCache();
                vTaskDelay(30); 

        }
        }
        
        


    
    // vTaskDelay(20);
    // SCB_CleanDCache();


    
    /* Wait for previous frame rendering to finish, then finalize this frame and flip the buffers */
    d2_flushframe(d2_handle);
    graphics_end_frame();
    // SCB_DisableDCache();
    // SCB_CleanDCache();


}

#endif // USE_SIMULATED_AI_DATA

