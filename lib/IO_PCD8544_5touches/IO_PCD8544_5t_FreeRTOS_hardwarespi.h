#ifndef __IO_PCD8544_5T_FREERTOS_HARDWARESPI_H__
#define __IO_PCD8544_5T_FREERTOS_HARDWARESPI_H__


#include <avr/pgmspace.h>
#include <PCD8544.h>
#include <Clavier_5T_FreeRTOS.h>
#include <Clavier.h>
#include "IO_PCD8544_5T_FreeRTOS.h"

class IO_PCD8544_5T_SPI_hardware_FreeRTOS: public Adafruit_PCD8544, public Clavier_5T_FreeRTOS{
public:
  IO_PCD8544_5T_SPI_hardware_FreeRTOS(int8_t DC, int8_t CS, int8_t RST, FonctionUpdate _fctUpdate);
  // Software SPI with CS tied to ground.  Saves a pin but other pins can't be shared with other hardware.
  IO_PCD8544_5T_SPI_hardware_FreeRTOS(int8_t DC, int8_t RST, FonctionUpdate _fctUpdate);

  // ***** Sans la fonction d'update du clavier /!\ dans ce cas, les attentes clavier seront bloquante /!\.
  IO_PCD8544_5T_SPI_hardware_FreeRTOS(int8_t DC, int8_t CS, int8_t RST);
  // Software SPI with CS tied to ground.  Saves a pin but other pins can't be shared with other hardware.
  IO_PCD8544_5T_SPI_hardware_FreeRTOS(int8_t DC, int8_t RST);

  IO_PCD8544_5T_SPI_hardware_FreeRTOS(pcd8544PinFct fctPin, pcd8544RstFct fctRst);
  IO_PCD8544_5T_SPI_hardware_FreeRTOS(pcd8544PinFct fctPin, pcd8544RstFct fctRst, FonctionUpdate _fctUpdate);

  void display();
  void command(uint8_t c);
  void data(uint8_t c);

  // change la fonction d'update du clavier
  void setUpdateFct(FonctionUpdate _fctUpdate);

  void begin(uint8_t contrast = 40, uint8_t bias = 0x04);
  void onOffEcran(bool etat); // réécriture de la fonction d'allumage/extinction de l'écran
  bool etatEcran(void);

  // widgets supplémentaires
  /*
  retourSelect menuListeOld(char* chaine, uint8_t nb, uint8_t taille = 15, uint8_t yMin = 0, uint8_t yMax = LCDHEIGHT, uint8_t xMin = 0, uint8_t xMax = LCDWIDTH, uint8_t hLigne = 1, uint8_t depart = 0, uint8_t textSize = 1, uint8_t actual = 0, uint8_t permissions = 0b10010000);  // pour ne pas supprimer le code si besoin
  retourSelect menuListe(char* chaine, uint8_t nb, uint8_t taille = 15, uint8_t yMin = 0, uint8_t yMax = LCDHEIGHT, uint8_t xMin = 0, uint8_t xMax = LCDWIDTH, uint8_t hLigne = 1, uint8_t depart = 0, uint8_t textSize = 1, uint8_t actual = 0, uint8_t permissions = 0b10010000); // pour compatibilité
  retourSelect menuListeV2(void (*callback)(uint8_t, char*, uint8_t, void*), void* arg, uint8_t nb, uint8_t taille = 15, uint8_t yMin = 0, uint8_t yMax = LCDHEIGHT, uint8_t xMin = 0, uint8_t xMax = LCDWIDTH, uint8_t hLigne = 1, uint8_t depart = 0, uint8_t textSize = 1, uint8_t actual = 0, uint8_t permissions = 0b10010000);
  retourSelect menuListeOuiNon(uint8_t yMin = 0, uint8_t yMax = LCDHEIGHT, uint8_t xMin = 0, uint8_t xMax = LCDWIDTH, uint8_t hLigne = 1, uint8_t depart = 0, uint8_t textSize = 1, uint8_t permissions = 0b10010000);

  void startLogger(int8_t x,int8_t y, int8_t w,int8_t h);
  void stopLogger();
  void log(uint8_t);*/
  //Widgets widgets=Widgets(this);
protected:
  #ifdef PORT_LED_ECRAN

  #else
  int8_t _led=48;
  bool _etatEcran=false;
  #endif

};

#endif