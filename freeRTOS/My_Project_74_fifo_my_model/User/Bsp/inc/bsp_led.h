#ifndef __BSP_LED_H
#define __BSP_LED_H

#include <stdint.h>

#define RED_LED_OPEN 0
#define RED_LED_OFF 0
#define GREEN_LED_OPEN 1
#define GREEN_LED_OFF 1
#define BLUE_LED_OPEN 2
#define BLUE_LED_OFF 2

int Bsp_Led_Init(void);
void Bsp_Led_Open(uint8_t i);
void Bsp_Led_Off(uint8_t i);
void Bsp_Led_Toggle(uint8_t i);

#endif
