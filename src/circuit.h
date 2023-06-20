#ifndef __CIRCUIT_H__
#define __CIRCUIT_H__

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

#include <canopenObj.h>

#include <BlocMem.h>
#include <Timer.h>
#include <Erreurs.h>
#include <OneWire.h>
#include "lang_fr.h"
#include "const.h"
#include <DebugLogger.h>

#include "EV3.h"
#include "SondeV2.h"


#define CIRCUIT_CHAUD               0b00000001
#define CIRCUIT_FROID               0b00000010
#define CIRCUIT_TYPE_ON_SI_0_EV     0b00000100
#define CIRCUIT_TYPE_OFF_SI_0_EV    0b00001000
#define CICRUIT_TYPE_RESETABLE      0b00010000
#define CIRCUIT_TYPE_2_ACTIONS      0b00100000


#define CIRCUIT_TYPE_TJR_OUI        0b00000000
#define CIRCUIT_TYPE_GROUPE_FROID   0b00010110
#define CIRCUIT_TYPE_CIRCULATEUR    0b00101001


/**
 * Pin de 0 à 7 => PA0 -> PA7
 * 8 à 15 => PC0 -> PC7
 * 16 et 17 => PL3 et PL4 (pwm)
 * 
*/

class Circuit{
    public:
        Circuit();

        void ouvertureEV();
        void fermetureEV();

        bool estPret(uint16_t temperatureDemande);


        void setDureeMiniAvantMesure(uint8_t s);
        void setDureeEntreActivation1et2(uint8_t s);
        uint8_t getDureeMiniAvantMesure();
        uint8_t getDureeEntreActivation1et2();

        void setPin(uint8_t numPin, uint8_t typePin);
        uint8_t getPin(uint8_t typePin);
        uint8_t getNom();
        void setNom(uint8_t nom);
        void setType(uint8_t type);
        uint8_t getType();
        void setTemperatureReset(int16_t temp);
        int16_t getTemperatureReset();

        uint8_t getNbEVOuvert();

/* getsion sonde */
        unsigned long timeToWait();
        bool demandeMesureTemp();
        int16_t getTemperature();


        uint16_t save(uint16_t addr);
        bool save(BlocMem* bloc);
        uint16_t load(uint16_t addr);
        bool load(BlocMem* bloc);
        void defaut(void);


    protected:
        SondeV2* sonde;
        uint8_t nom;
        uint8_t type;
        uint8_t nbEVOuverte;

        uint8_t pin1;
        uint8_t pin2;
        uint8_t tempsEntre_pin1_pin2;
        uint8_t tempsAvantMesure;
        uint16_t tempsAvantReset;
        int16_t temperatureReset;
        uint8_t pinReset;



        void ouverturePremiereEV();
        void fermetureDerniereEV();

        void modificationEtatPin(uint8_t pin, uint8_t etat);
};


#endif