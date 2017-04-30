#ifndef __PROGRAMME_H__
#define __PROGRAMME_H__

#include <BlocMem.h>
#include <RtcDateTime.h>
#include <EEPROM_fct.h>
#include <Sonde.h>
#include <DebugLogger.h>
#include "lang_fr.h"

#define DEBUG

#define MODE_PROG_NUL       0b000
#define MODE_PROG_10M       0b110
#define MODE_PROG_HC_HP     0b100
#define MODE_PROG_HP_HC     0b101
#define MODE_PROG_H         0b111
#define MODE_PROG_J         0b001
#define MODE_PROG_S         0b010
#define MODE_PROG_M         0b011

class plageHC_HP {
  public:
    uint8_t nb_plages;
    uint8_t horaires[6];
    /* horaires contient les plages heures creuses/ heures pleines.
      les bit pair (0,2 et 4) contiennent le début de la plage heure creuses
      les autres contient la fin de la plage
      la plage est noté par demi-heure
    */

    uint16_t save(uint16_t addr) const;
    uint16_t load(uint16_t addr);
    bool save(BlocMem* bloc) const;
    bool load(BlocMem* bloc);
    void defaut(void);

    plageHC_HP operator=(plageHC_HP const &plage);
};

#define TAILLE_MEM_PROGRAMME 10

class Programme {
  public:
    uint32_t date;
    // date => programme avec repetition si 0x30000000

    unsigned char cuve;
    unsigned char alarme;
    signed int temp;

    Programme();

    bool set(RtcDateTime& date,unsigned char cuve,signed int temp);
    bool set(uint8_t modeRepetition,unsigned char cuve,signed int temp);
    bool set(uint8_t modeRepetition,uint8_t frequenceRepetition, unsigned char cuve,signed int temp);
    bool set(uint8_t modeRepetition,uint8_t frequenceRepetition, uint32_t date, unsigned char cuve,signed int temp);

    bool setDate(RtcDateTime& date);
    bool setRepetition(uint8_t modeRepetition);
    bool setRepetition(uint8_t modeRepetition,uint8_t frequenceRepetition);
    bool setRepetition(uint8_t modeRepetition,uint8_t frequenceRepetition, uint32_t date);

    bool setCuve(unsigned char cuve,signed int temp);

    uint16_t save(uint16_t addr) const;
    uint16_t load(uint16_t addr);
    // bool save(BlocMembloc);
    // bool load(BlocMem* bloc);
    void defaut(void);

    //    bool save(BlocMem* bloc);
    //    bool load(BlocMem* bloc);
    //    void defaut(void);

    bool estRepetition() const;
    uint32_t prochain(RtcDateTime& now,plageHC_HP& HC_HP,bool precedent=false);
    uint32_t precedent(RtcDateTime& now,plageHC_HP& HC_HP);
    uint32_t prochain();

    Programme operator=(Programme const &programme);

    //    // allows for comparisons to just work (==, <, >, <=, >=, !=)
    //    operator uint32_t() const
    //    {
    //        return prochain();
    //    }

    uint8_t freqRepetition() const;
    uint8_t mode() const;
    uint32_t getDate() const;
    RtcDateTime getRtcDateTime() const;
    unsigned long frequence() const;
    unsigned long decalage() const;

    size_t print(HardwareSerial& serial);
    size_t print(DebugLogger& debug);
    size_t println(DebugLogger& debug);
    size_t println(HardwareSerial& serial);

};

#define TAILLE_ENTETE_BLOCMEMPROGRAMME (5+15)
#define TAILLE_DATA_ENTETE_BLOCMEMPROGRAMME (15)

/* classe BlocMemProgramme
classe premettant l'enregistrement d'une liste de Programme en EEPROM
Cette classe est fille de BlocMem
En EEPROM, les données sont organisées comme suit:
/------ Entête de la class BlocMem
|  crc16: 2 octets
|  taille: 2 octets (taille de la partie data complète)
|  version: 1 octet
\________
/------ Entête supplémentaire spécifique de BlocMemProgramme
|  class HC_HP: 7 octets
|  date dernier acces: 4 octets
\_______
/------Tableau de _nb éléments Programme de la forme:
|
|  /--- éléments Programme
|  | enregistrement de l'objet Programme: 10 octets
|  | crc8 de l'objet programme précédent: 1 octet
|  \___
\_______


*/

class BlocMemProgramme : public BlocMem {
  public:
    BlocMemProgramme();
    BlocMemProgramme(uint16_t addr);
    uint8_t alloc(uint8_t nb, bool reduit = false);
    uint8_t alloc(void) const {
      return _nb;
    }
    bool getHC_HP(plageHC_HP& HC_HP, bool forceLecture=false) const;
    bool setHC_HP(const plageHC_HP& HC_HP);
    uint16_t adresseHC_HP(void) const{ return 0;}
    uint16_t adresseHC_HP_EEPROM(void) const { return _addr+TAILLE_ENTETE_BLOCMEMPROGRAMME - TAILLE_DATA_ENTETE_BLOCMEMPROGRAMME + this->adresseHC_HP();}
    bool get(uint8_t num, Programme &programme);
    bool set(uint8_t num, const Programme &programme);
    bool del(uint8_t num);
    bool add(const Programme &programme);
    uint8_t count() const {
      return _nbOK;
    }

    uint32_t getDateDernierAcces(void);
    bool setDateDernierAcces(uint32_t date);
    uint16_t adresseDateDernierAccess(void) const {return 7;}
    uint16_t adresseDateDernierAccess_EEPROM(void) const{ return _addr+TAILLE_ENTETE_BLOCMEMPROGRAMME - TAILLE_DATA_ENTETE_BLOCMEMPROGRAMME + this->adresseDateDernierAccess();}

    uint32_t getDateProchain(void);
    bool setDateProchain(uint32_t date);
    uint16_t adresseDateProchain(void) const {return 11;}
    uint16_t adresseDateProchain_EEPROM(void) const{ return _addr+TAILLE_ENTETE_BLOCMEMPROGRAMME - TAILLE_DATA_ENTETE_BLOCMEMPROGRAMME + this->adresseDateProchain();}

    bool estRepetition(uint8_t num);
    uint8_t freqRepetition(uint8_t num);
    uint8_t mode(uint8_t num);
    uint32_t getDate(uint8_t num);
    RtcDateTime getRtcDateTime(uint8_t num);
    unsigned long frequence(uint8_t num);
    unsigned long decalage(uint8_t num);

    uint8_t prochain(RtcDateTime& now,uint8_t p=0xFF);
    uint8_t precedent(RtcDateTime& now,uint8_t p=0xFF);

    plageHC_HP _HC_HP;

  protected:
    uint8_t _nb;
    uint8_t _nbOK;
    void compacte(void);
    uint8_t _count(void);

    Programme _programme;
    uint8_t _numCharge;

    uint16_t adresseElt_EEPROM(uint8_t num) const;
    uint16_t adresseElt(uint8_t num) const;
    bool getAt(uint8_t num, Programme& programme);
    bool setAt(uint8_t num, const Programme& programme);
    bool testAt(uint8_t num);
    bool delAt(uint8_t num, bool decal = true);

};

#endif
