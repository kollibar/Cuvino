#ifndef __PCD8544_3TOUCHES_OS48_H__
#define __PCD8544_3TOUCHES_OS48_H__

#include <avr/pgmspace.h>
#include <PCD8544.h>
#include <Clavier_OS48.h>
//#include <Logger.h>
#include <os48.h>

using namespace os48;

//extern const char PROGMEM TXT_OUI[]="oui";
//extern const char PROGMEM TXT_NON[]="non";

/*struct retourSelect {
  uint8_t touche;
  uint8_t result;
};*/


class IO_PCD8544_3touches_OS48: virtual public Adafruit_PCD8544, public Clavier_3T_Analog_OS48{
public:
  IO_PCD8544_3touches_OS48(int8_t SCLK, int8_t DIN, int8_t DC, int8_t CS, int8_t RST,Scheduler* _scheduler,FonctionUpdate _fctUpdate);
  // Software SPI with CS tied to ground.  Saves a pin but other pins can't be shared with other hardware.
  IO_PCD8544_3touches_OS48(int8_t SCLK, int8_t DIN, int8_t DC, int8_t RST,Scheduler* _scheduler,FonctionUpdate _fctUpdate);
  // Hardware SPI based on hardware controlled SCK (SCLK) and MOSI (DIN) pins. CS is still controlled by any IO pin.
  // NOTE: MISO and SS will be set as an input and output respectively, so be careful sharing those pins!
  IO_PCD8544_3touches_OS48(int8_t DC, int8_t CS, int8_t RST,Scheduler* _scheduler,FonctionUpdate _fctUpdate);

  // ***** Sans la fonction d'update du clavier /!\ dans ce cas, les attentes clavier seront bloquante /!\.
  IO_PCD8544_3touches_OS48(int8_t SCLK, int8_t DIN, int8_t DC, int8_t CS, int8_t RST,Scheduler* _scheduler);
  // Software SPI with CS tied to ground.  Saves a pin but other pins can't be shared with other hardware.
  IO_PCD8544_3touches_OS48(int8_t SCLK, int8_t DIN, int8_t DC, int8_t RST,Scheduler* _scheduler);
  // Hardware SPI based on hardware controlled SCK (SCLK) and MOSI (DIN) pins. CS is still controlled by any IO pin.
  // NOTE: MISO and SS will be set as an input and output respectively, so be careful sharing those pins!
  IO_PCD8544_3touches_OS48(int8_t DC, int8_t CS, int8_t RST,Scheduler* _scheduler);

  // change la fonction d'update du clavier
  void setUpdateFct(FonctionUpdate _fctUpdate);

  void begin(uint8_t contrast = 40, uint8_t bias = 0x04);
  void onOffEcran(bool etat); // réécriture de la fonction d'allumage/extinction de l'écran

  void display();
  void command(uint8_t c);
  void data(uint8_t c);

// widgets supplémentaires
/*retourSelect menuListeOld(char* chaine, uint8_t nb, uint8_t taille = 15, uint8_t yMin = 0, uint8_t yMax = LCDHEIGHT, uint8_t xMin = 0, uint8_t xMax = LCDWIDTH, uint8_t hLigne = 1, uint8_t depart = 0, uint8_t textSize = 1, uint8_t actual = 0, uint8_t permissions = 0b10010000);  // pour ne pas supprimer le code si besoin
retourSelect menuListe(char* chaine, uint8_t nb, uint8_t taille = 15, uint8_t yMin = 0, uint8_t yMax = LCDHEIGHT, uint8_t xMin = 0, uint8_t xMax = LCDWIDTH, uint8_t hLigne = 1, uint8_t depart = 0, uint8_t textSize = 1, uint8_t actual = 0, uint8_t permissions = 0b10010000); // pour compatibilité
retourSelect menuListeV2(void (*callback)(uint8_t, char*, uint8_t, void*), void* arg, uint8_t nb, uint8_t taille = 15, uint8_t yMin = 0, uint8_t yMax = LCDHEIGHT, uint8_t xMin = 0, uint8_t xMax = LCDWIDTH, uint8_t hLigne = 1, uint8_t depart = 0, uint8_t textSize = 1, uint8_t actual = 0, uint8_t permissions = 0b10010000);
retourSelect menuListeOuiNon(uint8_t yMin = 0, uint8_t yMax = LCDHEIGHT, uint8_t xMin = 0, uint8_t xMax = LCDWIDTH, uint8_t hLigne = 1, uint8_t depart = 0, uint8_t textSize = 1, uint8_t permissions = 0b10010000);

void startLogger(int8_t x,int8_t y, int8_t w,int8_t h);
void stopLogger();
void log(uint8_t);
*/

  //Widgets widgets;
protected:
  #ifdef PORT_LED_ECRAN

  #else
  int8_t _led=48;
  #endif

};

class IO_PCD8544_3touches_SPI_software_OS48: public PCD8544_SPI_Software, public Clavier_3T_Analog_OS48{
public:
  IO_PCD8544_3touches_SPI_software_OS48(int8_t SCLK, int8_t DIN, int8_t DC, int8_t CS, int8_t RST,Scheduler* _scheduler,FonctionUpdate _fctUpdate);
  // Software SPI with CS tied to ground.  Saves a pin but other pins can't be shared with other hardware.
  IO_PCD8544_3touches_SPI_software_OS48(int8_t SCLK, int8_t DIN, int8_t DC, int8_t RST,Scheduler* _scheduler,FonctionUpdate _fctUpdate);

  // ***** Sans la fonction d'update du clavier /!\ dans ce cas, les attentes clavier seront bloquante /!\.
  IO_PCD8544_3touches_SPI_software_OS48(int8_t SCLK, int8_t DIN, int8_t DC, int8_t CS, int8_t RST,Scheduler* _scheduler);
  // Software SPI with CS tied to ground.  Saves a pin but other pins can't be shared with other hardware.
  IO_PCD8544_3touches_SPI_software_OS48(int8_t SCLK, int8_t DIN, int8_t DC, int8_t RST,Scheduler* _scheduler);

  void display();
  void command(uint8_t c);
  void data(uint8_t c);

  // change la fonction d'update du clavier
  void setUpdateFct(FonctionUpdate _fctUpdate);

  void begin(uint8_t contrast = 40, uint8_t bias = 0x04);
  void onOffEcran(bool etat); // réécriture de la fonction d'allumage/extinction de l'écran

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
  #endif

};

//void _creeLigneMenuListeV1(uint8_t i, char* chaine, uint8_t taille, void* arg);
//void _creeLigneMenuListeOuiNon(uint8_t i, char* chaine, uint8_t taille, void* arg);

#endif
