#ifndef __CUVE_H__
#define __CUVE_H__

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
#include "Sonde.h"
#include <BlocMem.h>
#include <Erreurs.h>
#include <OneWire.h>
#include "const.h"
#include <DebugLogger.h>
#include "Cuve_DEBUG.h"
#include "brochage.h"


class Cuve {
  public:
    Sonde sonde;
    signed int tempConsigneCuve;
    byte nom;

    ElectroVanne3 EV_F;
    ElectroVanne3 EV_C;

    uint16_t save(uint16_t addr);
    bool save(BlocMem* bloc);
    uint16_t load(uint16_t addr);
    bool load(BlocMem* bloc);
    void defaut(void);
    bool estConfigure();

    bool controlTemp(signed int decalage=0);
    signed int getTemperature();
    signed int getEcart();
    bool setPrecisionMesure(unsigned char precision);
    unsigned char getPrecisionMesure() const;

    void print(HardwareSerial& serial);
    void print(DebugLogger&);

    void begin(PileErreur& _pileErreur,Timer& _timer,OneWire& _ds,QueueHandle_t queueCmdeToCore,QueueHandle_t queueCmdeToTimer, MCP23S17* gpio);
    void initEV();
    Cuve &operator=(const Cuve &source);
    bool operator==(const Cuve &) const;
    bool operator!=(const Cuve &) const;
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
