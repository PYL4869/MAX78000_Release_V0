#include "bsp.h"

/*
    显示图片要求：
    24位BMP/XX,详细可以查看bmp2c.py文件说明  可以通过电脑自带的画图功能转化为24位BMP图
    320x240    可以通过python bmp2c.py xxx.bmp -s生成
    必须在它的目录下，也就是需要有几个特殊文件生成

    python bmp2c.py xxx.bmp -f 生成c文件
    将生成的c文件中的数组名字替换成tft_ili9341.h里面有的名字
    int image_bitmap_1 = (int)&img_1_rgb565[0];
    MXC_TFT_ShowImage(0, 0, image_bitmap_1);
    就可以正常显示了
 */

/**
 * @description: 初始化TFT屏
 * @return {*}
 */
int App_Tft_Init(void)
{
    mxc_gpio_cfg_t tft_reset_pin = {MXC_GPIO0, MXC_GPIO_PIN_19, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH};
    mxc_gpio_cfg_t tft_led_pin = {MXC_GPIO0, MXC_GPIO_PIN_9, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE, MXC_GPIO_VSSEL_VDDIOH};
    MXC_TFT_Init(MXC_SPI0, 1, &tft_reset_pin, &tft_led_pin);
    /* 
        设置屏幕旋转 
        摄像模式注释掉    
    */
    MXC_TFT_SetRotation(ROTATE_270);

    MXC_TFT_SetForeGroundColor(WHITE);
    /* 将背景设置为红色 */
    /*     MXC_TFT_SetBackGroundColor(4); */

    return MAX_OK;
}

/**
 * @description: 显示屏输出文字
 * @param {char} *str  字符串
 * @param {int} x      x坐标
 * @param {int} y      y坐标
 * @param {int} font   字体文件
 * @param {int} length 字符串长度
 * @return {*}
 */
void App_Tft_Print(char *str, int x, int y, int font, int length)
{
    text_t text;
    text.data = str;
    text.len = length;
    MXC_TFT_PrintFont(x, y, font, &text, NULL);
}


static int font = (int)&SansSerif16x16[0];

static text_t label_text[] = {
    // info
    { (char *)"1", 1 },  { (char *)"2", 1 },  { (char *)"3", 1 }, { (char *)"4", 1 },
    { (char *)"5", 1 }, { (char *)"6", 1 },  { (char *)"7", 1 }, { (char *)"8", 1 },
    { (char *)"9", 1 }, { (char *)"0", 1 },
};


void TFT_Print(char *str, int x, int y, int font, int length)
{
    // fonts id
    text_t text;
    text.data = str;
    text.len = length;

/*     MXC_TFT_PrintFont(x, y, font, &text, NULL); */
}

/**
 * @description:           在TFT上绘制预测框和预测数字
 * @param {float} *xy
 * @param {int} class_idx
 * @param {uint32_t} w
 * @param {uint32_t} h
 * @param {uint8_t} scale
 * @return {*}
 */
void draw_obj_rect(float *xy, int class_idx, uint32_t w, uint32_t h, uint8_t scale)
{
    int r = 0, g = 0, b = 0;
    uint32_t color;

    r = 253;
    g = 172;
    b = 83;

    int x1 = w * xy[0];
    int y1 = h * xy[1];
    int x2 = w * xy[2];
    int y2 = h * xy[3];
    int x, y;

    /* 绘制预测框 */
    color = ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);

    for (x = x1; x < x2; ++x) {
        MXC_TFT_WritePixel(x * scale, y1 * scale, scale, scale, color);
        MXC_TFT_WritePixel(x * scale, y2 * scale, scale, scale, color);
    }

    for (y = y1; y < y2; ++y) {
        MXC_TFT_WritePixel(x1 * scale, y * scale, scale, scale, color);
        MXC_TFT_WritePixel(x2 * scale, y * scale, scale, scale, color);
    }
    /* 绘制预测数字 */
    MXC_TFT_PrintFont(x1 * scale + THICKNESS, y1 * scale + THICKNESS, font, &label_text[class_idx],
                      NULL);
}
