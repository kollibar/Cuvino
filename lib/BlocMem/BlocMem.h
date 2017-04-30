#ifndef __BLOCMEM_H__
#define __BLOCMEM_H__

#include <Arduino.h>
#include "EEPROM_fct.h"

#define NB_MAX_BLOC 20

#define TAILLE_ENTETE_BLOCMEM 5


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

bool saveDataBlocMem(uint16_t addr, unsigned char* objet, uint16_t taille, uint16_t pos, uint8_t version = 1, uint16_t tailleMax = 0xFFFF);

class BlocMem {
  public:
    BlocMem(uint16_t addr, uint8_t version = 1, modeBlocMem mode = modeBlocMem_Lecture, uint16_t tailleMax = 0xFFFF);
    BlocMem();
    ~BlocMem();
    bool init(uint16_t addr, uint8_t version = 1, modeBlocMem mode = modeBlocMem_Lecture, uint16_t tailleMax = 0xFFFF);

    //    bool ecrit(unsigned char* objet, uint16_t taille);
    bool ecrit(unsigned char* objet, uint16_t taille, uint16_t position = 0xFFFF);
    //    bool ecrit(uint8_t data);
    //    bool ecrit(uint16_t data);
    bool ecrit(uint8_t data, uint16_t position = 0xFFFF);
    bool ecrit(uint16_t data, uint16_t position = 0xFFFF);
    bool ecrit(uint32_t data, uint16_t position = 0xFFFF);
    //    bool lit(unsigned char* objet, uint16_t taille);
    bool lit(unsigned char* objet, uint16_t taille = 1, uint16_t position = 0xFFFF);
    uint8_t lit8(uint16_t position = 0xFFFF);
    uint16_t lit16(uint16_t position = 0xFFFF);
    uint32_t lit32(uint16_t position = 0xFFFF);
    void close();

    bool reserve(uint16_t taille);
    bool changeMode(modeBlocMem mode);

    bool CRCValide();
    uint16_t CRC16(void);
    uint16_t CRC16lue(void);
    void actualiseEntete(void);

    uint16_t addrFin(void) const {
      return _addr + _taille;
    }
    modeBlocMem mode() const {
      return _mode;
    }
    uint16_t addr() const {
      return _addr;
    }
    uint16_t taille() const {
      return _taille;
    }
    bool curseur(uint16_t addr);
    uint16_t curseur() const {
      return (_curseur - TAILLE_ENTETE_BLOCMEM - _addr);
    }
    uint8_t version() const {
      return _version;
    }

    void ecritVersion(uint8_t);
    uint8_t litVersion() const;


    uint16_t _curseur;
    uint16_t _addr;
    uint16_t _taille;
    uint16_t _tailleMax;
    uint8_t _version;
    modeBlocMem _mode;
};

#endif
