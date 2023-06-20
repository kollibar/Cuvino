#include "DebugLogger.h"

MasterDebugLogger::MasterDebugLogger(){
  this->nbModule=0;
  this->dernierModule=0xFFFF;
  this->newLine=true;

  this->output=NULL;
  this->modeOutput=modeDebug; // niveau de débug sur la sortie (si l'entré est d'un niveau supérieur, ne l'envoi pas à la sortie)
  this->modeWrite=modeDebug; // niveau de debug de toutes les entrées
}

/*void DebugLogger::init(){
  this->output=NULL;
  modeOutput=modeDebug; // niveau de débug sur la sortie (si l'entré est d'un niveau supérieur, ne l'envoi pas à la sortie)
  modeWrite=modeDebug; // niveau de debug de toutes les entrées
}*/

bool MasterDebugLogger::setOutput(HardwareSerial& serial){
  this->output=&serial;
  return true;
}

/*size_t DebugLogger::printPGM(const char* c,const modeDEBUG mode){
  if( modeOutput >= mode){
    modeDEBUG m=modeWrite;
    modeWrite=mode;
    size_t size=this->PrintPGM::printPGM(c);
    modeWrite=m;
    return size;
  }
  return 0;
}
size_t DebugLogger::printlnPGM(const char* c,const modeDEBUG mode){
  if( modeOutput >= mode){
    modeDEBUG m=modeWrite;
    modeWrite=mode;
    size_t size=this->PrintPGM::printlnPGM(c);
    modeWrite=m;
    return size;
  }
  return 0;
}*/

size_t MasterDebugLogger::write(uint8_t c){
  if( output != NULL){
    if( modeOutput >= modeWrite){
      output->write(c);
      if( c=='\n') this->newLine=true;
      else this->newLine=false;
      this->dernierModule=0;
      return 1;
    }
  }
  return 0;
}

size_t MasterDebugLogger::write(const uint8_t *buffer, size_t size){
  if( output != NULL){
    if( modeOutput >= modeWrite){
      size_t i;
      for(;i<size;++i){
        this->write(buffer[i]);

        if( buffer[i]=='\n') this->newLine=true;
        else this->newLine=false;
        this->dernierModule=0;

      }
      return i;
    }
  }
  return 0;
}

unsigned char MasterDebugLogger::getNumModuleSuiv(){
  nbModule++;
  return nbModule;
}

MasterDebugLogger Debug=MasterDebugLogger();

DebugLogger::DebugLogger(){
  this->master=&Debug;
  this->nomModule=NULL;
  this->modeWrite=modeDebug;
  if( this->numModule == 255) this->numModule=this->master->getNumModuleSuiv();
}

DebugLogger::DebugLogger(const char* nomModule){
  this->master=&Debug;
  this->nomModule=nomModule;
  this->modeWrite=modeDebug;
  if( this->numModule == 255) this->numModule=this->master->getNumModuleSuiv();
}

size_t DebugLogger::entete(){
  this->save=this->master->modeWrite;
  this->master->modeWrite=this->modeWrite;

  if( this->master->newLine || this->master->dernierModule!=this->numModule){
    if( ! this->master->newLine) this->master->write('\n');
    this->master->printPGM(nomModule);
    this->master->write(':');
  } else{
    this->close();
    return 0;
  }
}

void DebugLogger::close(){
  this->master->dernierModule=this->numModule;
  this->master->modeWrite=this->save;
}

size_t DebugLogger::printPGM(const char *buffer,bool addMaj,uint8_t i){
  size_t size=this->entete();
  size+=this->master->printPGM(buffer,addMaj,i);
  this->close();
  return size;
}

size_t DebugLogger::printlnPGM(const char * buffer, bool addMaj, uint8_t i){
  size_t size=this->entete();
  size+=this->master->printlnPGM(buffer,addMaj,i);
  this->close();
  return size;
}

size_t DebugLogger::print(const __FlashStringHelper * ifsh){
  size_t size=this->entete();
  size+=this->master->print(ifsh);
  this->close();
  return size;
}

size_t DebugLogger::print(const String &s){
  size_t size=this->entete();
  size+=this->master->print(s);
  this->close();
  return size;
}

size_t DebugLogger::print(const char * str){
  size_t size=this->entete();
  size+=this->master->print(str);
  this->close();
  return size;
}

size_t DebugLogger::print(char c){
  size_t size=this->entete();
  size+=this->master->print(c);
  this->close();
  return size;
}

size_t DebugLogger::print(unsigned char b,int base){
  size_t size=this->entete();
  size+=this->master->print(b,base);
  this->close();
  return size;
}

