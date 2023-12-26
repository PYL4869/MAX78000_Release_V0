#include "bsp.h"

mxc_gpio_cfg_t gpio_button1_t = {0};
mxc_gpio_cfg_t gpio_button2_t = {0};



char test_3[] = "99999\r\n";

/**
 * @description: Button1中断服务函数
 * @param {void*} n
 * @return {*}
 */
void Bsp_Button1_Callback(void)
{
    /* 关中断 */
    MXC_GPIO_DisableInt(MXC_GPIO0, MXC_GPIO_PIN_2);
    MXC_GPIO_RegisterCallback(&gpio_button1_t, NULL, NULL);
    /* 填写中断执行函数 */

    Bsp_Led_Open(RED_LED_OPEN);
    Bsp_Led_Off(GREEN_LED_OFF);
    Bsp_Led_Off(BLUE_LED_OFF);
    /* 按键按下事件 */

/*     App_Mc800m_Get_Time(); */
/*     App_Sd_Write(test_3); */
/*     App_Mc800m_Reporting_Attribute_1(test_1,test_2); */


/*     App_Mc800m_Disconnect_Mqtt(); */



    /*     strcat(strcpy(file_name, aaa[1]), "/date_num.txt"); */
/*     App_Sd_Write(sd_card_time); */
/*     Bsp_Uart_Transmit((uint8_t *)BUFF88, sizeof(BUFF88));  */
/*     aaa++; */
/*     a = 1;
    App_Mc800m_Reporting_Attribute_1(aaaa, aaa); */
    /* Bsp_Uart_Transmit((uint8_t *)BUFF14, sizeof(BUFF14));  */

    /* 开中断 */
    MXC_GPIO_IntConfig(&gpio_button1_t, MXC_GPIO_INT_FALLING);
    MXC_GPIO_EnableInt(MXC_GPIO0, MXC_GPIO_PIN_2);
    MXC_GPIO_RegisterCallback(&gpio_button1_t, (void *)Bsp_Button1_Callback, NULL);
    NVIC_EnableIRQ(MXC_GPIO_GET_IRQ(MXC_GPIO_GET_IDX(MXC_GPIO0)));
}

void Bsp_Button2_Callback(void)
{
    /* 关中断 */
    MXC_GPIO_DisableInt(MXC_GPIO1, MXC_GPIO_PIN_7);
    MXC_GPIO_RegisterCallback(&gpio_button2_t, NULL, NULL);
    /* 填写中断执行函数 */

    Bsp_Led_Off(RED_LED_OFF);
    Bsp_Led_Off(GREEN_LED_OFF);
    Bsp_Led_Off(BLUE_LED_OFF);
    /* 按键按下事件 */



    App_Mc800m_Disconnect_Mqtt();



/*     App_Ec800m_Connect(); */


    /*     Bsp_Uart_Transmit((uint8_t *)BUFF4, sizeof(BUFF4)); */
    /*     App_Sd_Write(water_num); */
/*     Bsp_Uart_Transmit((uint8_t *)BUFF88, sizeof(BUFF88)); */
/*    if( a == 0)
   {
        App_Ec800m_Connect();
   }
   else if( a == 1)
   { */
/*         Bsp_Uart_Transmit((uint8_t *)BUFF88, sizeof(BUFF88)); */
/*    } */

 /*    App_Mc800m_Reporting_Attribute_1(aaaa, aaa); */


    /* 开中断 */
    MXC_GPIO_IntConfig(&gpio_button2_t, MXC_GPIO_INT_FALLING);
    MXC_GPIO_EnableInt(MXC_GPIO1, MXC_GPIO_PIN_7);
    MXC_GPIO_RegisterCallback(&gpio_button2_t, (void *)Bsp_Button2_Callback, NULL);
    NVIC_EnableIRQ(MXC_GPIO_GET_IRQ(MXC_GPIO_GET_IDX(MXC_GPIO1)));
}
/**
 * @description:
 * @return {*}
 */
int Bsp_Button1_Init(void)
{
    gpio_button1_t.port = MXC_GPIO0;
    gpio_button1_t.mask = MXC_GPIO_PIN_2;
    gpio_button1_t.pad = MXC_GPIO_PAD_PULL_UP;
    gpio_button1_t.func = MXC_GPIO_FUNC_IN;
    gpio_button1_t.vssel = MXC_GPIO_VSSEL_VDDIOH;
    MXC_GPIO_Config(&gpio_button1_t);
    MXC_GPIO_RegisterCallback(&gpio_button1_t, (void *)Bsp_Button1_Callback, NULL);
    MXC_GPIO_IntConfig(&gpio_button1_t, MXC_GPIO_INT_FALLING);
    MXC_GPIO_EnableInt(gpio_button1_t.port, gpio_button1_t.mask);
    NVIC_EnableIRQ(MXC_GPIO_GET_IRQ(MXC_GPIO_GET_IDX(MXC_GPIO0)));

    return MAX_OK;
}

int Bsp_Button2_Init(void)
{
    gpio_button2_t.port = MXC_GPIO1;
    gpio_button2_t.mask = MXC_GPIO_PIN_7;
    gpio_button2_t.pad = MXC_GPIO_PAD_PULL_UP;
    gpio_button2_t.func = MXC_GPIO_FUNC_IN;
    gpio_button2_t.vssel = MXC_GPIO_VSSEL_VDDIOH;
    MXC_GPIO_Config(&gpio_button2_t);
    MXC_GPIO_RegisterCallback(&gpio_button2_t, (void *)Bsp_Button2_Callback, NULL);
    MXC_GPIO_IntConfig(&gpio_button2_t, MXC_GPIO_INT_FALLING);
    MXC_GPIO_EnableInt(gpio_button2_t.port, gpio_button2_t.mask);
    NVIC_EnableIRQ(MXC_GPIO_GET_IRQ(MXC_GPIO_GET_IDX(MXC_GPIO1)));

    return MAX_OK;
}
