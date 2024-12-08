/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : menu_ns.c
 * Version      : .
 * Description  : The next steps screen display.
 *********************************************************************************************************************/

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
#include "graphics/graphics.h"

#include "r_glcdc.h"
#include "r_glcdc_cfg.h"

#include "bg_font_18_full.h"

#include "touch_ft5x06.h"
#include "gimp.h"


#include "VVRLogo.h"

/*  USE THIS define below to FORCE required page after splash screen */
//#define OVERRIDE_ACTIVE_SCREEN (AI_OBJECT_RECONITION)
#define OVERRIDE_ACTIVE_SCREEN (AI_FACE_RECONITION)

#define CONNECTION_ABORT_CRTL    (0x00)
#define MENU_EXIT_CRTL           (0x20)

#define MODULE_NAME     "\r\n%d. VARIOUS INTERACTIVE CONNECTIVITY & HMI DEMOS\r\n\r\n"


#define SUB_OPTIONS     "To explore various Interactive Connectivity & HMI Demos\r\n\r\n" \
                        "a) Please connect the Graphic and Camera Expansion Boards including the EK-\r\n" \
                        "   RA8D1 kit to the EK-RA8D1 board.\r\n" \
                        "b) Please connect the Ethernet cable included in the EK-RA8D1 kit to the EK-\r\n" \
                        "   RA8D1 boards and Internet router/switch.\r\n" \
                        "c) Reset the kit and follow the instructions.\r\n\r\n" \

#define EP_INFO         "(1)"

extern char_t g_print_buffer[];

extern bool_t init_ts(void);
extern void test_tp();
extern void enable_ts(void);
extern void clear_touch(void);


extern void  do_led_screen(void);


#define SPLASH_SCREEN        (0)
#define MAIN_SCREEN          (1)
#define MENU_SCREEN          (2)
#define HELP_SCREEN          (3)
#define KIS_SCREEN           (4)
#define CAM_SCREEN           (5)
#define LED_SCREEN           (6)
#define WEATHER_SCREEN       (7)
#define AI_FACE_RECONITION   (8)
#define AI_OBJECT_RECONITION (9)

#define SLIDESHOW_MODE      (42)
#define SLIDESHOW_AUTO_MODE (48)

#define MAX_SCRRENS_FOR_DEMO 28

st_image_data_t slideshow_images[] =
{
     GI_SPLASH_SCREEN,
     GI_MAIN_SCREEN,
     MODE_SLIDE_2,
     MODE_SLIDE_3,
     MODE_SLIDE_4,
     MODE_SLIDE_5,
     MODE_SLIDE_6,
     MODE_SLIDE_7,
     MODE_SLIDE_8,
     MODE_SLIDE_8,
     MODE_SLIDE_9,
     MODE_SLIDE_10,
     MODE_SLIDE_11,
     MODE_SLIDE_12,
     MODE_SLIDE_13,
     MODE_SLIDE_14,
     MODE_SLIDE_15,
     MODE_SLIDE_16,
     MODE_SLIDE_17,
     MODE_SLIDE_18,
     MODE_SLIDE_19,
     MODE_SLIDE_20,
     MODE_SLIDE_21,
     MODE_SLIDE_22,
     MODE_SLIDE_23,
     MODE_SLIDE_24,
     MODE_SLIDE_25,
     GI_AI_FACE_RECONITION,
     GI_AI_OBJECT_DETECTION,
     MODE_SLIDE_28,
     MODE_SLIDE_29,
//     MODE_SLIDE_30,
     GI_MAIN_SCREEN,
     GI_MAIN_SCREEN,
     GI_MAIN_SCREEN,
};

char slideshow_titles[][32] =
{
 "Splash",                      //  1
 "Welcome",                     //  2
 "Welcome menu 1",              //  3
 "Welcome menu 2",              //  4
 "Kit Information",             //  5
 "Getting Started",             //  6
 "Credits",                     //  7
 "User Led Control",            //  8
 "Currency base",               //  9
 "World Timezones",             // 10
 "Hong Kong",                   // 11
 "Kyoto",                       // 12
 "London",                      // 13
 "Miami",                       // 14
 "Munich",                      // 15
 "New York",                    // 16
 "Paris",                       // 17
 "Prague",                      // 18
 "Queenstown",                  // 19
 "Rio De Janeiro",              // 20
 "Rome",                        // 21
 "San Francisco",               // 22
 "Shanghai",                    // 23
 "Singapore",                   // 24
 "Sydney",                      // 25
 "Toronto",                     // 26
 "AI App 1",                    // 27
 "AI App 2",                    // 28
 "AI App 3",                    // 29
 "Weather Mock-up",             // 30
 "",
};

