#ifndef __CUVEV2_H__
#define __CUVEV2_H__

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

#include "EV3.h"
#include "lang_fr.h"
#include "SondeV2.h"
#include <BlocMem.h>
#include <Erreurs.h>
#include <OneWire.h>
#include "const.h"
#include <DebugLogger.h>
#include "Cuve_DEBUG.h"
#include "brochage.h"
#include "circuit.h"


#define CHAUD 1
#define FROID 2

#ifndef NB_CIRCUIT_AUTRE
#define NB_CIRCUIT_AUTRE  1
#endif

class CuveV2 {
  public:
    SondeV2* sonde;
    signed char sondeNum;
    int16_t tempConsigneCuve;
    int16_t tempConsigneMin;
    byte nom;

    ElectroVanne3 EV_F;
    ElectroVanne3 EV_C;

    uint16_t save(uint16_t addr);
    bool save(BlocMem* bloc);
    uint16_t load(uint16_t addr);
    bool load(BlocMem* bloc);
    void defaut(void);

    bool estConfigure();
    bool hasSondeTemp();

    bool controlTemp(signed int decalage=0);
    signed int getTemperature();
    bool demandeMesureTemp();
    unsigned long timeToWait();

    signed int getConsigne();
    bool setConsigned(signed int c);
    bool setArret(signed int c);

    bool setPourcentageActivite(uint8_t vanne, uint8_t p);
    uint8_t getPourcentageActivite(uint8_t vanne);

    bool setConsigne(int16_t max, int16_t min);
    int16_t getConsigneMin();
    int16_t getConsigneMax();

    signed int getEcart();
    bool setPrecisionMesure(unsigned char precision);
    unsigned char getPrecisionMesure() const;

    void print(HardwareSerial& serial);
    void print(DebugLogger&);

    static void begin(PileErreur& _pileErreur,Timer& _timer,OneWire& _ds,QueueHandle_t queueCmdeToCore,QueueHandle_t queueCmdeToTimer, MCP23S17*);
    void initEV();
    CuveV2 &operator=(const CuveV2 &source);
    bool operator==(const CuveV2 &) const;
    bool operator!=(const CuveV2 &) const;

    static uint8_t setCircuit(Circuit* circuit);
    static Circuit* getCircuit(uint8_t n);

protected:
    uint8_t pourcentageActiviteChaud;
    uint8_t pourcentageActiviteFroid;

    uint16_t dernierCheck;
    
    unsigned long dateMesure;
    signed int mesure;
    static PileErreur* pileErreur;
    static Timer* timer;
    static OneWire* ds;
    static QueueHandle_t qCmdeToCore;
    static QueueHandle_t qCmdeToTimer;

    static Circuit* circuitChaud;
    static Circuit* circuitFroid;

    #ifdef NB_CIRCUIT_AUTRE
    #if NB_CIRCUIT_AUTRE > 0
    static Circuit* circuits[NB_CIRCUIT_AUTRE];
    static uint8_t nbCircuits;
    #endif
    #endif
};

#endif
