#ifndef __CUVINO_PROGRAMME_H__
#define __CUVINO_PROGRAMME_H__

#include "Programme.h"
#include <BlocMem.h>

#define AUCUN_PROCHAIN_PROG  0xFFFFFFFF

namespace CuvinoProgramme{
  void loadProgramme(void);
  bool menuConfigProgrammateur(void);
  void _creeLigneMenuProgrammateur(uint8_t i, char* chaine, uint8_t taille, void* arg);
  bool regleProgramme(Programme& programme);
  bool menuConfigurationHC_HP(void);
  void afficheLigneHC_HP(plageHC_HP plages, uint8_t n, int8_t y, uint8_t sel = 0);
  void verifProchain(void); // fonction appellé pour vérifier (et activer éventuellement) le prochain évènement programmé
  void initProgramme(bool loadCuvino=true); // a appeller au lancement de Cuvino
  void programmeInterval(uint32_t debut,uint32_t fin,bool write=false); // execute tous les programme dans l'interval donné
  void checkProchain(RtcDateTime& now); // calcul la date du prochain appel et l'enregistre
  void actualiseDateProchainProg();
}

#endif
