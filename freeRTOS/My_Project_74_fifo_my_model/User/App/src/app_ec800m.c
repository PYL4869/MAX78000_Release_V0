#include "bsp.h"

/* 打开 MQTT 客户端网络 */
char BUFF1[] = "AT+QMTOPEN=0,\"QCWGDPVYC6.iotcloud.tencentdevices.com\",1883\r\n";
/* MQTT三元组根据情况修改 */
char BUFF2[] = "AT+QMTCONN=0,\"QCWGDPVYC6Water_meter_1\",\"QCWGDPVYC6Water_meter_1;12010126;8697d;1711296000\",\"1a7285034f9e1a3248a0e7b7a9fe7f3b25a9d93a8a188fb6f6cf89e733ced986;hmacsha256\"\r\n";
/* 客户端断开与 MQTT 服务器的连接 */
char BUFF3[] = "AT+QMTDISC=0\r\n";
/* 发布消息 */
char BUFF4[70] = "AT+QMTPUBEX=0,0,0,0,\"$thing/up/property/QCWGDPVYC6/Water_meter_1\",66\r\n";
/* 发布消息内容 */
char BUFF5[] = "{\"method\":\"report\",\"clientToken\":\"123\",\"params\":{\"AAA_6\":";
/* 发布消息内容使用 */
char BUFF6[66] = {0};
char num1[] = "}}\r\n";
/* 时间暂存buff */
char sd_card_time[21] = {0};
/* 用水量暂存buff */
char sd_card_water[9] = {0};
/* 
    sd卡buff 
    格式为 "2023/10/26/10:48:17","99999"\r\n
    char sd_card_test_time[31] = "\"2023/10/26/10:48:17\",\"99999\"\r\n";
*/
char sd_card_buff[31] = {0};
char sd_card_buff_user[1860] = {0};


EC800M_BUFF_SB ec800m_buff_sb;


/**
 * @description: 使能EC800M供电信号
 * @return {*}
 */
int App_Ec800m_GPIO_Init(void)
{
    mxc_gpio_cfg_t gpio_out = {0};

    MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_GPIO1);

    gpio_out.port = MXC_GPIO1;
    gpio_out.mask = MXC_GPIO_PIN_6;
    gpio_out.func = MXC_GPIO_FUNC_OUT;    
    gpio_out.pad = MXC_GPIO_PAD_NONE;
    MXC_GPIO_Config(&gpio_out);

    MXC_GPIO_OutSet(gpio_out.port, gpio_out.mask);
    MXC_Delay(500000);
    MXC_GPIO_OutClr(gpio_out.port, gpio_out.mask);
    MXC_Delay(1000000);
    MXC_GPIO_OutSet(gpio_out.port, gpio_out.mask);

    return MAX_OK;
}

int App_Ec800m_GPIO_Init_1(void)
{
    mxc_gpio_cfg_t gpio_out = {0};

    MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_GPIO0);

    gpio_out.port = MXC_GPIO0;
    gpio_out.mask = MXC_GPIO_PIN_16;
    gpio_out.pad = MXC_GPIO_PAD_NONE;
    gpio_out.func = MXC_GPIO_FUNC_OUT;
    MXC_GPIO_Config(&gpio_out);

    MXC_GPIO_OutSet(gpio_out.port, gpio_out.mask);
    MXC_Delay(100000);
    MXC_GPIO_OutClr(gpio_out.port, gpio_out.mask);   

    return MAX_OK;
}

/**
 * @description: 初始化EC800M硬件
 * @return {*}
 */
int App_Ec800m_Init(void)
{
    App_Ec800m_GPIO_Init();
/*     App_Ec800m_GPIO_Init_1();  */
    return MAX_OK;
}

/**
 * @description: 初始化EC800M连接腾讯云
 * @return {*}
 */
void App_Ec800m_Connect(void)
{
    Bsp_Uart_Transmit((uint8_t *)BUFF1, sizeof(BUFF1));
    MXC_Delay(250000);
    Bsp_Uart_Transmit((uint8_t *)BUFF2, sizeof(BUFF2));
    MXC_Delay(100000);
}

/**
 * @description: 客户端断开与 MQTT 服务器的连接
 * @return {*}
 */
void App_Mc800m_Disconnect_Mqtt(void)
{
    Bsp_Uart_Transmit((uint8_t *)BUFF3, sizeof(BUFF3));
}

/**
 * @description: 从EC800获取时间
 * @return {*}
 */
void App_Mc800m_Get_Time(void)
{
    char get_time[] = "AT+QLTS=2\r\n";
    Bsp_Uart_Transmit((uint8_t *)get_time, sizeof(get_time));
    ec800m_buff_sb.ec800m_flag = MAX_OK;
}

