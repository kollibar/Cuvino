#include "BlocMem.h"

BlocMem _bloc;

bool saveDataBlocMem(uint16_t addr, unsigned char* objet, uint16_t taille, uint16_t pos, uint8_t version, uint16_t tailleMax) {
  _bloc = BlocMem(addr, version, modeBlocMem_LectureEcriture, tailleMax);
  if ( _bloc.mode() != modeBlocMem_Erreur && _bloc.curseur(pos) && _bloc.ecrit(objet, taille)) {
    _bloc.close();
    return true;
  }
  _bloc.close();
  return false;
}

BlocMem::BlocMem(uint16_t addr, uint8_t version, modeBlocMem mode, uint16_t tailleMax) {
#ifdef DEBUG
  Serial.print("BlocMem(");
  Serial.print(addr);
  Serial.print(",");
  Serial.print(version);
  Serial.print(",");
  Serial.print((char)mode);
  Serial.print(",");
  Serial.print(tailleMax);
  Serial.println(")");
#endif
  _mode = modeBlocMem_Close;
  init(addr, version, mode, tailleMax);
#ifdef DEBUG
  Serial.print("->   ");
  Serial.print("_taille=");
  Serial.print(_taille);
  Serial.print(" _addr=");
  Serial.print(_addr);
  Serial.print(" _mode=");
  Serial.print((char)_mode);
  Serial.print(" _curseur=");
  Serial.println(_curseur);
#endif
}

BlocMem::BlocMem() {
  _mode = modeBlocMem_Close;
  _taille = 0XFFFF;
  _addr = 0XFFFF;
  _curseur = 0XFFFF;
}

bool BlocMem::init(uint16_t addr, uint8_t version, modeBlocMem mode, uint16_t tailleMax) {
  if ( _mode != modeBlocMem_Close) return false; // déjà initialisé

  if ( addr + TAILLE_ENTETE_BLOCMEM >= EEPROM.length()) {
    _mode = modeBlocMem_Erreur;
    return false;
  }
  else {
    _addr = addr;
    _mode = mode;
    _curseur = addr + TAILLE_ENTETE_BLOCMEM;
    if ( tailleMax > EEPROM.length() - _addr) tailleMax = EEPROM.length() - addr;
    _tailleMax = tailleMax;

    if ( mode == modeBlocMem_Ecriture) { // mode ecriture
      _version = version;
      _taille = TAILLE_ENTETE_BLOCMEM;
    }
    else if ( mode == modeBlocMem_Lecture) { // mode lecture
      litEEPROM((unsigned char*)&_version, 1, _addr + 4);
      litEEPROM((unsigned char*)&_taille, 2, _addr + 2);
      if ( (uint16_t)(_addr + _taille) > EEPROM.length() ) {
        _mode = modeBlocMem_Erreur;
        return false;
      }
    }
    else if ( mode == modeBlocMem_LectureEcriture) { // mode lecture-ecriture
      litEEPROM((unsigned char*)&_version, 1, _addr + 4);
      litEEPROM((unsigned char*)&_taille, 2, _addr + 2);
      if ( CRCValide() ) {
        if ( _version != version) _mode = modeBlocMem_Lecture;
      } else {
        _taille = TAILLE_ENTETE_BLOCMEM;
        _version = version;
      }
    }
  }
  return true;
}

void BlocMem::ecritVersion(uint8_t version){
  ecritEEPROM((unsigned char*)&version, 1, _addr + 4);
  _version=version;
}
uint8_t BlocMem::litVersion() const{
  return lit8EEPROM(_addr + 4);
}

BlocMem::~BlocMem() {
  if ( _mode == modeBlocMem_Ecriture || _mode == modeBlocMem_LectureEcriture) actualiseEntete();
}

bool BlocMem::reserve(uint16_t taille) {
  if ( _mode == modeBlocMem_Erreur || _mode == modeBlocMem_Close) return false;
  if ( _curseur + taille > EEPROM.length() ) return false;
  _curseur += taille;
  if ( _mode == modeBlocMem_Ecriture) _taille += taille;
  return true;
}

//bool BlocMem::ecrit(unsigned char *objet, uint16_t taille) {
//  if ( (_mode != modeBlocMem_Ecriture && _mode != modeBlocMem_LectureEcriture) || _curseur + taille > EEPROM.length() || (_tailleMax != 0 && (_curseur - _addr - TAILLE_ENTETE_BLOCMEM + taille > _tailleMax)) ) return false;
//  _curseur = ecritEEPROM(objet, taille, _curseur);
//  _taille += taille;
//  return true;
//}

bool BlocMem::ecrit(unsigned char *objet, uint16_t taille, uint16_t position) {
#ifdef DEBUG
  Serial.print("BlocMem::ecrit(objet@");
  Serial.print(uint16_t(objet), HEX);
  Serial.print(", taille=");
  Serial.print(taille);
  Serial.print(", position=");
  Serial.print(position);
  Serial.println(')');
#endif
  if ( (_mode != modeBlocMem_Ecriture && _mode != modeBlocMem_LectureEcriture) || (_mode == modeBlocMem_Ecriture && position != 0xFFFF)) return false;

  bool ecritureCurseur = (position == 0xFFFF);
  position = ((ecritureCurseur) ? _curseur : (position + _addr + TAILLE_ENTETE_BLOCMEM));
  if ( position == _curseur && position - _addr - TAILLE_ENTETE_BLOCMEM == _taille) ecritureCurseur = true;

  if (( !ecritureCurseur && position - _addr - TAILLE_ENTETE_BLOCMEM + taille > _taille ) || (position + taille > EEPROM.length()) || (_tailleMax != 0 && (position - _addr - TAILLE_ENTETE_BLOCMEM + taille > _tailleMax)) ) return false;

  position = ecritEEPROM(objet, taille, position);
  if (ecritureCurseur) {
    if (position - _addr - TAILLE_ENTETE_BLOCMEM + taille > _taille) _taille += taille;
    _curseur = position;
  }
  return true;
}

