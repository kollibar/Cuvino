#ifndef __CLAVIER_3T_ANALOG_H__
#define __CLAVIER_3T_ANALOG_H__

#include "Clavier.h"

#ifndef BROCHE_CLAVIER
  #define BROCHE_CLAVIER        A4
  #define PORT_CLAVIER          PORTF
  #define DDR_CLAVIER           DDRF
  #define PIN_CLAVIER           PINF
  #define BIT_CLAVIER           4
#endif

class Clavier_3T_Analog : public Clavier {
public:
  Clavier_3T_Analog(FonctionUpdate _fctUpdate);
  Clavier_3T_Analog();
  virtual void begin();
  virtual uint8_t getToucheAct(bool valDerTouche = true);
  virtual bool aucuneTouche(bool attend = true);
};

#endif
