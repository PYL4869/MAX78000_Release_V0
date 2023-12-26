#include "bsp.h"

void Bsp_Uart_Handler(void);
void Bsp_Uart_Rx_Callback(mxc_uart_req_t *req, int error);
void Bsp_Uart_Tx_Callback(mxc_uart_req_t *req, int error);
void Bsp_Uart_Rx(uint8_t *Rx_Buff, uint16_t size);
uint8_t Rx_Buff[53] = {0};
mxc_uart_req_t rx_uart_req = {0};

/**
 * @description: LPUART0串口初始化
 * @return {*}
 */
int Bsp_Uart_Init(void)
{
    NVIC_ClearPendingIRQ(MXC_UART_GET_IRQ(3));
    NVIC_DisableIRQ(MXC_UART_GET_IRQ(3));
    MXC_NVIC_SetVector(MXC_UART_GET_IRQ(3), Bsp_Uart_Handler);
    NVIC_EnableIRQ(MXC_UART_GET_IRQ(3));

    MXC_UART_Init(MXC_UART_GET_UART(3), 115200, MXC_UART_APB_CLK);

    Bsp_Uart_Rx(Rx_Buff, sizeof(Rx_Buff));

    NVIC_SetPriority(UART3_IRQn, 3);

    return MAX_OK;
}

/**
 * @description: 以阻塞模式发送一定数量的数据
 * @param {uint8_t} *Tx_Buff 指向数据缓冲区的指针
 * @param {uint16_t} size    要发送的数据元素的数量
 * @return {*}
 */
void Bsp_Uart_Transmit(uint8_t *Tx_Buff, uint16_t size)
{
    rx_uart_req.uart = MXC_UART_GET_UART(3);
    rx_uart_req.txData = Tx_Buff;
    rx_uart_req.txLen = size;
    rx_uart_req.rxLen = 0;
    rx_uart_req.callback = Bsp_Uart_Tx_Callback;

    MXC_UART_Transaction(&rx_uart_req);
    /* 使能中断 */
/*     MXC_UART_EnableInt(MXC_UART_GET_UART(3), ((uint32_t)(0x1UL << 4))); */
}

/**
 * @description: 设置一个中断驱动的UART事务
 * @param {uint8_t} *Rx_Buff  指向数据缓冲区的指针
 * @param {uint16_t} size     要接收的数据元素的数量
 * @return {*}
 */
void Bsp_Uart_Rx(uint8_t *Rx_Buff, uint16_t size)
{
/*     rx_uart_req.uart = MXC_UART_GET_UART(3);
    rx_uart_req.rxData = Rx_Buff;
    rx_uart_req.rxLen = size;
    rx_uart_req.txData = 0;
    rx_uart_req.txLen = 0;
    rx_uart_req.txCnt = 0;
    rx_uart_req.callback = Bsp_Uart_Rx_Callback;

    MXC_UART_TransactionAsync(&rx_uart_req); */
}

/**
 * @description: 中断服务函数 串口接收到一个数据进入一次中断
 * @return {*}
 */
void Bsp_Uart_Handler(void)
{
    MXC_UART_AsyncHandler(MXC_UART_GET_UART(3));
    ec800m_buff_sb.ec800m_len++;
/*     Bsp_Led_Toggle(GREEN_LED_OPEN); */
}

/**
 * @description: 中断回调函数 需要接收字节数填满rx_buff才能执行
 * @return {*}
 */
void Bsp_Uart_Rx_Callback(mxc_uart_req_t *req, int error)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    MXC_UART_TransactionAsync(&rx_uart_req);
    if(ec800m_buff_sb.ec800m_flag == MAX_OK)
    {
        ec800m_buff_sb.ec800m_flag = MAX_IDLE;
        memcpy(ec800m_buff_sb.ec800m_time_buff, Rx_Buff, sizeof(Rx_Buff)); 

        xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
       
    }
    ec800m_buff_sb.ec800m_flag = MAX_IDLE;
    ec800m_buff_sb.ec800m_len = 0;
    memset(Rx_Buff, 0, sizeof(Rx_Buff));
}

/**
 * @description: 中断回调函数 当不满足串口发送条件的时候执行
 * @return {*}
 */
void Bsp_Uart_Tx_Callback(mxc_uart_req_t *req, int error)
{
    Bsp_Uart_Rx(Rx_Buff, sizeof(Rx_Buff));
}
