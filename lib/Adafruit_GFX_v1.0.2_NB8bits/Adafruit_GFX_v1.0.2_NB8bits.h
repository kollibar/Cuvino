#ifndef _ADAFRUIT_GFX_v1_0_2_NB_H
#define _ADAFRUIT_GFX_v1_0_2_NB_H

#include "glcdfont_v1.0.2_NB8bits.c"
#include "glcdfontReduit_v1.0.2_NB8bits.c"

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif

#include <PrintPGM.h>

#define swap(a, b) { int8_t t = a; a = b; b = t; }

class Adafruit_GFX_NB8bits : public PrintPGM {

 public:

  Adafruit_GFX_NB8bits(int8_t w, int8_t h); // Constructor

  // ajout!
    //get textSize
    uint8_t getTextSize(void) const;

    // get color
    uint8_t getTextColor(void) const;
    uint8_t getTextColorBg(void) const;

  // This MUST be defined by the subclass:
  virtual void drawPixel(int8_t x, int8_t y, uint8_t color) = 0;
  virtual uint8_t getPixel(int8_t x, int8_t y)=0;

  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.
  virtual void
    drawLine(int8_t x0, int8_t y0, int8_t x1, int8_t y1, uint8_t color),
    drawFastVLine(int8_t x, int8_t y, int8_t h, uint8_t color),
    drawFastHLine(int8_t x, int8_t y, int8_t w, uint8_t color),
    drawRect(int8_t x, int8_t y, int8_t w, int8_t h, uint8_t color),
    fillRect(int8_t x, int8_t y, int8_t w, int8_t h, uint8_t color),
    fillScreen(uint8_t color),
    invertDisplay(boolean i),
    defillementEcranVt(int8_t h);



  // These exist only with Adafruit_GFX (no subclass overrides)
  void
    drawCircle(int8_t x0, int8_t y0, int8_t r, uint8_t color),
    drawCircleHelper(int8_t x0, int8_t y0, int8_t r, uint8_t cornername,
      uint8_t color),
    fillCircle(int8_t x0, int8_t y0, int8_t r, uint8_t color),
    fillCircleHelper(int8_t x0, int8_t y0, int8_t r, uint8_t cornername,
      int8_t delta, uint8_t color),
    drawTriangle(int8_t x0, int8_t y0, int8_t x1, int8_t y1,
      int8_t x2, int8_t y2, uint8_t color),
    fillTriangle(int8_t x0, int8_t y0, int8_t x1, int8_t y1,
      int8_t x2, int8_t y2, uint8_t color),
    drawRoundRect(int8_t x0, int8_t y0, int8_t w, int8_t h,
      int8_t radius, uint8_t color),
    fillRoundRect(int8_t x0, int8_t y0, int8_t w, int8_t h,
      int8_t radius, uint8_t color),
    drawBitmap(int8_t x, int8_t y, const uint8_t *bitmap,
      int8_t w, int8_t h, uint8_t color),
    drawBitmap(int8_t x, int8_t y, const uint8_t *bitmap,
      int8_t w, int8_t h, uint8_t color, uint8_t bg),
    drawXBitmap(int8_t x, int8_t y, const uint8_t *bitmap,
      int8_t w, int8_t h, uint8_t color),
    drawChar(int8_t x, int8_t y, unsigned char c, uint8_t color,
      uint8_t bg, uint8_t size),
    drawCharReduit(int8_t x, int8_t y, unsigned char c, uint8_t color,
      uint8_t bg),

    setCursor(int8_t x, int8_t y),
    setTextColor(uint8_t c),
    setTextColor(uint8_t c, uint8_t bg),
    setTextSize(uint8_t s),
    setTextWrap(boolean w),
    setRotation(uint8_t r),
    cp437(boolean x=true);

#if ARDUINO >= 100
  virtual size_t write(uint8_t);
#else
  virtual void   write(uint8_t);
#endif

  int8_t height(void) const;
  int8_t width(void) const;

  uint8_t getRotation(void) const;

  // get current cursor position (get rotation safe maximum values, using: width() for x, height() for y)
  int8_t getCursorX(void) const;
  int8_t getCursorY(void) const;



 protected:
  const int8_t
    WIDTH, HEIGHT;   // This is the 'raw' display w/h - never changes
  int8_t
    _width, _height, // Display w/h as modified by current rotation
    cursor_x, cursor_y;
  uint8_t
    textcolor, textbgcolor;
  uint8_t
    textsize,
    rotation;
  boolean
    wrap,   // If set, 'wrap' text at right edge of display
    _cp437; // If set, use correct CP437 charset (default is off)
};

class Adafruit_GFX_Button_NB8bits {

 public:
  Adafruit_GFX_Button_NB8bits(void);
  void initButton(Adafruit_GFX_NB8bits *gfx, int8_t x, int8_t y,
		      uint8_t w, uint8_t h,
		      uint8_t outline, uint8_t fill, uint8_t textcolor,
		      char *label, uint8_t textsize);
  void drawButton(boolean inverted = false);
  boolean contains(int8_t x, int8_t y);

  void press(boolean p);
  boolean isPressed();
  boolean justPressed();
  boolean justReleased();

 private:
  Adafruit_GFX_NB8bits *_gfx;
  int8_t _x, _y;
  uint8_t _w, _h;
  uint8_t _textsize;
  uint8_t _outlinecolor, _fillcolor, _textcolor;
  char _label[10];

  boolean currstate, laststate;
};

#endif // _ADAFRUIT_GFX_v1_0_2_NB_H