size_t DebugLogger::print(int b, int base){
  size_t size=this->entete();
  size+=this->master->print(b,base);
  this->close();
  return size;
}

size_t DebugLogger::print(unsigned int b, int base){
  size_t size=this->entete();
  size+=this->master->print(b,base);
  this->close();
  return size;
}

size_t DebugLogger::print(long b, int base){
  size_t size=this->entete();
  size+=this->master->print(b,base);
  this->close();
  return size;
}

size_t DebugLogger::print(unsigned long b, int base){
  size_t size=this->entete();
  size+=this->master->print(b,base);
  this->close();
  return size;
}

size_t DebugLogger::print(double b, int base){
  size_t size=this->entete();
  size+=this->master->print(b,base);
  this->close();
  return size;
}

size_t DebugLogger::print(const Printable & x){
  size_t size=this->entete();
  size+=this->master->print(x);
  this->close();
  return size;
}

size_t DebugLogger::println(const __FlashStringHelper * ifsh){
  size_t size=this->entete();
  size+=this->master->println(ifsh);
  this->close();
  return size;
}

size_t DebugLogger::println(const String &s){
  size_t size=this->entete();
  size+=this->master->println(s);
  this->close();
  return size;
}

size_t DebugLogger::println(const char * str){
  size_t size=this->entete();
  size+=this->master->println(str);
  this->close();
  return size;
}

size_t DebugLogger::println(char c){
  size_t size=this->entete();
  size+=this->master->println(c);
  this->close();
  return size;
}

size_t DebugLogger::println(unsigned char b,int base){
  size_t size=this->entete();
  size+=this->master->println(b,base);
  this->close();
  return size;
}

size_t DebugLogger::println(int b, int base){
  size_t size=this->entete();
  size+=this->master->println(b,base);
  this->close();
  return size;
}

size_t DebugLogger::println(unsigned int b, int base){
  size_t size=this->entete();
  size+=this->master->println(b,base);
  this->close();
  return size;
}

size_t DebugLogger::println(long b, int base){
  size_t size=this->entete();
  size+=this->master->println(b,base);
  this->close();
  return size;
}

size_t DebugLogger::println(unsigned long b, int base){
  size_t size=this->entete();
  size+=this->master->println(b,base);
  this->close();
  return size;
}

size_t DebugLogger::println(double b, int base){
  size_t size=this->entete();
  size+=this->master->println(b,base);
  this->close();
  return size;
}

size_t DebugLogger::println(const Printable & x){
  size_t size=this->entete();
  size+=this->master->println(x);
  this->close();
  return size;
}

size_t DebugLogger::println(void){
  size_t size=this->entete();
  size+=this->master->println();
  this->close();
  return size;
}

size_t DebugLogger::printNdigits(uint8_t nb,uint8_t n){
  size_t size=this->entete();
  size+=this->master->printNdigits(nb, n);
  this->close();
  return size;
}
size_t DebugLogger::printNdigits(uint16_t nb,uint8_t n){
  size_t size=this->entete();
  size+=this->master->printNdigits(nb, n);
  this->close();
  return size;
}
size_t DebugLogger::printNdigits(uint32_t nb,uint8_t n){
  size_t size=this->entete();
  size+=this->master->printNdigits(nb, n);
  this->close();
  return size;
}
size_t DebugLogger::print2digits(uint8_t t){
  size_t size=this->entete();
  size+=this->master->print2digits(t);
  this->close();
  return size;
}
size_t DebugLogger::printEspace(uint8_t nb){
  size_t size=this->entete();
  size+=this->master->printEspace(nb);
  this->close();
  return size;
}
size_t DebugLogger::printDateTime(uint8_t D, uint8_t M, uint8_t Y, uint8_t h,uint8_t m, uint8_t s){
  size_t size=this->entete();
  size+=this->master->printDateTime(D, M, Y, h, m, s);
  this->close();
  return size;
}
size_t DebugLogger::printTime(uint8_t h,uint8_t m, uint8_t s){
  size_t size=this->entete();
  size+=this->master->printTime(h, m, s);
  this->close();
  return size;
}
size_t DebugLogger::printTemp16e(signed int temp,unsigned char n,unsigned char m){
  size_t size=this->entete();
  size+=this->master->printTemp16e(temp, n, m);
  this->close();
  return size;
}

size_t DebugLogger::write(uint8_t c){
  size_t size=this->entete();
  size+=this->master->write(c);
  this->close();
  return size;
}

size_t DebugLogger::write(const uint8_t *buffer, size_t s){
  size_t size=this->entete();
  size+=this->master->write(buffer,s);
  this->close();
  return size;
}
