#include "bsp.h"

/* 变量定义 */
static SemaphoreHandle_t xMutex = NULL;
static TaskHandle_t xHandleTaskLed = NULL;
static TaskHandle_t xHandleTaskEc800m = NULL;
static TaskHandle_t xHandleTaskCnn = NULL;

/* 函数声明 */
static void Task_Create(void);
static void Task_Obj_Create(void);
static void Task_Led(void *pvParameters);

char test_1[] = "6";
char sd_water[] = {0};

/* 已注释掉EC8000与SD卡 */
int main(void)
{
    /* 硬件初始化 */
    Bsp_Init();
    /* 创建任务 */
    Task_Create();
    /* 任务通信初始化 */
    Task_Obj_Create();
    /* 开机 */
    Max78000_Open_Init();
    /* 启动任务调度 */
    vTaskStartScheduler();
    /* 正常启动不会执行到此 */
    MXC_GPIO2->out_clr = MXC_GPIO_PIN_0;
    MXC_GPIO2->out_clr = MXC_GPIO_PIN_1;
    MXC_GPIO2->out_clr = MXC_GPIO_PIN_2;
    while (1)
    {
        App_Cnn_Execute();

        MXC_Delay(SEC(1));

    }
}

static void Task_Led(void *pvParameters)
{
    TickType_t xLastWakeTime;

    /* Get task start time */
    xLastWakeTime = xTaskGetTickCount();

    while (1)
    {
/*                Bsp_Led_Toggle(GREEN_LED_OPEN); */
        vTaskDelayUntil(&xLastWakeTime, 10);
    }
}

static void Task_Cnn(void *pvParameters)
{
    TickType_t xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount();

    while (1)
    {
/*         App_Camera_Get(); */
        App_Cnn_Execute();

        vTaskDelayUntil(&xLastWakeTime, 2000);
    }
}

static void Task_Ec800m(void *pvParameters)
{
    BaseType_t xResult;
    TickType_t xLastWakeTime;
    const TickType_t xMaxBlockTime = pdMS_TO_TICKS(1);
    /* Get task start time */
    xLastWakeTime = xTaskGetTickCount();
    while (1)
    {
        xResult = xSemaphoreTake(xSemaphore, (TickType_t)xMaxBlockTime);
        if (xResult == pdTRUE)
        {
            App_Mc800m_Reporting_Attribute_1(test_1, cnn_result);
            sprintf(sd_water, "%u\r\n", cnn_result);
            App_Sd_Write(sd_water);
            memset(sd_water, 0, sizeof(sd_water));   
/*             Bsp_Led_Toggle(1); */
/*             App_Mc800m_Time_Buff_Rx();
            App_Mc800m_Reporting_Attribute_1(test_1, cnn_result); */
        }
        else
        {
            ;
        }
         
        vTaskDelayUntil(&xLastWakeTime, 2000);
    }
}

static void Task_Create(void)
{   
/*      xTaskCreate(Task_Led,             
                "vTaskLED",        
                512,                  
                NULL,                 
                tskIDLE_PRIORITY + 1, 
               &xHandleTaskLed);   */ 

     xTaskCreate(Task_Cnn,             
                "vTaskCNN",       
                2048,                  
                NULL,                 
                tskIDLE_PRIORITY + 2, 
                &xHandleTaskCnn);     

     xTaskCreate(Task_Ec800m,         
                "vTaskEC800M",         
                2048,                  
                NULL,                
                tskIDLE_PRIORITY + 3, 
                &xHandleTaskEc800m);  
}

static void Task_Obj_Create(void)
{
    /* 创建互斥信号量 */
    xMutex = xSemaphoreCreateMutex();

    if (xMutex == NULL)
    {
        /* 没有创建成功 */
        printf("xMutex failed \n");
    }

    /* 创建二值信号量，首次创建信号量计数值是0 */
    xSemaphore = xSemaphoreCreateBinary();

    if (xSemaphore == NULL)
    {
        /* 没有创建成功 */
        printf("xSemaphore failed \n");
    }
}
