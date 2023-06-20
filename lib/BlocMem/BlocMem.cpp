#include "BlocMem.h"

BlocMem _bloc;

bool saveDataBlocMem(uint16_t addr, unsigned char* objet, size_t taille, uint16_t pos, uint8_t version, size_t tailleMax) {
  _bloc = BlocMem(addr, version, modeBlocMem_LectureEcriture, tailleMax);
  if ( _bloc.mode() != modeBlocMem_Erreur && _bloc.setPosCurseur(pos) && _bloc.ecrit(objet, taille)) {
    _bloc.close();
    return true;
  }
  _bloc.close();
  return false;
}

BlocMem::BlocMem(uint16_t addr, uint8_t version, modeBlocMem mode, size_t tailleMax) {
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

bool BlocMem::init(uint16_t addr, uint8_t version, modeBlocMem mode, size_t tailleMax) {
  if ( _mode != modeBlocMem_Close) return false; // déjà initialisé

  if ( addr + TAILLE_ENTETE_BLOCMEM >= TAILLE_EEPROM) {
    _mode = modeBlocMem_Erreur;
    return false;
  }
  else {
    _addr = addr;
    _mode = mode;
    _curseur = 0;
    if ( tailleMax > TAILLE_EEPROM - _addr) tailleMax = TAILLE_EEPROM - addr - TAILLE_ENTETE_BLOCMEM;
    _tailleMax = tailleMax;

    if ( mode == modeBlocMem_Ecriture) { // mode ecriture
      _version = version;
      _taille = 0;
    }
    else if ( mode == modeBlocMem_Lecture) { // mode lecture
      // litEEPROM((unsigned char*)&_version, 1, _addr + 4);
      // litEEPROM((unsigned char*)&_taille, 2, _addr + 2);
      _version=eeprom_read_byte((uint8_t*)(_addr + BLOCMEM_ENTETE_POS_VERSION));
      _taille=eeprom_read_word((uint16_t*)(_addr + BLOCMEM_ENTETE_POS_TAILLE));
      if ( (uint16_t)(_addr + _taille + TAILLE_ENTETE_BLOCMEM) > TAILLE_EEPROM ) {
        _mode = modeBlocMem_Erreur;
        return false;
      }
    }
    else if ( mode == modeBlocMem_LectureEcriture) { // mode lecture-ecriture
      //litEEPROM((unsigned char*)&_version, 1, _addr + 4);
      //litEEPROM((unsigned char*)&_taille, 2, _addr + 2);
      _version=eeprom_read_byte((uint8_t*)(_addr + BLOCMEM_ENTETE_POS_VERSION));
      _taille=eeprom_read_word((uint16_t*)(_addr + BLOCMEM_ENTETE_POS_TAILLE));
      if ( CRCValide() ) {
        if ( _version != version) _mode = modeBlocMem_Lecture;
      } else {
        _taille = 0;
        _version = version;
      }
    }
  }
  return true;
}

void BlocMem::ecritVersion(uint8_t version){
  //ecritEEPROM((unsigned char*)&version, 1, _addr + 4);
  eeprom_update_byte((uint8_t*)(_addr + BLOCMEM_ENTETE_POS_VERSION),version);
  _version=version;
}
uint8_t BlocMem::litVersion() const{
  return eeprom_read_byte((uint8_t*)(_addr + BLOCMEM_ENTETE_POS_VERSION));
  // return lit8EEPROM(_addr + 4);
}

BlocMem::~BlocMem() {
  if ( _mode == modeBlocMem_Ecriture || _mode == modeBlocMem_LectureEcriture) actualiseEntete();
}

bool BlocMem::reserve(size_t taille) {
  if ( _mode == modeBlocMem_Erreur || _mode == modeBlocMem_Close) return false;
  if ( _addr + _curseur + taille + TAILLE_ENTETE_BLOCMEM > TAILLE_EEPROM ) return false;
  _curseur += taille;
  if ( _mode == modeBlocMem_Ecriture) _taille += taille;
  return true;
}

//bool BlocMem::ecrit(unsigned char *objet, size_t taille) {
//  if ( (_mode != modeBlocMem_Ecriture && _mode != modeBlocMem_LectureEcriture) || _curseur + taille > TAILLE_EEPROM || (_tailleMax != 0 && (_curseur - _addr - TAILLE_ENTETE_BLOCMEM + taille > _tailleMax)) ) return false;
//  _curseur = ecritEEPROM(objet, taille, _curseur);
//  _taille += taille;
//  return true;
//}

bool BlocMem::ecrit(void* objet, size_t taille, uint16_t position) {
#ifdef DEBUG
  Serial.print(F("BlocMem::ecrit(objet@"));
  Serial.print(uint16_t(objet), HEX);
  Serial.print(F(", taille="));
  Serial.print(taille);
  Serial.print(F(", position="));
  Serial.print(position);
  Serial.print(')');
  Serial.print(F("=>EEPROM@"));
  Serial.println(((position == 0xFFFF) ? _curseur : (position + _addr + TAILLE_ENTETE_BLOCMEM)));
#endif
  if ( (_mode != modeBlocMem_Ecriture && _mode != modeBlocMem_LectureEcriture) || (_mode == modeBlocMem_Ecriture && position != 0xFFFF)) return false;

  bool ecritureCurseur = (position == 0xFFFF);
  position = ((ecritureCurseur) ? _curseur : position );
  if ( position == _curseur && position == this->_taille) ecritureCurseur = true;

  if (
    ( !ecritureCurseur && position + taille > this->_taille ) 
    || ( position + taille + this->_addr + TAILLE_ENTETE_BLOCMEM > TAILLE_EEPROM) 
    || (_tailleMax != 0 && ( position + taille > _tailleMax ) )
      ) return false;

  // ecritEEPROM(objet, taille, position);
  eeprom_update_block(objet,(void*)( position +this->_addr + TAILLE_ENTETE_BLOCMEM ) ,taille);

  if (ecritureCurseur) {
    if (position + taille > this->_taille) this->_taille = position + taille;
    _curseur = position + taille;
  }
  return true;
}

bool BlocMem::ecritPGM(const char* chaine, uint16_t position){
  if ( (_mode != modeBlocMem_Ecriture && _mode != modeBlocMem_LectureEcriture) || (_mode == modeBlocMem_Ecriture && position != 0xFFFF)) return false;

  size_t taille=0;
  while( pgm_read_byte( (uint16_t)chaine + taille) != '\0' ){ taille++; }
  taille+=1;

  bool ecritureCurseur = (position == 0xFFFF);
  position = ((ecritureCurseur) ? _curseur : position );
  if ( position == _curseur && position == this->_taille) ecritureCurseur = true;

  if (
    ( !ecritureCurseur && position - _addr - TAILLE_ENTETE_BLOCMEM + taille > _taille )
    || ( position + taille +this->_addr + TAILLE_ENTETE_BLOCMEM > TAILLE_EEPROM)
    || (_tailleMax != 0 && ( position + taille > _tailleMax ) ) 
      ) return false;

  uint16_t pos=0;
  uint8_t c;
  uint16_t posEEPROM=position + this->_addr + TAILLE_ENTETE_BLOCMEM;
  do{
    c=pgm_read_byte( (uint16_t)chaine + pos);
    //ecritEEPROM(&c,1,position + pos);
    eeprom_update_byte( (uint8_t*)(posEEPROM + pos), (uint8_t)c);
    
    pos++;
  } while( c != '\0');

  if (ecritureCurseur) {
    if (position + taille > this->_taille) this->_taille = position + taille;
    _curseur = position + taille;
  }
  return true;
}

int8_t BlocMem::strcmp_P(const char* chainePGM, uint16_t position){
  if ( (_mode != modeBlocMem_Lecture && _mode != modeBlocMem_LectureEcriture) || (_mode == modeBlocMem_Lecture && position != 0xFFFF)) return false;

  bool lectureCurseur = (position == 0xFFFF);
  position = ((lectureCurseur) ? _curseur : position );

  char p, e;
  uint16_t pos=0;
  uint16_t posEEPROM=position + this->_addr + TAILLE_ENTETE_BLOCMEM;
  do{
    p=pgm_read_byte( (uint16_t)chainePGM + pos);
    //litEEPROM((byte*)&e, 1, position + pos);
    e=(char)eeprom_read_byte((uint8_t*) ( posEEPROM + pos ));
    pos++;
  } while(p == e && p != '\0' && e != '\0' && ( position + pos ) < this->_taille );

  if (lectureCurseur) {
    while( e != '\0' && ( position + pos ) < this->_taille ){
      e=(char)eeprom_read_byte((uint8_t*) ( posEEPROM + pos ));
      pos++;
    }
    this->_curseur = position + pos; // actualisation de la position du curseur
  }

  if( p == '\0' && e == '\0' ) return 0; // on est arrivé à la fin des 2 chaines et elle sont égales
  if( p < e ) return -1;  // PROGMEM <
  if( p > e ) return 1; // PROGMEM >
  
  return 1; // on est arrivé à la fin du BlocMem mais pas à la fin des 2 chaines donc la chaine eeprom sera interrompu ici et donc sera plus courtes => PROGMEM >
}

//bool BlocMem::lit(unsigned char *objet, size_t taille) {
//  if ( (_mode != modeBlocMem_Lecture && _mode != modeBlocMem_LectureEcriture) || _curseur - _addr - TAILLE_ENTETE_BLOCMEM + taille > _taille) return false;
//  _curseur = litEEPROM(objet, taille, _curseur);
//  return true;
//}

bool BlocMem::lit(void* objet, size_t taille, uint16_t position) {
#ifdef DEBUG
  Serial.print(F("BlocMem::lit(objet@"));
  Serial.print(uint16_t(objet), HEX);
  Serial.print(F(", taille="));
  Serial.print(taille);
  Serial.print(F(", position="));
  Serial.print(position);
  Serial.println(')');
  Serial.print(F("->   "));
  Serial.print(F("_taille="));
  Serial.print(_taille);
  Serial.print(F(" _addr="));
  Serial.print(_addr);
  Serial.print(F(" _mode="));
  Serial.print((char)_mode);
  Serial.print(F(" _curseur="));
  Serial.print(_curseur);
  Serial.print(F(" _tailleMax="));
  Serial.print(_tailleMax);
  Serial.print(F(" from EEPROM@"));
  Serial.println( ((position == 0xFFFF) ? _curseur : (position + _addr + TAILLE_ENTETE_BLOCMEM)));
#endif
  if ( (_mode != modeBlocMem_Lecture && _mode != modeBlocMem_LectureEcriture) || (_mode == modeBlocMem_Lecture && position != 0xFFFF)) return false;

  if ( ((position == 0xFFFF) ? this->_curseur : position) + taille > _taille) return false;

  // position = litEEPROM(objet, taille, position);
  eeprom_read_block(objet,(void*)(((position == 0xFFFF) ? this->_curseur : position) + this->_addr + TAILLE_ENTETE_BLOCMEM), taille);
  if (position == 0xFFFF ) this->_curseur += taille;

  return true;
}

//bool BlocMem::ecrit(uint8_t data) {
//  return ecrit((unsigned char*)&data, 1);
//}
//bool BlocMem::ecrit(uint16_t data) {
//  return ecrit((unsigned char*)&data, 2);
//}
bool BlocMem::ecrit(uint8_t data, uint16_t position) {
  return ecrit(&data, 1, position);
}
bool BlocMem::ecrit(uint16_t data, uint16_t position) {
  return ecrit(&data, 2, position);
}
bool BlocMem::ecrit(uint32_t data, uint16_t position) {
  return ecrit(&data, 4, position);
}
//uint8_t BlocMem::lit8() {
//  uint8_t data = 0;
//  lit((unsigned char*)&data, 1);
//  return data;
//}
uint8_t BlocMem::lit8(uint16_t position) {
  uint8_t data = 0;
  lit(&data, 1, position);
  return data;
}
//uint16_t BlocMem::lit16() {
//  uint16_t data = 0;
//  lit((unsigned char*)&data, 2);
//  return data;
//}

uint16_t BlocMem::lit16(uint16_t position) {
  uint16_t data = 0;
  lit(&data, 2, position);
  return data;
}
uint32_t BlocMem::lit32(uint16_t position) {
  uint32_t data = 0;
  lit(&data, 4, position);
  return data;
}

bool BlocMem::CRCValide() {
  if ( _mode == modeBlocMem_Erreur || _mode == modeBlocMem_Close) return false;
  uint16_t crc = eeprom_read_word((uint16_t*) ( _addr + BLOCMEM_ENTETE_POS_CRC ) );
  // litEEPROM((unsigned char*)&crc, 2, _addr);
  if ( _addr + _taille >= TAILLE_EEPROM) return false;
  return (crc == this->CRC16());
}

uint16_t BlocMem::CRC16lue(void) {
  if ( _mode == modeBlocMem_Erreur || _mode == modeBlocMem_Close) return 0;
  // uint16_t crc;
  /// litEEPROM((unsigned char*)&crc, 2, _addr);
  return eeprom_read_word((uint16_t*) ( _addr + BLOCMEM_ENTETE_POS_CRC ) );
}

bool BlocMem::changeMode(modeBlocMem mode) {
  if ( _mode == modeBlocMem_Erreur || _mode == modeBlocMem_Close) return false;
  if ( _mode != mode) {
    if (_mode == modeBlocMem_Ecriture || _mode == modeBlocMem_LectureEcriture) actualiseEntete();
    _mode = mode;
  }
  return true;
}

uint16_t BlocMem::CRC16() {
  if ( _mode == modeBlocMem_Erreur || _mode == modeBlocMem_Close) return 0;


  uint16_t crc = 0xFFFF;
  for (uint16_t i = _addr + BLOCMEM_ENTETE_TAILLE_CRC; i < _addr + TAILLE_ENTETE_BLOCMEM + _taille ; ++i) {
    crc = crc ^ eeprom_read_byte((uint8_t*)i);
    for (uint8_t j = 0; j < 8; ++j) {
      if ( (crc & 1) == 1) crc = (crc / 2) ^ 0xA001;
      else crc = crc >> 1;
    }
  }
  return crc;


  // return crc16Memoire(_addr + 2, _taille - 2);
}
void BlocMem::actualiseEntete(void) {
  if ( _mode != modeBlocMem_Ecriture && _mode != modeBlocMem_LectureEcriture) return;
  
  // ecritEEPROM((unsigned char*)&_taille, 2, _addr + 2);
  eeprom_update_word((uint16_t*)( _addr + BLOCMEM_ENTETE_POS_TAILLE), _taille);
  // ecritEEPROM((unsigned char*)&_version, 1, _addr + 4);
  eeprom_update_byte((uint8_t*)( _addr + BLOCMEM_ENTETE_POS_VERSION), _version);
  uint16_t crc = this->CRC16();

  // ecritEEPROM((unsigned char*)&crc, 2, _addr);
  eeprom_update_word((uint16_t*)( _addr + BLOCMEM_ENTETE_POS_CRC), crc);
}

bool BlocMem::setPosCurseur(uint16_t pos) {
  if (_mode != modeBlocMem_LectureEcriture || pos > _taille ) return false;
  _curseur = pos;
  return true;
}


void BlocMem::close() {
  if ( _mode == modeBlocMem_Ecriture || _mode == modeBlocMem_LectureEcriture) actualiseEntete();
  _mode = modeBlocMem_Close;
  _taille = 0XFFFF;
  _addr = 0XFFFF;
  _curseur = 0XFFFF;
}