extern st_gimp_weather_image_t test_weather_text;

#define AI_DETECTION_ENABLED
#ifdef AI_DETECTION_ENABLED
// extern uint32_t g_ai_detection_events;
// extern st_ai_detection_point_t g_ai_detection[5];
#else
uint32_t g_ai_detection_events;
st_ai_detection_point_t g_ai_detection[5];
#endif

extern uint8_t fb_foreground[1][DISPLAY_BUFFER_STRIDE_BYTES_INPUT1 * DISPLAY_VSIZE_INPUT1];
extern void  do_face_reconition_screen(void);
extern void  do_object_detection_screen(void);

//#define CAM_DEMO (1)
volatile uint8_t * live_bg_ptr  = (uint8_t *)fb_background[1];
volatile uint8_t * upd_bg_ptr   = (uint8_t *)fb_background[0];

#if 0
extern bool update_frame;

static bool_t lcd_init = false;
static volatile bool update_fps = false;
static volatile uint32_t g_fps_counter_cycles = 0;
static float precision_x = 0;
static float precision_y = 0;
#endif

extern touch_data_t g_touch_data;

#define OFF                                     (0U)
#define ON                                      (1U)

extern void init_sdram (void);
extern void glcdc_init();


extern void  reset_transition(void);
extern bool_t in_transition(void);
extern void show_menu_icon();
extern void process_main_menu();

#define NUM_WEATHER_IMAGE_ICONS (2)


uint32_t demo_screen = SPLASH_SCREEN; // DAV/2D SPLASH SCREEN

//uint32_t slide_show = 0; // DAV/2D SPLASH SCREEN
//uint32_t slide_show = 20; // Weather
uint32_t slide_show = 7; // LED

#define NUM_VIEWER_IMAGES 16
#define REFRESH_RATE 8


/* RESOLUTION FROM CAMERA */
#define CAM_IMG_SIZE_X   320
#define CAM_IMG_SIZE_Y   238  // Trim the Right Hand Edge hiding corruption

//#define FULL_SCREEN 1

#ifdef FULL_SCREEN
// full screen
#define CAM_LAYER_SIZE_X 480 // 000 --> 480
#define CAM_LAYER_SIZE_Y 854 // 000 --> 854


#else
// normal screen
#define CAM_LAYER_SIZE_X 476 // 000 --> LCD_VPIX
#define CAM_LAYER_SIZE_Y 360 // 000 --> 480


// Full screen keeping asoect raition
//#define CAM_LAYER_SIZE_X 480 // 000 --> LCD_VPIX
//#define CAM_LAYER_SIZE_Y 600 // 000 --> 480

#endif

bool_t show_bg = true;

// extern uint8_t bsp_camera_out_buffer565 [];
extern uint32_t get_image_data(st_image_data_t ref);
extern bool_t camera_init(bool_t use_test_mode);

static bool_t g_camera_initialised = false;
static bool_t g_ts_initialised  = false;
// static uint32_t g_transition_frame = 2;
static uint32_t g_transition_frame = 1;

 void  reset_transition(void)
{
    g_transition_frame = 3;
    // g_transition_frame = 5;
}

 bool_t in_transition(void)
{
    if(g_transition_frame > 0)
    {
//        TURN_RED_ON;
        g_transition_frame--;
        return (true);
    }
    else
    {
//        TURN_RED_OFF;

        /* Re-enable IRQn interrupt once payload reception completes */
        R_ICU_ExternalIrqEnable((void *)&g_external_irq3_ctrl);

        return (false);
    }
}

