#include "Clavier_5T.h"

Clavier_5T::Clavier_5T(FonctionUpdate _fctUpdate): Clavier(_fctUpdate){
  return;
}

Clavier_5T::Clavier_5T(): Clavier(){
  return;
}

void Clavier_5T::begin(){
  DDR_CLAVIER  &= ~MASQUE_CLAVIER; // mise des broches du clavier en lecture
  PORT_CLAVIER |= MASQUE_CLAVIER; // mise en mode pullup des broches du clavier
}

uint8_t Clavier_5T::getToucheAct(bool valDerTouche) {
  // renvoi la valeur de la touche actuellement pressé
  uint8_t touche;


  //fctUpdate();
  if ( (PIN_CLAVIER & MASQUE_CLAVIER ) == MASQUE_CLAVIER ) return 0; // si aucune touche


  onOffEcran(true);
  //#ifdef PORT_LED_ECRAN
  //    PORT_LED_ECRAN |= (1 << BIT_LED_ECRAN); // on ralume l'écran
  //#endif

  touche = 0;

  if( (PIN_CLAVIER & ( 1 << BIT_CLAVIER_BAS)) == 0 ) touche = touche | BT_BAS;
  if( (PIN_CLAVIER & ( 1 << BIT_CLAVIER_HAUT)) == 0 ) touche = touche | BT_HAUT;
  if( (PIN_CLAVIER & ( 1 << BIT_CLAVIER_GAUCHE)) == 0 ) touche = touche | BT_ESC;
  if( (PIN_CLAVIER & ( 1 << BIT_CLAVIER_DROITE)) == 0 ) touche = touche | BT_SELECT;
  if( (PIN_CLAVIER & ( 1 << BIT_CLAVIER_MENU)) == 0 ) touche = touche | BT_MENU;

  if (valDerTouche) {
    dateDerniereTouche = millis();
    derniereTouche = touche;
  }

  return touche;
}

bool Clavier_5T::aucuneTouche(bool attend) {
  // version optimisé pour Clavier_5T
  uint8_t touche;
  
  if ( (PIN_CLAVIER & ( MASQUE_CLAVIER )) == MASQUE_CLAVIER ) return true;

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
