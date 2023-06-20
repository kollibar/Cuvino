#ifndef __CLAVIER_xD_H__
#define __CLAVIER_xD_H__

#include "Clavier.h"
#include <broches.h>
#include <Arduino.h>

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

#define NB_TOUCHE_MAX 8

typedef volatile uint8_t PortReg;
typedef uint8_t PortMask;

class ToucheDigit{
public:
  ToucheDigit(unsigned char broche, unsigned char touche);
  ToucheDigit();
  void begin();
  bool active();
protected:
  unsigned char broche;
  unsigned char touche;
  volatile PortReg *portIn;
  PortMask pinmask;
  friend class Clavier_xD;
};

class Clavier_xD:public Clavier{
public:
  Clavier_xD(ToucheDigit* touches, unsigned char nbTouche, FonctionUpdate _fctUpdate);
  Clavier_xD(ToucheDigit* touches, unsigned char nbTouche);
  virtual void begin();
  virtual uint8_t getToucheAct(bool valDerTouche = true);
  virtual bool aucuneTouche(bool attend = true);
protected:
  ToucheDigit* touches;
  unsigned char nbTouche;
};


#endif
