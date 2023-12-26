#ifndef _APP_EC800M_H_
#define _APP_EC800M_H_

#include <stdint.h>

typedef struct
{
	uint8_t ec800m_flag;
	uint16_t ec800m_len;
	uint8_t ec800m_time_buff[53];
	bool ec800m_dat_state;
} EC800M_BUFF_SB;

extern EC800M_BUFF_SB ec800m_buff_sb;
extern char sd_card_time[21];



int App_Ec800m_GPIO_Init(void);
int App_Ec800m_Init(void);
void App_Ec800m_Connect(void);
void App_Mc800m_Reporting_Attribute(char* identifier, char* value);
void App_Mc800m_Reporting_Attribute_1(char* identifier, uint32_t value);
void App_Mc800m_Disconnect_Mqtt(void);
void App_Mc800m_Get_Time(void);
int App_Mc800m_Time_Buff_Rx(void);

#endif

