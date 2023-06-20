#ifndef __CLAVIER_5T_H__
#define __CLAVIER_5T_H__

#include "Clavier.h"
#include <broches.h>

#ifndef PORT_CLAVIER
  #define PORT_CLAVIER    PORTK
  #define DDR_CLAVIER     DDRK
  #define PIN_CLAVIER     PINK
  #define MASQUE_CLAVIER  0b01111100

  #define BROCHE_CLAVIER_HAUT   A10
  #define BIT_CLAVIER_HAUT      2
  #define BROCHE_CLAVIER_BAS    A11
  #define BIT_CLAVIER_BAS       3
  #define BROCHE_CLAVIER_GAUCHE A12
  #define BIT_CLAVIER_GAUCHE    4
  #define BROCHE_CLAVIER_DROITE A13
  #define BIT_CLAVIER_DROITE    5
  #define BROCHE_CLAVIER_MENU A14
  #define BIT_CLAVIER_MENU    6
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
