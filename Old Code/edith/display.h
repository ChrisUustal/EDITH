//display.h
#ifndef DISPLAY_H
#define DISPLAY_H

#include <SPI.h>

#define DISPLAY_RESET   14
#define DISPLAY_SPI_CS  32
#define DISPLAY_SPI_DC  15

#define MAX_BRIGHT (0x8F)

#define XLevelL    0x00
#define XLevelH    0x10
#define XLevel     ((XLevelH&0x0F)*16+XLevelL)
#define HRES       128
#define VRES       56
#define Brightness 0xBF

#define LOOP_DELAY 40

/* ~~~~~~~~~~ Classes + Enums ~~~~~~~~~~ */
enum Brightness_t {bright, dim};
Brightness_t display_brightness;

/* ~~~~~~~~~~ Prototypes ~~~~~~~~~~ */

void display_setup();
void display_loop();
void draw_square(int x, int y, int r);
void clear_frame();

void writeCommand(uint8_t command);
void writeData(uint8_t data);
void Set_Start_Column(uint8_t d);
void Set_Column_Address(uint8_t a, uint8_t b);
void Set_Page_Address(uint8_t a, uint8_t b);
void Set_Start_Page(uint8_t d);
void Fill_RAM(uint8_t Data);
void Fill_RAM_CheckerBoard(void);
void OLED_Init();
void showImage(const uint8_t image[7][128]);
void set_brightness(Brightness_t input);
void display_frame();

/* ~~~~~~~~~~ Variables ~~~~~~~~~~ */
uint8_t frame[7][128];
int square_x, square_y, square_r;

#endif
