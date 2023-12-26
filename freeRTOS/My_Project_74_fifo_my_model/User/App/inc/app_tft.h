#ifndef _APP_TFT_H_
#define _APP_TFT_H_

#define THICKNESS 4

void TFT_Print(char *str, int x, int y, int font, int length);
void draw_obj_rect(float *xy, int class_idx, uint32_t w, uint32_t h, uint8_t scale);
int App_Tft_Init(void);
void App_Tft_Print(char *str, int x, int y, int font, int length);

#endif

