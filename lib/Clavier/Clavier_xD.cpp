#include "Clavier_xD.h"

ToucheDigit::ToucheDigit(){
  this->broche=0;
}

ToucheDigit::ToucheDigit(unsigned char broche, unsigned char touche){
  this->broche=broche;
  this->touche=touche;
  portIn=portInputRegister(digitalPinToPort(broche));
  pinmask=digitalPinToBitMask(broche);
}

void ToucheDigit::begin(){
  pinMode(this->broche, INPUT_PULLUP);
}

bool ToucheDigit::active(){
  return ((*this->portIn & ( pinmask)) == 0 );
}

Clavier_xD::Clavier_xD(ToucheDigit* touches, unsigned char nbTouche, FonctionUpdate _fctUpdate): Clavier(_fctUpdate){
  this->touches=touches;
  this->nbTouche=nbTouche;
}

Clavier_xD::Clavier_xD(ToucheDigit* touches, unsigned char nbTouche): Clavier(){
  this->touches=touches;
  this->nbTouche=nbTouche;
}


void Clavier_xD::begin(){
  for(unsigned char i=0;i<this->nbTouche;++i){
    (this->touches+i)->begin();
  }
}

uint8_t Clavier_xD::getToucheAct(bool valDerTouche) {
  // renvoi la valeur de la touche actuellement pressé
  uint8_t touche;

  //fctUpdate();
  //if ( (PIN_CLAVIER & MASQUE_CLAVIER ) == MASQUE_CLAVIER ) return 0; // si aucune touche


  onOffEcran(true);
  //#ifdef PORT_LED_ECRAN
  //    PORT_LED_ECRAN |= (1 << BIT_LED_ECRAN); // on ralume l'écran
  //#endif

  touche = 0;

  for(unsigned char i=0;i<this->nbTouche;++i){
    if((this->touches+i)->active()) touche = touche | ((this->touches+i)->touche);
  }

  if (valDerTouche) {
    dateDerniereTouche = millis();
    derniereTouche = touche;
  }

  return touche;
}

bool Clavier_xD::aucuneTouche(bool attend) {
  // version optimisé pour Clavier_xD
  uint8_t touche;
  if ( (PIN_CLAVIER & ( MASQUE_CLAVIER )) == 0) return true;

  if (attend) {
    do {
      touche = getToucheAct(false);
      if (touche == derniereTouche && (millis() - dateDerniereTouche) > delaiTouche) {
        delaiTouche = DELAI_REDUIT;
        return true;
      }
      if (touche != derniereTouche) delaiTouche = DELAI_STANDARD;
    } while (touche != 0);
    return true;
  }

  return false;
}
