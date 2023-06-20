#include "Clavier.h"

void _fausseUpdate(void){
  return;
}


Clavier::Clavier(){
  derniereTouche = 0; // derniere touche pressée
  dateDerniereTouche = 0; // variable contenant la date en millisecondes du dernier appuis sur une touche
  delaiTouche = DELAI_STANDARD;
  fctUpdate=_fausseUpdate;
}

Clavier::Clavier(FonctionUpdate _fctUpdate){
  derniereTouche = 0; // derniere touche pressée
  dateDerniereTouche = 0; // variable contenant la date en millisecondes du dernier appuis sur une touche
  delaiTouche = DELAI_STANDARD;
  fctUpdate=_fctUpdate;
}

void Clavier::onOffEcran(bool etat){
  return;
}
bool Clavier::etatEcran(void){return false;}



uint8_t Clavier::getTouche(int16_t attenteTouche) {
// attend qu'une touche soit pressé et renvoi le code de la touche
// temps d'attente avant de quitter la routine. -2 attentes infini, >=0 attente en seconde, -1 attente que la LED de l'écran soit eteinte
  uint8_t touche, t;
  uint32_t date_entre = millis();
  uint32_t attenteToucheMillis = 0;
  uint8_t datePression;
  
  if (attenteTouche > 0) attenteToucheMillis = (uint32_t)attenteTouche * 1000;
  if (attenteTouche == 0) attenteToucheMillis = 1;

  for (;;) {
    for (uint8_t count = 250; count != 0; --count) { // vérification du temps toute les 250 boucles
      touche = getToucheAct();
      if (touche != 0) {
        datePression=millis()%256;
        do{ // la touche ne doit pas varier durant le temps DELAI_PRESSION
          fctUpdate();
          for ( uint8_t c = 10; c > 0; --c) { // toutes les 10 boucles, fait une verif du temps
            t = getToucheAct();
            if ( t != touche) break;
          }
        }while((millis()%256) - datePression < DELAI_PRESSION );
        if ( t == touche) {
          #ifdef DEBUG
                    Serial.print("==> touche :");
                    Serial.println(touche, BIN);
          #endif
          return touche;
        }
        break;
      }
      fctUpdate();

      //count--;
    }
    if ( attenteToucheMillis != 0 && (millis() - date_entre) > attenteToucheMillis) return TIMEOUT_DECOMPTE;
    if ( (millis() - dateDerniereTouche) > 60000) { // temps depuis la dernière touche dépassé, extinction de l'écran

      onOffEcran(false);
//#ifdef PORT_LED_ECRAN
//      PORT_LED_ECRAN &= ~(1 << BIT_LED_ECRAN); // éteint l'écran
//#endif
      if (attenteTouche == -1) return TIMEOUT_LED;
    }
  }
}


bool Clavier::aucuneTouche(bool attend) {
  // return true si aucune touche n'est pressé, false dans le cas contraire
  // si le paramètre attend= true (non défaut), attend qu'aucune touche ne soit pressé (ou l'expiration du délais d'attente entre 2 touches)
  uint8_t touche;
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

  if ( getToucheAct()==0 ) return true;
  return false;
}
