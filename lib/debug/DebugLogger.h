#ifndef __DEBUG_LOGGER_H__
#define __DEBUG_LOGGER_H__

#include <PrintPGM.h>


enum modeDEBUG {
  modeDebugNA = 255,
  modeDebug = 4,
  modeVerbose = 3,
  modeWarning = 2,
  modeErreur = 1,
  modeAucun = 0,
};

class DebugLogger : public PrintPGM{
  public:
    DebugLogger();
    bool setWriteMode(modeDEBUG mode){ modeWrite=mode;return true;};
    bool setOutputMode(modeDEBUG mode){ modeOutput=mode;return true;};
    bool setOutput(HardwareSerial& serial);
    modeDEBUG getOutputMode() { return modeOutput;}
    modeDEBUG getWriteMode() { return modeWrite;}

  protected:
    modeDEBUG modeOutput;
    modeDEBUG modeWrite;
    HardwareSerial* output;

    size_t write(uint8_t);
    size_t write(const uint8_t *buffer, size_t size);
};

extern DebugLogger debug=DebugLogger();

#endif