static uint32_t select_demo(uint32_t current)
{
    uint32_t next = current;
    EventBits_t s_ux_bits;

    s_ux_bits = xEventGroupGetBitsFromISR (g_update_console_event);

    if ((s_ux_bits & (STATUS_HMI_NEXT_DEMO)) == (STATUS_HMI_NEXT_DEMO))
    {
        if(next != 0)
        {
            next--;
        }

        /* Clear Event */
        xEventGroupClearBits (g_update_console_event, (STATUS_HMI_NEXT_DEMO));
    }


    if ((s_ux_bits & (STATUS_HMI_PREV_DEMO)) == (STATUS_HMI_PREV_DEMO))
    {
        if(next != MAX_SCRRENS_FOR_DEMO)
        {
            next++;
        }

        /* Clear Event */
        xEventGroupClearBits (g_update_console_event, (STATUS_HMI_PREV_DEMO));
    }

    return next;
}

static bool_t menu_in_use   = false;
static bool_t previous_menu = 1;
bool_t do_detection = false;
bool_t do_classification = false;

static void log_touch_points(void)
{
    if(g_touch_data.num_points > 0)
    {
        for(uint8_t np = 0; np < g_touch_data.num_points; np++)
        {
            if(np > 0)
            {
                print_to_console(": ");
            }

            sprintf(g_print_buffer, "tp [%d] [%03d,%03d]", np , g_touch_data.point[np].x, g_touch_data.point[np].y);
            print_to_console(g_print_buffer);
        }
        print_to_console("\r\n");
    }
}


void process_main_menu()
{
    // Check for show mwnu)
    /*
    if(g_touch_data.num_points > 0)
    {
        uint8_t active_tp = g_touch_data.num_points - 1;
        bool_t valid = false;

        for(uint8_t np = 0; np < g_touch_data.num_points; np++)
        {

            
            if(menu_in_use == true)
            {

                    if( ( g_touch_data.point[active_tp].x < 374 ) && (g_touch_data.point[active_tp].y < 85 ) &&
                        ( g_touch_data.point[active_tp].x > 20  ) && (g_touch_data.point[active_tp].y > 15 ) )

                    {
                        // Exit menu
                        valid = true;
                        demo_screen = MAIN_SCREEN;
                    }
                    else
                    {

                        // TOP LEFT KIS SCREEN
                        if( ( g_touch_data.point[active_tp].x < 865 ) && (g_touch_data.point[active_tp].y < 190 ) &&
                            ( g_touch_data.point[active_tp].x > 850 ) && (g_touch_data.point[active_tp].y > 15  ) )
                        {
                            // KIS menu
                            valid = true;
                            demo_screen = KIS_SCREEN;
                        }

                        // TOP COL 4 WEATHER SCREEN
                        if( ( g_touch_data.point[active_tp].x < 350 ) && (g_touch_data.point[active_tp].y < 190 ) &&
                            ( g_touch_data.point[active_tp].x > 300  ) && (g_touch_data.point[active_tp].y > 15  ) )
                        {
                            // Weather menu
                            valid = true;
                            demo_screen = WEATHER_SCREEN;
                        }

                        // BOTTOM COL 4 HELP
                         if( ( g_touch_data.point[active_tp].x < 350 ) && (g_touch_data.point[active_tp].y < 240 ) &&
                             ( g_touch_data.point[active_tp].x > 300 ) && (g_touch_data.point[active_tp].y > 190  ) )
                         {
                             // HELP menu
                             valid = true;
                             demo_screen = HELP_SCREEN;
                         }

                        // BOTTOM COL 3 AI  SCREEN
                         if( ( g_touch_data.point[active_tp].x < 300 ) && (g_touch_data.point[active_tp].y < 240 ) &&
                             ( g_touch_data.point[active_tp].x > 200 ) && (g_touch_data.point[active_tp].y > 190  ) )
                         {
                             // AI menu
                             valid = true;

                             demo_screen = AI_FACE_RECONITION;

#if 1
                          
                            //  memset(&g_ai_detection[0], 0, sizeof(g_ai_detection));
                            //  xEventGroupSetBits(g_update_console_event, STATUS_START_ENABLE_AI_DEMO);
#endif
                         }

                        // BOTTOM COL 2 CAMERA  SCREEN
                         if( ( g_touch_data.point[active_tp].x < 190 ) && (g_touch_data.point[active_tp].y < 240 ) &&
                             ( g_touch_data.point[active_tp].x >  90 ) && (g_touch_data.point[active_tp].y > 190  ) )
                         {
                             // CAMERA menu
                             valid = true;
                             demo_screen = CAM_SCREEN;
                         }


                        // BOTTOM LEFT LED SCREEN
                        if( ( g_touch_data.point[active_tp].x < 865 ) && (g_touch_data.point[active_tp].y < 240 ) &&
                             ( g_touch_data.point[active_tp].x > 850 ) && (g_touch_data.point[active_tp].y > 190  ) )
                        {
                            // LEDS menu
                            valid = true;
                            demo_screen = LED_SCREEN;
                        }

                    }

                    if (valid == true)
                    {
                        reset_transition();
                        menu_in_use = false;
                        return;
                    }
            }
            else
            {
               
                if( ( g_touch_data.point[active_tp].x < 380 ) && (g_touch_data.point[active_tp].y < 120 ) &&
                    ( g_touch_data.point[active_tp].x > 20  ) && (g_touch_data.point[active_tp].y > 15 ) )
                {
                    reset_transition();
                    previous_menu = demo_screen;
                    demo_screen = MENU_SCREEN;
                    menu_in_use = true;
                    clear_touch();
                }
            }
        }
    }
    */


    bool valid = true;

    demo_screen = AI_FACE_RECONITION;
    xEventGroupSetBits(g_update_console_event, STATUS_START_ENABLE_AI_DEMO);
    if (valid == true)
    {
        reset_transition();
        menu_in_use = false;
        return;
    }

    /* Re-enable IRQn interrupt once payload reception completes */
    R_ICU_ExternalIrqEnable((void *)&g_external_irq3_ctrl);

}

