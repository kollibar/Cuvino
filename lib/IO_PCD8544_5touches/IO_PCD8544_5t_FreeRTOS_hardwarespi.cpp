#include "IO_PCD8544_5t_FreeRTOS_hardwarespi.h"

IO_PCD8544_5T_SPI_hardware_FreeRTOS::IO_PCD8544_5T_SPI_hardware_FreeRTOS(int8_t DC, int8_t CS, int8_t RST) : Adafruit_PCD8544(DC, CS, RST), Clavier_5T_FreeRTOS(_fausseUpdate){
    //widgets=Widgets(this);
    return;
}

IO_PCD8544_5T_SPI_hardware_FreeRTOS::IO_PCD8544_5T_SPI_hardware_FreeRTOS(int8_t DC,  int8_t RST) : Adafruit_PCD8544(DC, 0, RST), Clavier_5T_FreeRTOS(_fausseUpdate) {
    //widgets=Widgets(this);
    return;
}

IO_PCD8544_5T_SPI_hardware_FreeRTOS::IO_PCD8544_5T_SPI_hardware_FreeRTOS(pcd8544PinFct fctPin, pcd8544RstFct fctRst) : Adafruit_PCD8544(fctPin, fctRst), Clavier_5T_FreeRTOS(_fausseUpdate){
    //widgets=Widgets(this);
    return;
}

IO_PCD8544_5T_SPI_hardware_FreeRTOS::IO_PCD8544_5T_SPI_hardware_FreeRTOS(pcd8544PinFct fctPin, pcd8544RstFct fctRst, void(*_fct_update)(void)) : Adafruit_PCD8544(fctPin, fctRst), Clavier_5T_FreeRTOS(_fct_update) {
    //widgets=Widgets(this);
    return;
}


IO_PCD8544_5T_SPI_hardware_FreeRTOS::IO_PCD8544_5T_SPI_hardware_FreeRTOS(int8_t DC, int8_t CS, int8_t RST,void(*_fct_update)(void)) : Adafruit_PCD8544( DC, CS, RST), Clavier_5T_FreeRTOS(_fct_update) {
    //widgets=Widgets(this);
    return;
}

IO_PCD8544_5T_SPI_hardware_FreeRTOS::IO_PCD8544_5T_SPI_hardware_FreeRTOS(int8_t DC, int8_t RST,void(*_fct_update)(void)) : Adafruit_PCD8544( DC, 0, RST), Clavier_5T_FreeRTOS(_fct_update) {
    //widgets=Widgets(this);
    return;
}



void IO_PCD8544_5T_SPI_hardware_FreeRTOS::display(void){
  taskENTER_CRITICAL();
    Adafruit_PCD8544::display();
  taskEXIT_CRITICAL();
}

void IO_PCD8544_5T_SPI_hardware_FreeRTOS::command(uint8_t c){
  taskENTER_CRITICAL();
    Adafruit_PCD8544::command(c);
  taskEXIT_CRITICAL();
}
void IO_PCD8544_5T_SPI_hardware_FreeRTOS::data(uint8_t c){
  taskENTER_CRITICAL();
    Adafruit_PCD8544::data(c);
  taskEXIT_CRITICAL();
}

void IO_PCD8544_5T_SPI_hardware_FreeRTOS::setUpdateFct(FonctionUpdate _fctUpdate){
  fctUpdate=_fctUpdate;
}

void IO_PCD8544_5T_SPI_hardware_FreeRTOS::onOffEcran(bool etat){
  #ifdef PORT_LED_ECRAN
  if(etat) PORT_LED_ECRAN |= (1 << BIT_LED_ECRAN); // on ralume l'écran
  else PORT_LED_ECRAN &= ~(1 << BIT_LED_ECRAN); // éteint l'écran
  #else
  if( _led !=0 ){
    if(etat) digitalWrite(_led,HIGH);
    else digitalWrite(_led, LOW);
    _etatEcran=etat;
  }
  #endif
  return;
}
bool IO_PCD8544_5T_SPI_hardware_FreeRTOS::etatEcran(void){
  #ifdef PORT_LED_ECRAN
    return ((PORT_LED_ECRAN&(1<<BIT_LED_ECRAN)) != 0);
  #else
    return _etatEcran;
    return (bool)digitalRead(_led);
    _etatEcran=true;
  #endif
}

void IO_PCD8544_5T_SPI_hardware_FreeRTOS::begin(uint8_t contrast, uint8_t bias){
  Adafruit_PCD8544::begin(contrast, bias);
  Clavier_5T_FreeRTOS::begin();
  #ifdef PORT_LED_ECRAN
    DDR_LED_ECRAN |= (1 << BIT_LED_ECRAN); // met la broche de la led de l'ecran en mode out
    PORT_LED_ECRAN |= (1 << BIT_LED_ECRAN); // allume la led
  #else
    if( _led >=0) pinMode(_led, OUTPUT);
    digitalWrite(_led,HIGH);
  #endif
}