//bool BlocMem::lit(unsigned char *objet, uint16_t taille) {
//  if ( (_mode != modeBlocMem_Lecture && _mode != modeBlocMem_LectureEcriture) || _curseur - _addr - TAILLE_ENTETE_BLOCMEM + taille > _taille) return false;
//  _curseur = litEEPROM(objet, taille, _curseur);
//  return true;
//}

bool BlocMem::lit(unsigned char* objet, uint16_t taille, uint16_t position) {
#ifdef DEBUG
  Serial.print("BlocMem::lit(objet@");
  Serial.print(uint16_t(objet), HEX);
  Serial.print(", taille=");
  Serial.print(taille);
  Serial.print(", position=");
  Serial.print(position);
  Serial.println(')');
  Serial.print("->   ");
  Serial.print("_taille=");
  Serial.print(_taille);
  Serial.print(" _addr=");
  Serial.print(_addr);
  Serial.print(" _mode=");
  Serial.print((char)_mode);
  Serial.print(" _curseur=");
  Serial.print(_curseur);
  Serial.print(" _tailleMax=");
  Serial.println(_tailleMax);
#endif
  if ( (_mode != modeBlocMem_Lecture && _mode != modeBlocMem_LectureEcriture) || (_mode == modeBlocMem_Lecture && position != 0xFFFF)) return false;

  bool lectureCurseur = (position == 0xFFFF);
  position = ((lectureCurseur) ? _curseur : (position + _addr + TAILLE_ENTETE_BLOCMEM));

  if ( position - _addr - TAILLE_ENTETE_BLOCMEM + taille > _taille) return false;

  position = litEEPROM(objet, taille, position);
  if (lectureCurseur) _curseur = position;

  return true;
}

//bool BlocMem::ecrit(uint8_t data) {
//  return ecrit((unsigned char*)&data, 1);
//}
//bool BlocMem::ecrit(uint16_t data) {
//  return ecrit((unsigned char*)&data, 2);
//}
bool BlocMem::ecrit(uint8_t data, uint16_t position) {
  return ecrit((unsigned char*)&data, 1, position);
}
bool BlocMem::ecrit(uint16_t data, uint16_t position) {
  return ecrit((unsigned char*)&data, 2, position);
}
bool BlocMem::ecrit(uint32_t data, uint16_t position) {
  return ecrit((unsigned char*)&data, 4, position);
}
//uint8_t BlocMem::lit8() {
//  uint8_t data = 0;
//  lit((unsigned char*)&data, 1);
//  return data;
//}
uint8_t BlocMem::lit8(uint16_t position) {
  uint8_t data = 0;
  lit((unsigned char*)&data, 1, position);
  return data;
}
//uint16_t BlocMem::lit16() {
//  uint16_t data = 0;
//  lit((unsigned char*)&data, 2);
//  return data;
//}

uint16_t BlocMem::lit16(uint16_t position) {
  uint16_t data = 0;
  lit((unsigned char*)&data, 2, position);
  return data;
}
uint32_t BlocMem::lit32(uint16_t position) {
  uint32_t data = 0;
  lit((unsigned char*)&data, 4, position);
  return data;
}

bool BlocMem::CRCValide() {
  if ( _mode == modeBlocMem_Erreur || _mode == modeBlocMem_Close) return false;
  uint16_t crc;
  litEEPROM((unsigned char*)&crc, 2, _addr);
  if ( _addr + _taille >= EEPROM.length()) return false;
  return (crc == crc16Memoire(_addr + 2, _taille - 2));
}

uint16_t BlocMem::CRC16lue(void) {
  if ( _mode == modeBlocMem_Erreur || _mode == modeBlocMem_Close) return 0;
  uint16_t crc;
  litEEPROM((unsigned char*)&crc, 2, _addr);
  return crc;
}

bool BlocMem::changeMode(modeBlocMem mode) {
  if ( _mode == modeBlocMem_Erreur || _mode == modeBlocMem_Close) return false;
  if ( _mode != mode) {
    if (_mode == modeBlocMem_Ecriture || _mode == modeBlocMem_LectureEcriture) actualiseEntete();
    _mode = mode;
  }
  return true;
}

uint16_t BlocMem::CRC16(void) {
  if ( _mode == modeBlocMem_Erreur || _mode == modeBlocMem_Close) return 0;
  return crc16Memoire(_addr + 2, _taille - 2);
}
void BlocMem::actualiseEntete(void) {
  if ( _mode != modeBlocMem_Ecriture && _mode != modeBlocMem_LectureEcriture) return;
  ecritEEPROM((unsigned char*)&_taille, 2, _addr + 2);
  ecritEEPROM((unsigned char*)&_version, 1, _addr + 4);
  uint16_t crc = crc16Memoire(_addr + 2, _taille - 2);
  ecritEEPROM((unsigned char*)&crc, 2, _addr);
}

bool BlocMem::curseur(uint16_t pos) {
  if (_mode != modeBlocMem_LectureEcriture || pos > _taille - TAILLE_ENTETE_BLOCMEM) return false;
  _curseur = _addr + TAILLE_ENTETE_BLOCMEM + pos;
  return true;
}


void BlocMem::close() {
  if ( _mode == modeBlocMem_Ecriture || _mode == modeBlocMem_LectureEcriture) actualiseEntete();
  _mode = modeBlocMem_Close;
  _taille = 0XFFFF;
  _addr = 0XFFFF;
  _curseur = 0XFFFF;
}
