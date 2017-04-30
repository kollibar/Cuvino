#ifndef __CLAVIER_5T_H__
#define __CLAVIER_5T_H__

#include "Clavier.h"
#include <broches.h>

#ifndef PORT_CLAVIER
  #define PORT_CLAVIER    PORTF
  #define DDR_CLAVIER     DDRF
  #define PIN_CLAVIER     PINF
  #define MASQUE_CLAVIER  B11111000

  #define BROCHE_CLAVIER_HAUT   A5
  #define BIT_CLAVIER_HAUT      5
  #define BROCHE_CLAVIER_BAS    A4
  #define BIT_CLAVIER_BAS       4
  #define BROCHE_CLAVIER_GAUCHE A3
  #define BIT_CLAVIER_GAUCHE    3
  #define BROCHE_CLAVIER_DROITE A6
  #define BIT_CLAVIER_DROITE    6
  #define BROCHE_CLAVIER_MENU A7
  #define BIT_CLAVIER_MENU    7
#endif

class Clavier_5T:public Clavier{
public:
  Clavier_5T(FonctionUpdate _fctUpdate);
  Clavier_5T();
  virtual void begin();
  virtual uint8_t getToucheAct(bool valDerTouche = true);
  virtual bool aucuneTouche(bool attend = true);
};


#endif
