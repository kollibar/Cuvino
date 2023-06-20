#include "Logger.h"

Logger::Logger(FonctionWrite _fctWrite): Print(){
  fctWrite=_fctWrite;
}

size_t Logger::write(uint8_t c){
  return fctWrite(c);
}

void Logger::actDisplay(){
  fctWrite(0);
}
