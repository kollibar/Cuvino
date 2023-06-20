#ifndef __PRINTPGM_H__
#define __PRINTPGM_H__


#include <Print.h>
#include <math.h>
#include <Arduino.h>


class PrintPGM : public Print {
public:
  PrintPGM();
  size_t
    printNdigits(uint8_t nb,uint8_t n),
    printNdigits(uint16_t nb,uint8_t n),
    printNdigits(uint32_t nb,uint8_t n),
    print2digits(uint8_t),
    printEspace(uint8_t nb=1),
    printPGM(const char*,bool addMaj=false,uint8_t i=0),
    printlnPGM(const char*,bool addMaj=false,uint8_t i=0),
    printDateTime(uint8_t D, uint8_t M, uint8_t Y, uint8_t h,uint8_t m, uint8_t s=255),
    printTime(uint8_t h,uint8_t m, uint8_t s=255),
    printTemp16e(signed int temp,unsigned char n=0,unsigned char m=4);
};
#endif
