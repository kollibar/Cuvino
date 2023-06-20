#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <Print.h>
#include <IO_PCD8544_5T.h>

typedef size_t(*FonctionWrite)(uint8_t);

class Logger : public virtual Print{
public:
  Logger(FonctionWrite _fctWrite);
  virtual size_t write(uint8_t);
  void actDisplay();
protected:
  FonctionWrite fctWrite;
};


#endif
