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
#include <croutine.h>
#include <StackMacros.h>
#include <mpu_wrappers.h>
#include <task.h>
#include <portable.h>
/** !FreeRTOS **/

#include "EV2.h"
#include "lang_fr.h"
#include "SondeV2.h"
#include <BlocMem.h>
#include <Erreurs.h>
#include <OneWire.h>
#include "const.h"
#include <DebugLogger.h>
#include "Cuve_DEBUG.h"
#include "brochage.h"

class CuveV2 {
  public:
    SondeV2* sonde;
    signed char sondeNum;
    signed int tempConsigneCuve;
    byte nom;

    ElectroVanne2 EV_F;
    ElectroVanne2 EV_C;

    uint16_t save(uint16_t addr);
    bool save(BlocMem* bloc);
    uint16_t load(uint16_t addr);
    bool load(BlocMem* bloc);
    void defaut(void);
    bool estConfigure();

    bool controlTemp(signed int decalage=0);
    signed int getTemperature();
    bool demandeMesureTemp();
    unsigned long timeToWait();

    signed int getConsigne();
    bool setConsigned(signed int c);
    bool setArret(signed int c);

    signed int getEcart();
    bool setPrecisionMesure(unsigned char precision);
    unsigned char getPrecisionMesure() const;

    void print(HardwareSerial& serial);
    void print(DebugLogger&);

    void begin(PileErreur& _pileErreur,Timer& _timer,OneWire& _ds,QueueHandle_t queueCmdeToCore,QueueHandle_t queueCmdeToTimer);
    void initEV();
    CuveV2 &operator=(const CuveV2 &source);
    bool operator==(const CuveV2 &) const;
    bool operator!=(const CuveV2 &) const;
protected:
    unsigned long dateMesure;
    signed int mesure;
    static PileErreur* pileErreur;
    static Timer* timer;
    static OneWire* ds;
    static QueueHandle_t qCmdeToCore;
    static QueueHandle_t qCmdeToTimer;
};

#endif
