#ifndef HAL_TFT_TFT_INT_H_
#define HAL_TFT_TFT_INT_H_

void HTFT_vInit(void);
void HTFT_vShowImage(const u16 A_u16ImgArr[], u16 A_u16ImgSize);

void HTFT_vSetXPos(u16 A_u16xStart, u16 A_u16xEnd);
void HTFT_vSetYPos(u16 A_u16YStart, u16 A_u16YEnd);

void HTFT_vFillBackgroundColor(u16 A_u16Color);
void HTFT_vFillRectangle(u16 A_u16Color);

/* New Text Functions */
void HTFT_vDrawPixel(u16 x, u16 y, u16 color);
void HTFT_vPrintChar(u16 x, u16 y, char ch, u16 color, u16 bg_color);
void HTFT_vPrintString(u16 x, u16 y, const char *str, u16 color, u16 bg_color);

#endif /* HAL_TFT_TFT_INT_H_ */
