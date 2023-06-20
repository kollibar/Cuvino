#ifndef __DEBUG_LOGGER_H__
#define __DEBUG_LOGGER_H__

#include <PrintPGM.h>


enum modeDEBUG {
  modeDebugNA = 0,
  modeDebugComplet = 6,
  modeDebug = 5,
  modeVerbose = 4,
  modeWarning = 3,
  modeErreur = 2,
  modeAucun = 1,
};

class MasterDebugLogger : public PrintPGM{
  public:
    MasterDebugLogger();
    bool setWriteMode(modeDEBUG mode){ modeWrite=mode;return true;};
    bool setOutputMode(modeDEBUG mode){ modeOutput=mode;return true;};
    bool setOutput(HardwareSerial& serial);
    modeDEBUG getOutputMode() { return modeOutput;}
    modeDEBUG getWriteMode() { return modeWrite;}

    unsigned char getNumModuleSuiv();

    /*size_t printPGM(const char*,const modeDEBUG mode);
    size_t printlnPGM(const char*,const modeDEBUG mode);*/

  protected:
    friend class DebugLogger;

    unsigned char nbModule;
    unsigned char dernierModule;
    bool newLine;
    modeDEBUG modeOutput;
    modeDEBUG modeWrite;
    HardwareSerial* output;

    size_t write(uint8_t);
    size_t write(const uint8_t *buffer, size_t size);
};


class DebugLogger{
public:
  DebugLogger(const char* nomModule);
  DebugLogger();

  bool setWriteMode(modeDEBUG mode){ modeWrite=mode;return true;};
  modeDEBUG getWriteMode() { return modeWrite;}

  bool setOutputMode(modeDEBUG mode){this->master->setOutputMode(mode);};
  modeDEBUG getOutputMode() { return this->master->getOutputMode();}

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

    size_t write(uint8_t);
    size_t write(const char *str) {
      if (str == NULL) return 0;
      return write((const uint8_t *)str, strlen(str));
    }
    size_t write(const uint8_t *buffer, size_t size);
    size_t write(const char *buffer, size_t size) {
      return write((const uint8_t *)buffer, size);
    }

    size_t print(const __FlashStringHelper *);
    size_t print(const String &);
    size_t print(const char[]);
    size_t print(char);

    size_t print(unsigned char, int = DEC);
    size_t print(int, int = DEC);
    size_t print(unsigned int, int = DEC);
    size_t print(long, int = DEC);
    size_t print(unsigned long, int = DEC);
    size_t print(double, int = 2);

    size_t print(const Printable&);

    size_t println(const __FlashStringHelper *);
    size_t println(const String &s);
    size_t println(const char[]);
    size_t println(char);

    size_t println(unsigned char, int = DEC);
    size_t println(int, int = DEC);
    size_t println(unsigned int, int = DEC);
    size_t println(long, int = DEC);
    size_t println(unsigned long, int = DEC);
    size_t println(double, int = 2);

    size_t println(const Printable&);
    size_t println(void);

  protected:
    modeDEBUG modeWrite;
    modeDEBUG save;
    size_t entete(void);
    void close(void);
    MasterDebugLogger* master;
    unsigned char numModule=255;
    const char* nomModule;
};

extern MasterDebugLogger Debug;

#endif
