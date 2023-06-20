#include "DebugLogger.h"

DebugLogger::DebugLogger(){
  this->output=NULL;
  modeOutput=modeDebug; // niveau de débug sur la sortie (si l'entré est d'un niveau supérieur, ne l'envoi pas à la sortie)
  modeWrite=modeDebug; // niveau de debug de toutes les entrées
}

bool DebugLogger::setOutput(HardwareSerial& serial){
  this->output=&serial;
  return true;
}


size_t DebugLogger::write(uint8_t c){
  if( modeOutput >= modeWrite){
    output->write(c);
  }

  return 1;
}

size_t DebugLogger::write(const uint8_t *buffer, size_t size){
  size_t i;
  for(;i<size;++i){
    this->write(buffer[i]);
  }
  return i;
}
