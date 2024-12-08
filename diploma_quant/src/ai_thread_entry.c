 #include <stdarg.h>
#include "ai_thread.h"
#include "common_data.h"
#include "common_init.h"
#include "common_utils.h"
#include "menu_camview.h"
#include "timer.h"


#define MENU_EXIT_CRTL           (0x20)


static char_t s_print_buffer[BUFFER_LINE_LENGTH] = {};
static char_t myBuffer[160000] BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT) BSP_PLACE_IN_SECTION(".sdram") ={};
// static char_t myBuffer[160000] BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT) BSP_PLACE_IN_SECTION(".sdram") ={};
extern int two_apps (bool_t, bool_t);
// extern bool g_capture_ready;
extern int32_t checkCameraOverFlow;
// extern int CaptureFlag;
SemaphoreHandle_t CameraFlag= NULL;;



int e_printf(const char *format, ...)
{
#if 1
    va_list args;
    va_start(args, format);
    int result = vsprintf(s_print_buffer, format, args);
    va_end(args);
    sprintf(s_print_buffer, "%s\r\n", s_print_buffer);
    print_to_console((void*)s_print_buffer);
    return result;
#endif
    return 0;
}

void myPrintf(uint8_t *array, int size, ...){
    va_list args;
    va_start(args, size);
    vsprintf(myBuffer, "%d", args); // Use vsprintf to format the string
    int cnt=0;
    for(int i=0;i<size;i++){
        int t=array[i]%10;
        int t1=(array[i]%100)/10;
        int t2=array[i]/100;


        if(t2>0){
            myBuffer[cnt]='0'+t2;
            cnt++;
        }
        if(t1>0){
            myBuffer[cnt]='0'+t1;
            cnt++;
        }
        if(t>0){
            myBuffer[cnt]='0'+t;
            cnt++;
        }

        myBuffer[cnt]='\n';
        cnt++;
    }
     myBuffer[cnt]='\0';

    // Append the array elements to the string
    // for (int i = 0; i < size; ++i) {
    //     sprintf(myBuffer + strlen(myBuffer), "%d ", array[i]);
    // }
    // sprintf(myBuffer+ strlen(myBuffer), "\r\n");

    // va_end(args);

    print_to_console((void*)myBuffer);
}

/**********************************************************************************************************************
 * Function Name: main_display_menu
 * Description  : .
 * Return Value : The main menu controller.
 *********************************************************************************************************************/
/* AI Thread entry function */
/* pvParameters contains TaskHandle_t */
void ai_thread_entry(void *pvParameters)
{
    e_printf("Temp");


    FSP_PARAMETER_NOT_USED (pvParameters);
    timer_init();

    // xSemaphoreGiveFromISR(g_start_menu_binary_semaphore);

    // g_start_menu_binary_semaphore= xSemaphoreCreateBinary();
    // e_printf("camera flag132 %d %d",g_start_menu_binary_semaphore,xSemaphoreTake(g_start_menu_binary_semaphore,  pdMS_TO_TICKS ( 500u )));

	vTaskDelay(3000);
    while (1)
    {


        xSemaphoreGive(g_start_menu_binary_semaphore);

    	
    
        two_apps(do_detection, do_classification);
        
        
    }
}