int aaab = 0;
int App_Mc800m_Time_Buff_Rx(void)
{
/*     if (ec800m_buff_sb.ec800m_len == sizeof(ec800m_buff_sb.ec800m_time_buff))
    { */
        ec800m_buff_sb.ec800m_len = 0;
        if ((ec800m_buff_sb.ec800m_time_buff[0] == 'A') &&
            (ec800m_buff_sb.ec800m_time_buff[1] == 'T') &&
            (ec800m_buff_sb.ec800m_time_buff[2] == '+') &&
            (ec800m_buff_sb.ec800m_time_buff[3] == 'Q') &&
            (ec800m_buff_sb.ec800m_time_buff[4] == 'L') &&
            (ec800m_buff_sb.ec800m_time_buff[5] == 'T') &&
            (ec800m_buff_sb.ec800m_time_buff[6] == 'S') &&
            (ec800m_buff_sb.ec800m_time_buff[7] == '=') &&
            (ec800m_buff_sb.ec800m_time_buff[8] == '2') &&
            (ec800m_buff_sb.ec800m_time_buff[51] == '\r') &&
            (ec800m_buff_sb.ec800m_time_buff[52] == '\n'))
        {
            for (int i = 20; i <= 38; i++)
            {
                sd_card_time[i - 20] = (char)ec800m_buff_sb.ec800m_time_buff[i];
                ec800m_buff_sb.ec800m_dat_state = 1;
            }
            memset(ec800m_buff_sb.ec800m_time_buff, 0, sizeof(ec800m_buff_sb.ec800m_time_buff));
            sd_card_time[10] = '/';
            sd_card_time[19] = '"';
            sd_card_time[20] = ',';

            sd_card_water[0] = '"';
            sprintf(sd_card_water+1, "%u", cnn_result);
            sd_card_water[6] = '"'; 
            sd_card_water[7] = '\r';
            sd_card_water[8] = '\n';
           
            sd_card_buff[0] = '"';
            strcat(sd_card_buff+1, sd_card_time);
            strcat(sd_card_buff, sd_card_water);                   

            /* 这样判断buff是否满会缺失一次 每一次60次写入一次SD卡*/
            if( sd_card_buff_user[1859] != '\n')
            {
                strcat(sd_card_buff_user, sd_card_buff);        
            }
            else if( sd_card_buff_user[1859] == '\n')
            {
                /* 此处建议通知另一个任务对于SD卡进行写入操作 */
                App_Sd_Write(sd_card_buff_user);
                memset(sd_card_buff_user, 0, sizeof(sd_card_buff_user));
            }

            memset(sd_card_time, 0, sizeof(sd_card_time));
            memset(sd_card_water, 0, sizeof(sd_card_water));            
            memset(sd_card_buff, 0, sizeof(sd_card_buff));

        }
        else
        {
            ec800m_buff_sb.ec800m_dat_state = 0;
        }
   /*  } */
    return ec800m_buff_sb.ec800m_dat_state;
}

/**
 * @description: 向腾讯云发布消息
 * @param {char*} identifier 标识符
 * @param {char*} value 发送内容单数字，需要完善
 * @return {*}
 */
void App_Mc800m_Reporting_Attribute(char *identifier, char *value)
{

    Bsp_Uart_Transmit((uint8_t *)BUFF4, sizeof(BUFF4));
    BUFF5[54] = *identifier;
    BUFF5[57] = *value;
    Bsp_Uart_Transmit((uint8_t *)BUFF5, sizeof(BUFF5));
}

/**
 * @description: 向腾讯云发布消息
 * @param {char*} identifier 标识符
 * @param {uint32_t} value  发送内容，将传入的uint32_t值转为字符型发布消息
 * @return {*}
 */
void App_Mc800m_Reporting_Attribute_1(char *identifier, uint32_t value)
{
    /* 99999}}\r\n共9 */
    char num[9];
    BUFF5[54] = *identifier;
    /* 测试使用 */
/*     Bsp_Led_Toggle(BLUE_LED_OPEN); */
    /* 发布消息 */
    Bsp_Uart_Transmit((uint8_t *)BUFF4, sizeof(BUFF4));
    /* 此处延时时间需要修改小一点 1000000只是为了LED闪烁明显 */
    MXC_Delay(3000);
    /* 发布消息内容 */
    sprintf(num, "%u", value);
    strcat(num, num1);
    memcpy(BUFF6, &BUFF5, sizeof(BUFF5));
    strcat(BUFF6, num);

    Bsp_Uart_Transmit((uint8_t *)BUFF6, sizeof(BUFF6));
    strcpy(BUFF6, "");
}
