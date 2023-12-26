#ifndef _BSP_H_
#define _BSP_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "mxc_device.h"
#include "mxc_delay.h"
#include "icc.h"
#include "mxc_sys.h"
#include "tft_ili9341.h"
#include "nvic_table.h"

#include "camera.h"
#include "dma.h"
#include "uart.h"
#include "ff.h"



/* bsp头文件 */
#include "bsp_led.h"
#include "bsp_button.h"
#include "bsp_uart.h"


/* App头文件 */
#include "app_tft.h"
#include "app_camera.h"
#include "app_cnn.h"
#include "app_ec800m.h"
#include "app_sd.h"

/* Cnn头文件 */
#include "cnn.h"
#include "weights.h"
#include "post_process.h"

/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "portmacro.h"
#include "task.h"
#include "semphr.h"
#include "FreeRTOS_CLI.h"

typedef enum 
{
  MAX_IDLE     = -1,
  MAX_OK       = 0x00U,
  MAX_ERROR    = 0x01U,
  MAX_BUSY     = 0x02U,
  MAX_TIMEOUT  = 0x03U
} MAX_StatusTypeDef;

extern SemaphoreHandle_t xSemaphore;

void Max78000_Init(void);
void Max78000_Open_Init(void);
void Bsp_Init(void);

#endif

