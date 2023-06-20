#ifndef __CUVINO_CORE_H__
#define __CUVINO_CORE_H__

#include "CuveV2.h"
#include "SondeV2.h"
#include <EEPROM.h>
#include <EEPROM_fct.h>
#include <BlocMem.h>
#include <RtcDS3231.h>
#include <RtcDateTime.h>
#include "const.h"

#define ERREUR_CORE_ERREUR_INCONNUE   1
#define ERREUR_CORE_ACCES_DATA_BLOQUE 2


struct AdresseBloc {
  uint16_t principal;
  uint16_t cuves;
  uint16_t programmes;
};


namespace CuvinoCore{
  void sleep();

  void reset();

  void initBroches();
  void init();

  void demandeMesureTemperature(); // lance une demande de lecture des temperatures de toutes les sondes
  void controlTemp(void); // contrôle les températures et ouvre ou ferme les electrovannes

  /* initialisation des cuves */
  void initCuves(void);

  /* horloge */
  void initDS3231(void);

  /* Gestion des paramètres */
  void reglageDefaut(void);
  void saveParams(void);
  size_t saveParamsV2(size_t adresse=0);
  size_t saveParamsV2_cuves(size_t adresse);
  size_t saveParamsV2_principal(size_t adresse, size_t adresseBlocCuves=0xFFFF, size_t adresseBlocProgrammes=0xFFFF);
  bool loadParams(void);
  bool loadParamsV2(size_t adresse);
  bool loadParamsV2_cuves(size_t adresseBlocCuve, uint8_t nb_cuve);

  bool modifTempConsigne(int16_t& temp, int8_t add);

  bool modifieSonde(uint8_t cuve,SondeV2& s);
  bool modifieCuve(uint8_t cuve,CuveV2& c);

  void readBlocPrincipal(size_t adresse);
}



#endif