void show_menu_icon()
{
    /* Draw hamburger menu icon */
    d2_setcolor(d2_handle, 0, 0xFFFFFF);
    d2_setalpha(d2_handle, 0xFF);
    d2_setlinecap(d2_handle, d2_lc_round);

    {
        for(uint32_t i = 0; i < 3; i++)
        {
            d2_point mx = (d2_point) (8 << 4);
            d2_point my = (d2_point) ((450 - (17 * i)) << 4);
            d2_renderline(d2_handle, my, (mx), my, ((mx) + (50 << 4)), 7 << 4, 0);
        }
    }
}

static void  draw_menu_screen(void)
{
    st_gimp_image_t * img = NULL;

    /* Wait for vertical blanking period */
    graphics_wait_vsync();
    graphics_start_frame();

    if(in_transition())
    {
        img = (st_gimp_image_t *)get_image_data(GI_MAIN_MENU);
        d2_setblitsrc(d2_handle, img->pixel_data, 280, 280, 471, EP_SCREEN_MODE);

        d2_blitcopy(d2_handle,
                280,471,  // Source width/height
                (d2_blitpos) 0, 0, // Source position
                (d2_width) ((280) << 4), (d2_width) ((471) << 4), // Destination width/height
                (190 << 4), (385 << 4),  // Destination position
                d2_tm_filter);
    }
    else
    {
        process_main_menu();

    }

    /* Wait for previous frame rendering to finish, then finalize this frame and flip the buffers */
    d2_flushframe(d2_handle);
    graphics_end_frame();
}

static void  do_splash_screen(void)
{
    st_gimp_image_t * img = NULL;

    /* Wait for vertical blanking period */
    graphics_wait_vsync();
    graphics_start_frame();

    if(in_transition())
    {
        


        d2_clear(d2_handle,0xFFFFFF);

        // d2_setblitsrc(d2_handle, LogoBig, 400, 400, 400, 1);
        d2_setblitsrc(d2_handle, VVRLogo, 400, 400, 400, 1);
        // d2_setblitsrc(d2_handle, GrayScale, 240, 240, 320, d2_mode_alpha8);


        d2_blitcopy(d2_handle,
                400, 400,  // Source width/height
                (d2_blitpos) 0, 0, // Source position
                (d2_width) ((400) << 4), (d2_width) ((400) << 4), // Destination size width/height
                (d2_width) (((480 - 38 - 400)/2) << 4), (d2_width) (((LCD_VPIX - 400)/2) << 4), // Destination offset position
                d2_tm_filter);
        

    }
    else
    {
        process_main_menu();
    }

    /* Wait for previous frame rendering to finish, then finalize this frame and flip the buffers */
    d2_flushframe(d2_handle);
    graphics_end_frame();

}


