#include "bsp.h"

/**
 * @description: led初始化
 * @return {*}
 */
int Bsp_Led_Init(void)
{
    int retval = MAX_OK;
    mxc_gpio_cfg_t gpio_led_t = {0};
    /* 使能GPIO时钟 */
    MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_GPIO0);
    MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_GPIO1);
    MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_GPIO2);
    /* 配置GPIO引脚状态 */
    gpio_led_t.port = MXC_GPIO2;
    gpio_led_t.mask = MXC_GPIO_PIN_0 | MXC_GPIO_PIN_1 | MXC_GPIO_PIN_2;
    gpio_led_t.func = MXC_GPIO_FUNC_OUT;
    gpio_led_t.pad = MXC_GPIO_PAD_NONE;
    gpio_led_t.vssel = MXC_GPIO_VSSEL_VDDIOH;

    MXC_GPIO_OutSet(MXC_GPIO2, MXC_GPIO_PIN_0 | MXC_GPIO_PIN_1 | MXC_GPIO_PIN_2);

    if (MXC_GPIO_Config(&gpio_led_t) != MAX_OK)
    {
        retval = E_UNKNOWN;
    }
    return retval;
}
/**
 * @description: led亮
 * @param {uint8_t} i 哪一个灯亮
 * @return {*}
 */
void Bsp_Led_Open(uint8_t i)
{
    if (i == 0)
    {
        MXC_GPIO2->out_clr = MXC_GPIO_PIN_0;
    }
    else if (i == 1)
    {
        MXC_GPIO2->out_clr = MXC_GPIO_PIN_1;
    }
    else if (i == 2)
    {
        MXC_GPIO2->out_clr = MXC_GPIO_PIN_2;
    }
}
/**
 * @description: led灭
 * @param {uint8_t} i 哪一个灯灭
 * @return {*}
 */
void Bsp_Led_Off(uint8_t i)
{
    if (i == 0)
    {
        MXC_GPIO2->out_set = MXC_GPIO_PIN_0;
    }
    else if (i == 1)
    {
        MXC_GPIO2->out_set = MXC_GPIO_PIN_1;
    }
    else if (i == 2)
    {
        MXC_GPIO2->out_set = MXC_GPIO_PIN_2;
    }
}
/**
 * @description: led闪烁
 * @param {uint8_t} i 哪一个灯闪烁
 * @return {*}
 */
void Bsp_Led_Toggle(uint8_t i)
{
    if (i == 0)
    {
        MXC_GPIO2->out ^= MXC_GPIO_PIN_0;
    }
    else if (i == 1)
    {
        MXC_GPIO2->out ^= MXC_GPIO_PIN_1;
    }
    else if (i == 2)
    {
        MXC_GPIO2->out ^= MXC_GPIO_PIN_2;
    }
}
