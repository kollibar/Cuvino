#ifndef __SONDE_H__
#define __SONDE_H__

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
#include <mpu_wrappers.h>
#include <task.h>
#include <portable.h>
/** !FreeRTOS **/

#include <avr/eeprom.h>

#include <OneWire.h>
#include <broches.h>
#include <RtcDS3231.h>
#include <Wire.h>
#include <BlocMem.h>
#include <Timer.h>
#include <Erreurs.h>
#include <DebugLogger.h>
#include "Sonde_DEBUG.h"
#include "TempDS3231.h"
#include "brochage.h"
#include "fctDS18B20.h"

//#define DEBUG



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

const unsigned char TEMPS_MAX_ACCES_SONDE = 2000 / portTICK_PERIOD_MS;

class Sonde{
public:
  unsigned char addr[8];

  Sonde();
  Sonde(unsigned char _addr[8]);
  bool demandeMesureTemp();
  bool mesureFaite();
  signed int litTemperature(const bool correction = true);
  signed int getTemperature(const bool correction = true);
  signed int correctionMesure(signed int mesure);
  bool calcCorrection(signed int mesure,signed int tempReelle);
  bool calcCorrection(signed int mesure1,signed int tempReelle1,signed int mesure2,signed int tempReelle2);
  bool isSondeTemp();
  bool isDS3231();
  bool isDS18B20();

  void print(HardwareSerial& serial);
  void print(DebugLogger& debug);

  Sonde &operator=(const Sonde &source);
  bool operator==(const Sonde &) const;
  bool operator!=(const Sonde &) const;

  uint16_t save(uint16_t addr);
  bool save(BlocMem* bloc);
  uint16_t load(uint16_t addr);
  bool load(BlocMem* bloc);
  void defaut(void);

  void begin(PileErreur& _pileErreur,Timer& _timer,OneWire& _ds,QueueHandle_t queueCmdeToCore,QueueHandle_t queueCmdeToTimer);

  bool setPrecision(unsigned char precision);
  unsigned char getPrecision() const{return this->precision;}
protected:
  signed char a;
  signed char b;
  unsigned long date;
  unsigned char precision;
  signed char num;

  static PileErreur* pileErreur;
  static Timer* timer;
  static OneWire* ds;
  static QueueHandle_t qCmdeToCore;
  static QueueHandle_t qCmdeToTimer;
};



#endif
