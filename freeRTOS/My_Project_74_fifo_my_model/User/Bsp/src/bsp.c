#include "bsp.h"

/* 图像 */
/* int image_bitmap_1 = (int)&img_1_rgb565[0]; */

SemaphoreHandle_t xSemaphore = NULL;

/**
 * @description: MAX78000初始化函数
 * @return {*}
 */
void Max78000_Init(void)
{
    /* 等待PMIC可用，进行电源管理 */
    MXC_Delay(200000);
    /* 使能cache */
    MXC_ICC_Enable(MXC_ICC0);

    /* 使能时钟设置为100MHz */
    MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);

    /* 禁用RV32时钟 */
/*     MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_CPU1); */
    SystemCoreClockUpdate();    
    
/*     MXC_LP_EnterSleepMode(); */
}

/**
 * @description: MAX78000开机和EC800M上电
 * @return {*}
 */
void Max78000_Open_Init(void)
{
/*     MXC_TFT_ShowImage(0, 0, image_bitmap_1); */
    MXC_Delay(1000000);
    MXC_TFT_ClearScreen();

    MXC_Delay(SEC(12));
    App_Ec800m_Connect();
    MXC_Delay(1000);

}

/**
 * @description: Bsp初始化函数
 * @return {*}
 */
void Bsp_Init(void)
{
    Max78000_Init();
    Bsp_Led_Init();
    Bsp_Button1_Init();
    Bsp_Button2_Init();
    Bsp_Uart_Init();

    App_Tft_Init();
    App_Camera_Init();
    App_Sd_Init();
    App_Ec800m_Init();
    App_Cnn_Init();

    MXC_GPIO2->out_clr = MXC_GPIO_PIN_0;
    MXC_GPIO2->out_clr = MXC_GPIO_PIN_1;
    MXC_GPIO2->out_clr = MXC_GPIO_PIN_2;

}