extern uint32_t get_sub_image_data(st_image_data_t ref, uint32_t sub_image);

static st_gimp_weather_image_t sdram_weather_images[32] BSP_PLACE_IN_SECTION(".sdram") = {};
static bool_t sdram_loaded = false;



#define SLIDE_SHOW_RESET (60)
#define SLIDE_SHOW_RESET_WEATHER (150)

st_gimp_image_t * new_img = NULL;
static uint32_t g_slide_show_auto_current = 0;
static uint32_t g_slide_show_auto_timer   = SLIDE_SHOW_RESET;
static uint32_t g_led_control_frame = 0;
static bool_t   g_led_done_it_once  = false;

static uint32_t g_ai_control_frame = 0;

static bool_t doneit_once = false;
static display_runtime_cfg_t glcd_layer_change;

#define WEATHER_ICON_HOFFSET (308)
#define WEATHER_ICON_VOFFSET ( 48)

static void  draw_with_alpha(st_gimp_weather_image_t *object, uint16_t hoffset, uint16_t voffset)
{
    /* GLCDC Graphics Layer 1 size must be 182 x 200 */
    uint8_t * buf_ptr = (uint8_t *)fb_foreground;
    display_input_cfg_t const *p_input   = &g_display0.p_cfg->input[1];  // Layer 2

    /* clear screen */
//    memset(buf_ptr, 0, p_input->hstride * p_input->vsize * 4);

    /* offset 200 horizontal */
    buf_ptr += (hoffset * (p_input->hstride * 4)); // Horizontal offset
    buf_ptr += (voffset * 4); // Vertical offset

    uint32_t bpp = object->bytes_per_pixel;

    for(uint32_t v = 0; v < object->height; v++)
    {
        uint32_t v_offset = v*object->width*bpp;
        for(uint32_t h = 0; h < object->width*bpp; h+=bpp)
            {
            buf_ptr[0] = object->pixel_data[v_offset+h+2]; // B
            buf_ptr[1] = object->pixel_data[v_offset+h+1]; // G
            buf_ptr[2] = object->pixel_data[v_offset+h];   // R
            buf_ptr[3] = object->pixel_data[v_offset+h+3]; // A                                     // A (Unused)
            buf_ptr+=4;
            }
        buf_ptr += ((p_input->hstride - p_input->hsize ) * 4);

        /* align image size to be drawn with the surface size */
        buf_ptr += ((p_input->hsize -  object->width) * bpp);
    }
}

static uint32_t do_transition_nframes = 0;
#define START_OF_WEATHER_ICON (10)
#define END_OF_WEATHER_ICON   (27)


extern void reenable_backlight_control(void);

/**********************************************************************************************************************
 * Function Name: camview_display_menu
 * Description  : .
 * Return Value : The next steps screen.
 *********************************************************************************************************************/
