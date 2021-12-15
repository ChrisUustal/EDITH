/* ~~~~~~~~~~ Includes ~~~~~~~~~~ */
#include <SPI.h>
//#include "CFAL12856A0_0151_B_Splash.h"

/* ~~~~~~~~~~ Defines ~~~~~~~~~~ */
#define SER_DEBUG

#define DISPLAY_RESET 14
#define DISPLAY_SPI_CS 32
#define DISPLAY_SPI_DC 15

#define MAX_BRIGHT (0x8F)

#define XLevelL    0x00
#define XLevelH    0x10
#define XLevel     ((XLevelH&0x0F)*16+XLevelL)
#define HRES       128
#define VRES       56
#define Brightness 0xBF

/* ~~~~~~~~~~ Classes + Enums ~~~~~~~~~~ */
enum Brightness_t {bright, dim};
Brightness_t display_brightness;

/* ~~~~~~~~~~ Prototypes ~~~~~~~~~~ */

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
void display_custom();

//================================================================================
void setup() {

  pinMode(DISPLAY_RESET, OUTPUT);
  pinMode(DISPLAY_SPI_DC, OUTPUT);
  pinMode(DISPLAY_SPI_CS, OUTPUT);

#ifdef SER_DEBUG
  Serial.begin(9600);
  Serial.println();
  Serial.println("Serial Initialized");
#endif // SER_DEBUG
  
  //SPI begin transactions takes ~2.5us
  SPI.begin();
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
 
  //OLED_Init takes ~120ms
  OLED_Init();
  
#ifdef SER_DEBUG
  Serial.println("Init done");
#endif // SER_DEBUG

  set_brightness(dim);
  delay(100);

}

void loop() {
  #ifdef SER_DEBUG
    Serial.println("top of loop");
  #endif // SER_DEBUG

  // Put up our splash screen
//  showImage(Splash);
//  delay(1000);

//  Fill_RAM(0x01);
//  delay(1000);
//
//  Fill_RAM(0x02);
//  delay(1000);

  display_custom();
  delay(1000);
  
//  //Demonstrate dimming
//  writeCommand(0x81); // Set SEG Output Current
//  writeCommand(0); // Set Contrast Control for Bank 0
//  delay(1000);
//
//  Fill_RAM(0xff);
//  delay(1000);
//
//  //Back to full brightness
//  writeCommand(0x81); // Set SEG Output Current
//  writeCommand(MAX_BRIGHT); // Set Contrast Control for Bank 0
//  delay(500);
//
//  //Put up some demo screens
//  Fill_RAM_CheckerBoard();
//  delay(1000);
//  showImage(Eagle);
//  delay(2000);
//  Fill_RAM(0x00);
//  delay(500);
//
//
//  //Put up the "aiming" mock-up
//  showImage(Aiming);
//  delay(3000);
//
//  //Finished
//  Fill_RAM_CheckerBoard();
//  delay(1000);

  //showImage(SuperTank);
  //delay(1000);

//  Fill_RAM(0x00);
//  delay(1500);
//  Fill_RAM(0xff);
//  delay(1500);
//  Fill_RAM(0x0f);
//  delay(1500);
//  showImage(Splash);
//  delay(3000);
}

void writeCommand(uint8_t command) {
  // Select the LCD's command register
  //CLR_DC;
  digitalWrite(DISPLAY_SPI_DC, LOW);
  // Select the LCD controller
  //CLR_CS;
  digitalWrite(DISPLAY_SPI_CS, LOW);

  //Send the command via SPI:
  SPI.transfer(command);
  //deselect the controller
  //SET_CS;
  digitalWrite(DISPLAY_SPI_CS, HIGH);
}

void writeData(uint8_t data) {
  //Select the LCD's data register
  //SET_DC;
  digitalWrite(DISPLAY_SPI_DC, HIGH);
  //Select the LCD controller
  //CLR_CS;
  digitalWrite(DISPLAY_SPI_CS, LOW);
  //Send the command via SPI:
  SPI.transfer(data);

  // Deselect the LCD controller
  //SET_CS;
  digitalWrite(DISPLAY_SPI_CS, HIGH);
}

void Set_Start_Column(uint8_t d)
  {
  writeCommand(0x00 + d % 16);    // Set Lower Column Start Address for Page Addressing Mode
            //   Default => 0x00
  writeCommand(0x10 + d / 16);    // Set Higher Column Start Address for Page Addressing Mode
            //   Default => 0x10
  }

void Set_Column_Address(uint8_t a, uint8_t b)
  {
  writeCommand(0x21);     // Set Column Address
  writeCommand(a);      //   Default => 0x00 (Column Start Address)
  writeCommand(b);      //   Default => 0x7F (Column End Address)
  }

void Set_Page_Address(uint8_t a, uint8_t b)
  {
  writeCommand(0x22);     // Set Page Address
  writeCommand(a);      //   Default => 0x00 (Page Start Address)
  writeCommand(b);      //   Default => 0x07 (Page End Address)
  }

void Set_Start_Page(uint8_t d)
  {
  writeCommand(0xB0 | d);     // Set Page Start Address for Page Addressing Mode
            //   Default => 0xB0 (0x00)
  }

void Fill_RAM(uint8_t Data)
  {
  uint8_t
    i;
  uint8_t
    j;

  for (i = 0; i < 8; i++)
    {
    Set_Start_Page(i);
    Set_Start_Column(0x00);

    for (j = 0; j < 128; j++)
      {
      writeData(Data);
      }
    }
  }

