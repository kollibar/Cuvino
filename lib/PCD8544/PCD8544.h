/*********************************************************************
This is a library for our Monochrome Nokia 5110 LCD Displays

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/338

These displays use SPI to communicate, 4 or 5 pins are required to
interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/
#ifndef __PCD8544_H__
#define __PCD8544_H__

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include "pins_arduino.h"
#endif

#include <SPI.h>
#include <Adafruit_GFX_v1.0.2_NB8bits.h>

#ifdef __SAM3X8E__
  typedef volatile RwReg PortReg;
  typedef uint32_t PortMask;
#else
  typedef volatile uint8_t PortReg;
  typedef uint8_t PortMask;
#endif

#ifdef BLACK
#undef BLACK
#endif
#ifdef WHITE
#undef WHITE
#endif
#define BLACK 1
#define WHITE 0

#ifdef LCDWIDTH
#undef LCDWIDTH
#endif
#ifdef LCDHEIGHT
#undef LCDHEIGHT
#endif
#define LCDWIDTH 84
#define LCDHEIGHT 48

#define PCD8544_POWERDOWN 0x04
#define PCD8544_ENTRYMODE 0x02
#define PCD8544_EXTENDEDINSTRUCTION 0x01

#define PCD8544_DISPLAYBLANK 0x0
#define PCD8544_DISPLAYNORMAL 0x4
#define PCD8544_DISPLAYALLON 0x1
#define PCD8544_DISPLAYINVERTED 0x5

// H = 0
#define PCD8544_FUNCTIONSET 0x20
#define PCD8544_DISPLAYCONTROL 0x08
#define PCD8544_SETYADDR 0x40
#define PCD8544_SETXADDR 0x80

// H = 1
#define PCD8544_SETTEMP 0x04
#define PCD8544_SETBIAS 0x10
#define PCD8544_SETVOP 0x80

// Default to max SPI clock speed for PCD8544 of 4 mhz (16mhz / 4) for normal Arduinos.
// This can be modified to change the clock speed if necessary (like for supporting other hardware).
#define PCD8544_SPI_CLOCK_DIV SPI_CLOCK_DIV4

typedef void (*pcd8544PinFct)(uint8_t, uint8_t);
typedef void (*pcd8544RstFct)();


class Adafruit_PCD8544 : public Adafruit_GFX_NB8bits {
 public:
  // Software SPI with explicit CS pin.
  Adafruit_PCD8544(int8_t SCLK, int8_t DIN, int8_t DC, int8_t CS, int8_t RST);
  // Software SPI with CS tied to ground.  Saves a pin but other pins can't be shared with other hardware.
  Adafruit_PCD8544(int8_t SCLK, int8_t DIN, int8_t DC, int8_t RST);
  // Hardware SPI based on hardware controlled SCK (SCLK) and MOSI (DIN) pins. CS is still controlled by any IO pin.
  // NOTE: MISO and SS will be set as an input and output respectively, so be careful sharing those pins!
  Adafruit_PCD8544(int8_t DC, int8_t CS, int8_t RST);

  Adafruit_PCD8544(int8_t SCLK, int8_t DIN,pcd8544PinFct fct, pcd8544RstFct fctRST);
  Adafruit_PCD8544(pcd8544PinFct fctPin, pcd8544RstFct fctRst);

  // peut être réécrite pour ajouter des fonctions au démarrage, mais dans ce cas, DOIT être appellé quand même  (via Adafruit_PCD8544::begin)
  virtual void begin(uint8_t contrast = 40, uint8_t bias = 0x04);

  virtual void command(uint8_t c);
  virtual void data(uint8_t c);

  void setContrast(uint8_t val);
  void clearDisplay(void);
  virtual void display();

  void drawPixel(int8_t x, int8_t y, uint8_t color);
  void invertPixel(int8_t x, int8_t y);
  void invertRect(int8_t x, int8_t y,int8_t w,int8_t h);
  uint8_t getPixel(int8_t x, int8_t y);

protected:
  int8_t _din, _sclk, _dc, _rst, _cs;
  pcd8544PinFct fctPIN;
  pcd8544RstFct fctRST;
  volatile PortReg  *mosiport, *clkport;
  PortMask mosipinmask, clkpinmask;

  virtual void spiWrite(uint8_t c);
  virtual bool isHardwareSPI() const;
};

class PCD8544_SPI_Software : public Adafruit_PCD8544{
public:
  PCD8544_SPI_Software(int8_t SCLK, int8_t DIN, int8_t DC, int8_t CS, int8_t RST);
  PCD8544_SPI_Software(int8_t SCLK, int8_t DIN, int8_t DC, int8_t RST);

  // peut être réécrite pour ajouter des fonctions au démarrage, mais dans ce cas, DOIT être appellé quand même  (via PCD8544_SPI_SOFTWARE::begin)
  virtual void begin(uint8_t contrast = 40, uint8_t bias = 0x04);

  void command(uint8_t c);
  void data(uint8_t c);
protected:
  void spiWrite(uint8_t c);
  bool isHardwareSPI() const;
};

#endif
