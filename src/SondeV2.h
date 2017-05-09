#ifndef __SONDEV2_H__
#define __SONDEV2_H__

/** FreeRTOS **/
#include <Arduino_FreeRTOS.h>
#include <projdefs.h>
#include <portmacro.h>
#include <list.h>
#include <timers.h>
#include <event_groups.h>
#include <semphr.h>
#include <FreeRTOSVariant.h>
#include <queue.h>
#include <FreeRTOSConfig.h>
#include <croutine.h>
#include <StackMacros.h>
#include <mpu_wrappers.h>
#include <task.h>
#include <portable.h>
/** !FreeRTOS **/

#include <OneWire.h>
#include <broches.h>
#include <RtcDS3231.h>
#include <Wire.h>
// #include <BlocMem.h>
#include <Timer.h>
#include <Erreurs.h>
#include <DebugLogger.h>
#include "Sonde_DEBUG.h"
#include "TempDS3231.h"
#include "fctDS18B20.h"

//#define DEBUG

#define DS18B20 0x28     // Adresse 1-Wire du DS18B20

#define TEMP_LIGNE_NON_CONFIGURE -32761 // affecté à la consigne d'une ligne non configurée
#define TEMP_EV_NON_CONFIGURE -32763 // affecté à la consigne d'une ligne avec EV non configurée
#define TEMP_SONDE_NON_CONFIGURE -32764 // affecté à la consigne d'une ligne avec sonde non configurée
#define TEMP_SONDE_LOCAL -32762 // affecté à la consigne de la ligne NB_MAX_LIGNE (càd hors limite, pour marquer l'affectation de cette ligne à la sonde local)
#define TEMP_ARRET  -32740  // arrêt de la régulation
#define TEMP_MAINTENANCE -32760 // maintenance de la cuve...
#define TEMP_ERREUR -32750  // erreur de temperature: erreur non définie
#define TEMP_ERREUR_ABS_SONDE -32751  // erreur de temperature: pas de sonde
#define TEMP_ERREUR_EV_TEMPS_TROP_LONG  -32752 // erreur electrovanne : temps d'ouverture ou de fermeture bcp trop long
#define TEMP_ERREUR_PAS_SONDE -32753  // erreur ce n'est pas une sonde
#define TEMP_NON_LUE  -32700  // température non encore lue!

#define PRECISION_HAUTE     0
#define PRECISION_MOYENNE   1
#define PRECISION_BASSE     2
#define PRECISION_MAUVAISE  3

const signed char AUCUNE_SONDE = -1;
const signed char SONDE_DS3231 = -2;

class SondeV2{
public:
  unsigned char addr[8];

  //SondeV2();
  SondeV2(OneWire*, signed char num, unsigned char _addr[8]);
  SondeV2(OneWire*, signed char num);

  bool demandeMesureTemp(); // demande une mesure de température à la sonde
  bool mesureFaite();       // demande si la lecture de température est valide
  unsigned long tempsDerniereMesure();  // temps écoulé depuis la dernière mesure
  unsigned long timeToWait(); // retourne le temps à attendre avant que la mesure soit disponible. Si la mesure n'est plus valide demande une nouvelle mesure

  signed int litTemperature(const bool correction = true);
  signed int getTemperature(const bool correction = true); // récupère la température calculé par la sonde, si besoin demande un nouveau calcul, si besoin attends le temps que le calcul soit effectué

  signed int correctionMesure(signed int mesure);
  bool calcCorrection(signed int mesure,signed int tempReelle);
  bool calcCorrection(signed int mesure1,signed int tempReelle1,signed int mesure2,signed int tempReelle2);
  bool isSondeTemp();
  bool isDS3231();
  bool isDS18B20();


  void print(HardwareSerial& serial);
  void print(DebugLogger& debug);


/*
  uint16_t save(uint16_t addr,unsigned char v=1);
  bool save(BlocMem* bloc,unsigned char v=1);
  uint16_t load(uint16_t addr,unsigned char v=1);
  bool load(BlocMem* bloc,unsigned char v=1);
  void defaut(void);*/

  void begin(PileErreur& _pileErreur,Timer& _timer,QueueHandle_t queueCmdeToCore,QueueHandle_t queueCmdeToTimer);

  bool setPrecision(unsigned char precision);
  unsigned char getPrecision() const{return this->precision;}

  signed char num;
protected:
  void chercheAdresse();
  signed char a;
  signed char b;
  unsigned long date;
  unsigned char precision;

  OneWire* ds;

  static PileErreur* pileErreur;
  static Timer* timer;
  static QueueHandle_t qCmdeToCore;
  static QueueHandle_t qCmdeToTimer;

  friend class CuveV2;
};

#endif
