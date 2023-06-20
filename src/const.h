#ifndef __CONST_H__
#define __CONST_H__

#include <avr/pgmspace.h>

#define VERSION_LOGICIEL 1
#define NB_LIGNE_MAX 3
#define NB_EV_MAX 7
#define NB_SONDE_MAX  5
#define NB_PROGRAMME_MAX 20
#define VERSION_MEMOIRE 2
#define LANGUE_FR
#define TEMP_MINI_CONSIGNE  -48   // température minimale de consigne des cuves (en 1/16e de °C)
#define TEMP_MAXI_CONSIGNE   640  // température maximale de consigne des cuves (en 1/16e de °C)
#define HISTERESIS_SIMPLE   8 // histeresis 1/2 écart de température admissible pour l'histeresis (en 1/16e de °C)
#define _1_2_ECART_CHAUD_FROID 4 // ecart entre les temperatures de déclenchement de la vanne chaud et la vanne froid (en 1/16e de °C)
#define HISTERESIS_COMPLEXE 16 //
#define NOM_FICHIER_SD  "data.log"

// #define DEBUG

namespace VERSION{
  const char PROGMEM TXT_LOGICIEL[] = "    Cuvino";
  const char PROGMEM TXT_VERSION[] = "     V0.1";
  const char PROGMEM TXT_VERSION_MEM[] = " memoire V0.1";
}
//extern const char PROGMEM ENTETE_FICHIER_LOG[] = "date,temp_local,tempObj_EV0,temp_EV0,position_EV0,tempObj_EV1,temp_EV1,position_EV1,tempObj_EV2,temp_EV2,position_EV2,tempObj_EV3,temp_EV3,position_EV3";

enum cmdeInterne {
  cmdeActTouteTemp = 'A',
  cmdeActTemp = 'a',
  cmdeActConsigne = 'c',
  cmdeActEtatEV = 'e',
  cmdeActEtatTouteEV = 'E',
  cmdeSave = 's',
  cmdeNewProg = 'n',
  cmdeNull = 0,
};

#endif
