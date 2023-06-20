#ifndef __CLAVIER_H__
#define __CLAVIER_H__


#include <stdint.h>
#include <Arduino.h>
#include <broches.h>


// Touches
#define BT_HAUT     B1
#define BT_BAS      B10
#define BT_GAUCHE   B100
#define BT_DROIT    B1000
#define BT_SELECT   B10000
#define BT_ESC      B100000 // = HAUT + BAS
#define BT_MENU     B1000000 // = SELECT + HAUT + BAS
#define TIMEOUT     B10000000
#define TIMEOUT_LED B10000001
#define TIMEOUT_DECOMPTE  B10000010
#define TIMEOUT_SECONDE   B11111111

// #define DEBUG

#define DELAI_STANDARD  1000
#define DELAI_REDUIT    300
#define DELAI_PRESSION  50

typedef void(*FonctionUpdate)(void);

void _fausseUpdate(void);

class Clavier{
public:
  Clavier(FonctionUpdate _fctUpdate);
  Clavier();
  // PEUVENT être réécrite par la class clavier spécifique
  virtual uint8_t getTouche(int16_t attenteTouche); // attends
  virtual bool aucuneTouche(bool touche = true);  // si true, attends qu'aucune touche ne soit pressé, sinon renvoi true si aucune touche pressé ou false si touche pressé
  // DOIT être réécrite par la class clavier spécifique
  virtual uint8_t getToucheAct(bool valDerTouche = true)=0; // récupère la touche pressé actuellement (ou 0 si vide)
  // PEUVENT être réécrite si intégration a une class display
  virtual void onOffEcran(bool etat); // allume ou éteint l'écran (non fonctionnel par défaut)
  virtual bool etatEcran(void); // renvoi true si l'écran et allumé sinon false


  uint16_t getDateDerniereTouche() const{ // renvoi la date du dernier appui sur une touche levé par le clavier
    return dateDerniereTouche;
  }
  uint8_t getDerniereTouche() const{ // renvoi la derniere touche levé par le clavier
    return derniereTouche;
  }
protected:
  uint8_t derniereTouche; // derniere touche pressée
  uint32_t dateDerniereTouche; // variable contenant la date en millisecondes du dernier appuis sur une touche
  uint16_t delaiTouche;
  FonctionUpdate fctUpdate; // fonction appellée à chaque boucle clavier pour ne pas bloquer le programme sur le clavier
};

#endif