test_fn camview_display_menu(bool_t first_call)
{

    int16_t c = -1;
    uint32_t ss_timer = 50;

    sprintf(g_print_buffer, "%s%s", gp_clear_screen, gp_cursor_home);
    print_to_console(g_print_buffer);

//    if(first_call == false)
    {
        // Select text foreground
        sprintf(g_print_buffer, "%s", gp_white_fg);
        print_to_console(g_print_buffer);

        sprintf(g_print_buffer, MODULE_NAME, g_selected_menu);
        print_to_console(g_print_buffer);
    }

    memset(fb_foreground, 0, DISPLAY_BUFFER_STRIDE_BYTES_INPUT1 * DISPLAY_VSIZE_INPUT1);

    /* Initialize D/AVE 2D driver */
    d2_handle = d2_opendevice(0);
    d2_inithw(d2_handle, 0);

    /* Clear both buffers */
    d2_framebuffer(d2_handle, fb_background, LCD_HPIX, LCD_STRIDE, LCD_VPIX * LCD_BUF_NUM, EP_SCREEN_MODE);
    d2_clear(d2_handle, 0x00000000);

    /* Set various D2 parameters */
    d2_setblendmode(d2_handle, d2_bm_alpha, d2_bm_one_minus_alpha);
    d2_setalphamode(d2_handle, d2_am_constant);
    d2_setalpha(d2_handle, 0xff);
    d2_setantialiasing(d2_handle, 1);
    d2_setlinecap(d2_handle, d2_lc_butt);
    d2_setlinejoin(d2_handle, d2_lj_none);

    /* true  = use live camera feed */
    /* false = use test_mode */
    if(g_camera_initialised == false)
    {
        g_camera_initialised = camera_init(true);
        g_ts_initialised     = init_ts();
    }

    g_ts_initialised = true;    // hack for ts not working with fsp 5.0.0

    if((g_camera_initialised == true) && (g_ts_initialised == true))
    {
        print_to_console((void *)"Interact with the various HMI demos using the attached screen\r\n");

        sprintf(g_print_buffer, MENU_RETURN_INFO);
        print_to_console(g_print_buffer);

        if(first_call == true)
        {
#if 0
                             /* Start AI Demo */
                             memset(&g_ai_detection[0], 0, sizeof(g_ai_detection));
                             xEventGroupSetBits(g_update_console_event, STATUS_START_ENABLE_AI_DEMO);
#endif
            /* copy font into SD_ram TODO */
            initialise_bg_font_18_full();
            glcdc_init();

            test_tp(); /* Reset is performed in here. */

            /* Initialize and start display output */
            fsp_err_t err = display_mipi_ili9806e_init(&g_display0);
            if(FSP_SUCCESS != err)
            {
                /* Set red LED */
                R_BSP_PinWrite(g_bsp_leds.p_leds[BSP_LED_LED3], BSP_IO_LEVEL_HIGH);
            }

            //    TS setup
            enable_ts();
        }

        bool_t blinky = OFF;

        // start_key_check();
       demo_screen = SPLASH_SCREEN;
       reset_transition();

       /* Allow display backlight to enable  */
       reenable_backlight_control();

       R_ICU_ExternalIrqEnable((void *)&g_external_irq3_ctrl);

        while(1)
        {
            // e_printf("Inside camview_display_menu");
            // e_printf("%d!!!!!!!!!!!!!!!!!!!!!!!!!!",demo_screen);
            /* This task delay- allows other threads (ie AI) to run in parallel with screen */
            // vTaskDelay(10);
            // log_touch_points();

            switch(demo_screen)
            {
                case SPLASH_SCREEN:
                    // e_printf("inside splash");
                    do_splash_screen();
                    if(ss_timer-- == 0)
                    {
                        demo_screen = AI_FACE_RECONITION; // default


    //                        demo_screen = AI_OBJECT_RECONITION;
    //                        demo_screen = AI_FACE_RECONITION;
    //                        demo_screen = SLIDESHOW_AUTO_MODE;
    //                        demo_screen = SLIDESHOW_MODE;
    //                        demo_screen = LED_SCREEN;
    //                        demo_screen =  WEATHER_SCREEN;
    //                        demo_screen = AI_FACE_RECONITION;
                        // /* Start AI Demo  AUTOMATIC START */
                        //  memset(&g_ai_detection[0], 0, sizeof(g_ai_detection));
                       xEventGroupSetBits(g_update_console_event, STATUS_START_ENABLE_AI_DEMO);

                        reset_transition();
                    }
                    break;



                case AI_FACE_RECONITION:
                    // do_detection = true;
                    // do_classification = false;
                    do_face_reconition_screen();
                    break;

    //                 case SLIDESHOW_MODE:
    //                     do_slideshow_screen();
    //                     break;

    //                 case SLIDESHOW_AUTO_MODE:
    //                     do_slideshow_auto();
    //                     break;

    //                 case  AI_OBJECT_RECONITION:
    //                     {
    //                         do_detection = false;
    //                         do_classification = true;
    // //                       do_object_detection_screen();
    //                     }
    //                     break;
                case 99:
                    {
                        e_printf("inside error");

                        do_splash_screen();
                    }
                    break;
            }

            /* This task delay- allows other threads (ie AI) to run in parallel with screen */
            // vTaskDelay(3);
            vTaskDelay(30);
        }

    // DISABLE MENU CONTROL BY Switches

    
//    return (0);
    }
}
/**********************************************************************************************************************
 End of function camview_display_menu
 *********************************************************************************************************************/
