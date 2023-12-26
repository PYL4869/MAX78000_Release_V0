#include "bsp.h"

/*
   摄像模式：
   #define CAMERA_FREQ (10 * 1000 * 1000) 感觉更加流畅
   同时需要修改DMA模式
   camera_setup(IMAGE_SIZE_X, IMAGE_SIZE_Y, PIXFORMAT_RGB565, FIFO_FOUR_BYTE, USE_DMA,
                 dma_channel);
   识别模式下：
   #define CAMERA_FREQ (5 * 1000 * 1000)  感觉更加流畅
   同时需要修改DMA模式
   camera_setup(IMAGE_SIZE_X, IMAGE_SIZE_Y, PIXFORMAT_RGB888, FIFO_THREE_BYTE, STREAMING_DMA,
                     dma_channel);

*/
#define CAMERA_FREQ (10 * 1000 * 1000)
#define IMAGE_SIZE_X (74)
#define IMAGE_SIZE_Y (74)
#define X_START 0
#define Y_START 0


#define TFT_X_START 100
#define TFT_Y_START 50

/**
 * @description: 相机初始化
 * @return {*}
 */
int App_Camera_Init(void)
{

    int dma_channel;
    MXC_DMA_Init();
    dma_channel = MXC_DMA_AcquireChannel();

    camera_init(CAMERA_FREQ);
    /* 设置相机参数属性 */
    /* CNN */
    camera_setup(IMAGE_SIZE_X, IMAGE_SIZE_Y, PIXFORMAT_RGB888, FIFO_THREE_BYTE, USE_DMA,
                        dma_channel); 
    /* 摄像 */
/*     camera_setup(IMAGE_SIZE_X, IMAGE_SIZE_Y, PIXFORMAT_RGB565, FIFO_FOUR_BYTE, USE_DMA,
                 dma_channel); */

    /* 设置相机时钟预分频器以防止流溢出 */
/*     camera_write_reg(0x11, 0x3); */

    return MAX_OK;
}

/**
 * @description: 相机获取显示到TFT上
 * @return {*}
 */
void App_Camera_Get(void)
{
/*     TickType_t xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount(); */
    uint8_t *data = NULL;
    camera_start_capture_image();
    while (1)
    {
        while ((data = get_camera_stream_buffer()) == NULL)
        { 
        if (camera_is_image_rcv())
        {
            uint8_t *raw;
            uint32_t imgLen;
            uint32_t w, h;
            camera_get_image(&raw, &imgLen, &w, &h);
            MXC_TFT_ShowImageCameraRGB565(Y_START, X_START, raw, w, h);
            release_camera_stream_buffer();
            camera_start_capture_image();
            /*             Bsp_Led_Toggle(RED_LED_OPEN); */
        }
        }
/*         App_Mc800m_Get_Time(); */
/*         vTaskDelayUntil(&xLastWakeTime, 50); */
    }
}

/**
 * @description: 获取相机图像，将分别相机图像格式输入给CNN和TFT
 * @return {*}
 */
void App_Camera_Cnn_Get(void)
{

        uint8_t *frame_buffer;
        uint8_t *buffer;
        uint32_t imgLen;
        uint32_t w, h, x, y;
        uint8_t r, g, b;

        uint32_t *cnn_mem;
        uint32_t *fifo_addr =  (volatile uint32_t *) 0x50000008;

        uint32_t color;
        int cnt = 0;

        camera_start_capture_image();

        while (!camera_is_image_rcv())
        {
        }

        camera_get_image(&frame_buffer, &imgLen, &w, &h);
        buffer = frame_buffer;

        for (int y = 0; y < IMAGE_SIZE_Y; y++)
        {
            for (x = 0; x < IMAGE_SIZE_X; x++)
            {
                r = *buffer++;
                g = *buffer++;
                b = *buffer++;
                buffer++; // skip msb=0x00
                // change the range from [0,255] to [-128,127] and store in buffer for CNN
                while (((*((volatile uint32_t *) 0x50000004) & 1)) != 0); // Wait for FIFO 0
                *fifo_addr = ((b << 16) | (g << 8) | r) ^ 0x00808080;                

                color = ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);

                MXC_TFT_WritePixel(x * IMG_SCALE, y * IMG_SCALE, IMG_SCALE, IMG_SCALE, color);

                // Remove the following line if there is no risk that the source would overrun the FIFO:

            }
        }

/*     uint8_t *frame_buffer;
    uint8_t *buffer;
    uint32_t imgLen;
    uint32_t w, h, x, y;
    uint8_t r, g, b;
    uint32_t *cnn_mem = (uint32_t *)0x50402000;
    uint32_t color;

    camera_start_capture_image();

    while (!camera_is_image_rcv())
    {
    }

    camera_get_image(&frame_buffer, &imgLen, &w, &h);
    buffer = frame_buffer;

    for (y = 0; y < 74; y++)
    {
        for (x = 0; x < 74; x++)
        {
            r = *buffer++;
            g = *buffer++;
            b = *buffer++;
            buffer++; // skip msb=0x00
            // change the range from [0,255] to [-128,127] and store in buffer for CNN
            *cnn_mem++ = ((b << 16) | (g << 8) | r) ^ 0x00808080;
            
            color = ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);

            MXC_TFT_WritePixel(x * IMG_SCALE, y * IMG_SCALE, IMG_SCALE, IMG_SCALE, color);

        }
    } */
}
