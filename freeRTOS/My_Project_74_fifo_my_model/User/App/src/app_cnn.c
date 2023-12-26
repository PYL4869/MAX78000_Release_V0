#include "bsp.h"

volatile uint32_t cnn_time;

/**
 * @description: CNN使能
 * @return {*}
 */
int App_Cnn_Init(void)
{
  /* 使能CNN时钟,设置为50MHZ */
  cnn_enable(MXC_S_GCR_PCLKDIV_CNNCLKSEL_PCLK, MXC_S_GCR_PCLKDIV_CNNCLKDIV_DIV1);
  /* 打开升压电路 */
  cnn_boost_enable(MXC_GPIO2, MXC_GPIO_PIN_5);
  /* Bring CNN state machine into consistent state */
  cnn_init();
  /* 加载CNN内核*/
  cnn_load_weights();
  /* 加载CNN偏置 */
  cnn_load_bias();
  /* 给网络配置加速器 配置状态机 */
  cnn_configure();
  /* 使能CNN时钟 */
  /*     MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_CNN); */

  return MAX_OK;
}


/**
 * @description:执行推理
 * @return {*}
 */
void App_Cnn_Execute(void)
{
/*   MXC_TFT_ClearScreen(); */
  cnn_start();
  App_Camera_Cnn_Get();

  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
  while (cnn_time == 0)
  {
    __WFI(); // Wait for CNN interrupt
  }

  get_priors();
  localize_objects();
  cnn_stop();
  xSemaphoreGive(xSemaphore);
/*   MXC_Delay(SEC(1)); */
/*   App_Mc800m_Get_Time(); */
}