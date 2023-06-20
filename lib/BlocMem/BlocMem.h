#ifndef __BLOCMEM_H__
#define __BLOCMEM_H__

// #include <Arduino.h>
// #include "EEPROM_fct.h"

#include <inttypes.h>
#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#define NB_MAX_BLOC 20

#define TAILLE_ENTETE_BLOCMEM 5

#define TAILLE_EEPROM (E2END+1)

// #define DEBUG


enum modeBlocMem {
  modeBlocMem_Lecture = 'l',
  modeBlocMem_Ecriture = 'e',
  modeBlocMem_LectureEcriture = 'a',
  modeBlocMem_Erreur = 'r',
  modeBlocMem_Erreur2 = 'R',
  modeBlocMem_Close = 'c',
  modeBlocMem_nonInit = 'n',
};

/* class BlocMem
Classe servant de conteneur de données en EEPROM

Enête:
CRC16: 2 octets de tout le bloc (hors crc(!), mais y compris taille et version)
taille: 2 octets
version: 1 octet

DATA: 'taille' octets (variable)

les fonctions ecit et lit (y-compris lit8, lit16, lit32) ont pour paramètre une adresse à l'intéreieur de l'emplacement blocMem sans prendre en compte l'entete
*/

#define BLOCMEM_ENTETE_TAILLE_CRC     2
#define BLOCMEM_ENTETE_TAILLE_TAILLE  2
#define BLOCMEM_ENTETE_TAILLE_VERSION 1
#define BLOCMEM_TAILLE_ENTETE         ( BLOCMEM_ENTETE_TAILLE_CRC + BLOCMEM_ENTETE_TAILLE_TAILLE + BLOCMEM_ENTETE_TAILLE_VERSION )

#define BLOCMEM_ENTETE_POS_CRC        0
#define BLOCMEM_ENTETE_POS_TAILLE     ( BLOCMEM_ENTETE_POS_CRC + BLOCMEM_ENTETE_TAILLE_CRC )
#define BLOCMEM_ENTETE_POS_VERSION    ( BLOCMEM_ENTETE_POS_TAILLE + BLOCMEM_ENTETE_TAILLE_TAILLE )


bool saveDataBlocMem(uint16_t addr, unsigned char* objet, size_t taille, uint16_t pos, uint8_t version = 1, size_t tailleMax = 0xFFFF);

class BlocMem {
  public:
    BlocMem(uint16_t addr, uint8_t version = 1, modeBlocMem mode = modeBlocMem_Lecture, size_t tailleMax = 0xFFFF);
    BlocMem();
    ~BlocMem();
    bool init(uint16_t addr, uint8_t version = 1, modeBlocMem mode = modeBlocMem_Lecture, size_t tailleMax = 0xFFFF);

    //    bool ecrit(unsigned char* objet, size_t taille);
    bool ecrit(void* objet, size_t taille, uint16_t position = 0xFFFF);
    bool ecritPGM(const char* objet, uint16_t position=0xFFFF);
    int8_t strcmp_P(const char* chainePGM, uint16_t position=0xFFFF);
    //    bool ecrit(uint8_t data);
    //    bool ecrit(uint16_t data);
    bool ecrit(uint8_t data, uint16_t position = 0xFFFF);
    bool ecrit(uint16_t data, uint16_t position = 0xFFFF);
    bool ecrit(uint32_t data, uint16_t position = 0xFFFF);
    //    bool lit(unsigned char* objet, size_t taille);
    bool lit(void* objet, size_t taille = 1, uint16_t position = 0xFFFF);
    uint8_t lit8(uint16_t position = 0xFFFF);
    uint16_t lit16(uint16_t position = 0xFFFF);
    uint32_t lit32(uint16_t position = 0xFFFF);
    void close();

    bool reserve(size_t taille);
    bool changeMode(modeBlocMem mode);

    bool CRCValide();
    uint16_t CRC16(void);
    uint16_t CRC16lue(void);
    void actualiseEntete(void);

    uint16_t addrSuivant(void){ return this->_addr + this->_taille + TAILLE_ENTETE_BLOCMEM;}

    uint16_t addrFin(void) const {
      return _addr + _taille + TAILLE_ENTETE_BLOCMEM;
    }

    modeBlocMem mode() const {
      return _mode;
    }
    uint16_t addr() const {
      return this->_addr;
    }
    size_t taille() const {
      return _taille;
    }
    bool setPosCurseur(uint16_t addr);  // anciennement curseur(uint16_t addr)
    uint16_t getPosCurseur() const {  // anciennement curseur()
      return _curseur ;
    }
    uint8_t getVersion() const {  // anciennement version()
      return litVersion();
    }

    void ecritVersion(uint8_t);
    uint8_t litVersion() const;

  protected:
    uint16_t _curseur;
    uint16_t _addr;
    uint16_t _taille;
    uint16_t _tailleMax;
    uint8_t _version;
    modeBlocMem _mode;
};

#endif