void Fill_RAM_CheckerBoard(void)
  {
  uint8_t
    page;
  uint8_t
    column;

  for(page = 0; page < 8; page++)
    {
    Set_Start_Page(page);
    Set_Start_Column(0x00);

    for (column= 0; column < 128; column++)
      {
      if(0 == (column&0x01))
        {
        writeData(0x55);
        }
      else
        {
        writeData(0xAA);
        }
      }
    }
  }

void OLED_Init()
  {
  //The CFA10105 has a power-on reset circuit, 
  //you can use the following code if you are using GPIO for reset
  //CLR_RESET;
  digitalWrite(DISPLAY_RESET, LOW);
  delay(1);
  //SET_RESET;
  digitalWrite(DISPLAY_RESET, HIGH);
  delay(120);

  writeCommand(0xFD);  // Set Command Lock
  writeCommand(0X12); //   Default => 0x12
                      //     0x12 => Driver IC interface is unlocked from entering command.
                      //     0x16 => All Commands are locked except 0xFD.

  writeCommand(0XAE); // Set Display On/Off
                      //   Default => 0xAE
                      //     0xAE => Display Off
                      //     0xAF => Display On

                            
  writeCommand(0xD5); // Set Display Clock Divide Ratio / Oscillator Frequency
//  writeCommand(0XA0); // Set Clock as 116 Frames/Sec
  writeCommand(0X30);  // Set Clock as 116 Frames/Sec
                      //   Default => 0x70
                      //     D[3:0] => Display Clock Divider
                      //     D[7:4] => Oscillator Frequency

  writeCommand(0xA8); // Set Multiplex Ratio
  writeCommand(0X37); //   Default => 0x3F (1/56 Duty)

  writeCommand(0xD3); // Set Display Offset
  writeCommand(0X08); //   Default => 0x00

  writeCommand(0x40); // Set Mapping RAM Display Start Line (0x00~0x3F)
                      //   Default => 0x40 (0x00)

  //writeCommand(0xD8); // Set Low Power Display Mode (0x04/0x05)
  //writeCommand(0x05); //   Default => 0x04 (Normal Power Mode)

  writeCommand(0xA1); // Set SEG/Column Mapping (0xA0/0xA1)
                      //   Default => 0xA0
                      //     0xA0 => Column Address 0 Mapped to SEG0
                      //     0xA1 => Column Address 0 Mapped to SEG127

  writeCommand(0xC8); // Set COM/Row Scan Direction (0xC0/0xC8)
                      //   Default => 0xC0
                      //     0xC0 => Scan from COM0 to 63
                      //     0xC8 => Scan from COM63 to 0

  writeCommand(0xDA); // Set COM Pins Hardware Configuration
  writeCommand(0x12); //   Default => 0x12
                      //     Alternative COM Pin Configuration
                      //     Disable COM Left/Right Re-Map

  writeCommand(0x81); // Set SEG Output Current
  writeCommand(0x8F); // Set Contrast Control for Bank 0

  writeCommand(0xD9); // Set Pre-Charge as 2 Clocks & Discharge as 5 Clocks
  writeCommand(0x25); //   Default => 0x22 (2 Display Clocks [Phase 2] / 2 Display Clocks [Phase 1])
                      //     D[3:0] => Phase 1 Period in 1~15 Display Clocks
                      //     D[7:4] => Phase 2 Period in 1~15 Display Clocks
  
  writeCommand(0xDB); // Set VCOMH Deselect Level
  writeCommand(0x34); //   Default => 0x34 (0.78*VCC)

  writeCommand(0xA4); // Set Entire Display On / Off
                      //   Default => 0xA4
                      //     0xA4 => Normal Display
                      //     0xA5 => Entire Display On

  writeCommand(0xA6); // Set Inverse Display On/Off
                      //   Default => 0xA6
                      //     0xA6 => Normal Display
                      //     0xA7 => Inverse Display On

  Fill_RAM(0x00);     // Clear Screen

  writeCommand(0XAF); // Display On (0xAE/0xAF)
  }

void showImage(const uint8_t image[7][128])
  {
  //The logo fits in the first 7 pages (7x8=56)
  for (uint8_t y = 0; y < 7; y++)
    {
    // Set the starting page and column
    Set_Start_Page(y);
    Set_Start_Column(0x00);
    for (uint8_t x = 0; x < 128; x++)
      {
      writeData(pgm_read_byte(&image[y][x]));
      }
    }
  //Clear the last page so stray pixels do not show in getter area.
  Set_Start_Page(7);
  Set_Start_Column(0x00);
  for (uint8_t x = 0; x < 128; x++)
    {
    writeData(0x00);
    }
  }

void set_brightness(Brightness_t input) {

  display_brightness = input;
  writeCommand(0x81); // Set SEG Output Current

  switch(input) {
    case bright :
      writeCommand(MAX_BRIGHT); // Set Contrast Control for Bank 0
      break;
    case dim : 
      writeCommand(0); // Set Contrast Control for Bank 0
      break;
    default :
      writeCommand(0); // Set Contrast Control for Bank 0
  }
  
}

void display_custom(){
  uint8_t
    page;
  uint8_t
    column;

  for(page = 0; page < 8; page++)
    {
    Set_Start_Page(page);
    Set_Start_Column(0x00);

    for (column= 0; column < 128; column++)
      {
      if(0 == (column&0x01))
        {
        writeData(0x55);
        }
      else
        {
        writeData(0xAA);
        }
      }
    }
}
