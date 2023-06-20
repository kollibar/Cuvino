#ifndef __CUVINO_ERREURS_H__
#define __CUVINO_ERREURS_H__


#include <broches.h>
#include <Timer.h>

#define TIMER_INVALIDE -10

signed char timerAlerte = TIMER_INVALIDE;
unsigned char etatAlert = 0;

namespace CuvinoErreurs{
  void startLEDClignotante();
  void stopLEDClignotante();
  void LEDClignotante();

  bool menuErreurs();
  void _creeLigneMenuErreurs(uint8_t i, char* chaine, uint8_t taille, void* arg);
}

#endif
