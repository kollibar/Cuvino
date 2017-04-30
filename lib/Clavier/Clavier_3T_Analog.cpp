#include "Clavier_3T_Analog.h"

Clavier_3T_Analog::Clavier_3T_Analog(FonctionUpdate _fctUpdate): Clavier(_fctUpdate){
  return;
}

Clavier_3T_Analog::Clavier_3T_Analog(): Clavier(){
  return;
}

void Clavier_3T_Analog::begin(){
  DDR_CLAVIER  &= ~(1 << BIT_CLAVIER); // mise de la broche du clavier en lecture
}

uint8_t Clavier_3T_Analog::getToucheAct(bool valDerTouche) {
  // renvoi la valeur de la touche actuellement pressé
  int16_t val = 0;
  int16_t val2;
  uint8_t touche;
  for (;;) {
    do {
      val2 = val;
      //fctUpdate();
      if ( (PIN_CLAVIER & ( 1 << BIT_CLAVIER )) == 0) return 0; // si la lecture numérique retourne 0 (càd Tension <2,5V) -> pas de touche

      val = analogRead(BROCHE_CLAVIER); // lecture analogique
    } while (val != val2);

    onOffEcran(true);
//#ifdef PORT_LED_ECRAN
//    PORT_LED_ECRAN |= (1 << BIT_LED_ECRAN); // on ralume l'écran
//#endif

    touche = 0;
    if ( val > 500) {
      if ( val < 580 ) touche = BT_HAUT;
      else if ( val < 650 ) touche = BT_BAS;
      else if ( val < 720 ) touche = BT_SELECT;
      else if ( val < 765 ) touche = BT_HAUT | BT_BAS;
      else if ( val < 794 ) touche = BT_HAUT | BT_SELECT;
      else if ( val < 824 ) touche = BT_BAS | BT_SELECT;
      else if ( val < 900 ) touche = BT_BAS | BT_HAUT | BT_SELECT;
    }

    if (touche != 0) {
      if (valDerTouche) {
        dateDerniereTouche = millis();
        derniereTouche = touche;
      }
//#ifdef DEBUG
//            Serial.print("touche:");
//            Serial.print(touche, BIN);
//            Serial.print(" , val:");
//            Serial.println(val);
//#endif
      return touche;
    }
  }
}

bool Clavier_3T_Analog::aucuneTouche(bool attend) {
  // version optimisé pour Clavier_3T_Analog
  uint8_t touche;
  if ( (PIN_CLAVIER & ( 1 << BIT_CLAVIER )) == 0) return true;

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
