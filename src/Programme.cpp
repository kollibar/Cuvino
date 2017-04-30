#include "Programme.h"

// -- plageHC_HP -- //

uint16_t plageHC_HP::save(uint16_t addr) const{
  addr = ecritEEPROM((uint8_t*)&this->nb_plages, 1, addr);
  addr = ecritEEPROM((uint8_t*)&this->horaires, 6, addr);
  return addr;
}

bool plageHC_HP::save(BlocMem* bloc) const{
  if ( ! bloc->ecrit((uint8_t*)&this->nb_plages, 1)) return false;
  if ( ! bloc->ecrit((uint8_t*)&this->horaires, 6)) return false;
  return true;
}

bool plageHC_HP::load(BlocMem* bloc) {
  if ( ! bloc->lit((uint8_t*)&this->nb_plages, 1)) return false;
  if ( ! bloc->lit((uint8_t*)&this->horaires, 6)) return false;
  return true;
}

uint16_t plageHC_HP::load(uint16_t addr) {
  this->defaut();
  addr = litEEPROM((uint8_t*)&this->nb_plages, 1, addr);
  addr = litEEPROM((uint8_t*)&this->horaires, 6, addr);
  return addr;
}

void plageHC_HP::defaut(void) {
  this->nb_plages = 0;
}

plageHC_HP plageHC_HP::operator=(plageHC_HP const &plage) {
  plageHC_HP p;
  p.nb_plages = plage.nb_plages;
  for(uint8_t i=0;i<6;++i) p.horaires[i] = plage.horaires[i];
  return p;
}

Programme::Programme() {
  defaut();
}

bool Programme::estRepetition() const {
  if ( date == 0) return false;
  return (bool)((date & 0x30000000) == 0x30000000);
}

bool Programme::setDate(RtcDateTime &date){
  this->date=date;
  return (bool)((date & 0x30000000) == 0);
}

uint32_t Programme::getDate() const{
  if( this->estRepetition() ){
    return (this->date & 0x3FFFFF);
  } else {
    return this->date;
  }
}

RtcDateTime Programme::getRtcDateTime() const{
  return RtcDateTime(this->getDate());
}

bool Programme::setCuve(unsigned char cuve, int temp){
  this->cuve=cuve;
  this->temp=temp;
  return true;
}

bool Programme::set(RtcDateTime &date, unsigned char cuve, int temp){
  this->date=date;
  this->cuve=cuve;
  this->temp=temp;
  return (bool)((date & 0x30000000) == 0);
}

bool Programme::set(uint8_t modeRepetition, unsigned char cuve, int temp){
  return this->set(modeRepetition,1,0,cuve,temp);
}

bool Programme::set(uint8_t modeRepetition, uint8_t frequenceRepetition, unsigned char cuve, int temp){
  return this->set(modeRepetition,frequenceRepetition,0,cuve,temp);
}

bool Programme::set(uint8_t modeRepetition, uint8_t frequenceRepetition, uint32_t date, unsigned char cuve, int temp){
  if( modeRepetition==MODE_PROG_NUL || modeRepetition>0b111 || frequenceRepetition > 0b11111) return false;
  this->cuve=cuve;
  this->temp=temp;
  this->date=((uint32_t)modeRepetition&0b1)<<27
      | ((uint32_t)modeRepetition&0b110)<<29
      | ((uint32_t)frequenceRepetition&0b11111)<<22
      | (date&0x3FFFFF)
      | 0x30000000;
  return true;
}

bool Programme::setRepetition(uint8_t modeRepetition){
  return setRepetition(modeRepetition,1, 0);
}
bool Programme::setRepetition(uint8_t modeRepetition, uint8_t frequenceRepetition){
  return setRepetition(modeRepetition,frequenceRepetition, 0);
}

bool Programme::setRepetition(uint8_t modeRepetition, uint8_t frequenceRepetition, uint32_t date){
  if( modeRepetition==MODE_PROG_NUL || modeRepetition>0b111 || frequenceRepetition > 0b11111) return false;
  this->date=((uint32_t)modeRepetition&0b1)<<27
      | ((uint32_t)modeRepetition&0b110)<<29
      | ((uint32_t)frequenceRepetition)<<22
      | (date&0x3FFFFF)
      | 0x30000000;
  return true;
}

unsigned long Programme::frequence() const{
  switch(mode()){
    case MODE_PROG_NUL:
      return 0;
    case MODE_PROG_HC_HP:
      return ((date >> 22) & 0b11111)*60*60*24;
    case MODE_PROG_10M:
      return ((date >> 22) & 0b11111)*60*10;
    case MODE_PROG_H:
      return ((date >> 22) & 0b11111)*60*60;
    case MODE_PROG_J:
      return ((date >> 22) & 0b11111)*60*60*24;
    case MODE_PROG_S:
      return ((date >> 22) & 0b11111)*60*60*24*7;
    case MODE_PROG_M:
      return ((date >> 22) & 0b11111)*60*60*24*30;
  }
  return 0;
}

unsigned long Programme::decalage() const{
  switch(mode()){
    case MODE_PROG_NUL:
      return this->date;
    case MODE_PROG_HC_HP:
      return 0x7FFFFFFF;
    case MODE_PROG_HP_HC:
      return 0xFFFFFFFF;
    case MODE_PROG_S:
      return (((this->date & 0x3FFFFF)+2*86400)%(7*86400));
    case MODE_PROG_H:
      return (this->date & 0x3FFFFF);
    case MODE_PROG_M:
      return (this->date & 0x3FFFFF);
    case MODE_PROG_10M:
      return (this->date & 0x3FFFFF);
    case MODE_PROG_J:
      return (this->date & 0x3FFFFF);
  }
}

uint32_t Programme::precedent(RtcDateTime &now, plageHC_HP& HC_HP) {
  return prochain(now, HC_HP, true);
}

uint32_t Programme::prochain(RtcDateTime &now, plageHC_HP& HC_HP, bool precedent) {
  RtcDateTime dateTime;
  if (this->estRepetition()) { // repetition
    switch (mode()) {
      /*
      case MODE_PROG_10M:
        return (((now.TotalSeconds()-1) / (600 * freqRepetition())) + ((precedent) ? 0 : freqRepetition())) * (600 * freqRepetition());
      case MODE_PROG_H:
        return (((now.TotalSeconds()-1) / (3600 * freqRepetition())) + ((precedent) ? 0 : freqRepetition())) * (3600 * freqRepetition());
      case MODE_PROG_J:
        //return ((now.TotalSeconds() - (date % 86400)) / (86400 * freqRepetition())) * (86400 * freqRepetition()) + (date % 86400);
        return (((now.TotalSeconds() - getDate() - 1) / (86400 * freqRepetition())) + ((precedent) ? 0 : freqRepetition())) * (86400 * freqRepetition()) + getDate();
        */
      case MODE_PROG_10M:
      case MODE_PROG_H:
      case MODE_PROG_J:
      case MODE_PROG_S:
        return (((now.TotalSeconds()-1) / this->frequence()) + ((precedent) ? 0 : 1)) * this->frequence() + this->decalage();
      case MODE_PROG_M: // ajouter précédent
        dateTime = RtcDateTime(this->decalage());
        dateTime = RtcDateTime(now.Year(), now.Month(), dateTime.Day(), dateTime.Hour(), dateTime.Minute(), dateTime.Second());
        if( dateTime < now){
          if( precedent ) return dateTime.TotalSeconds();
          else {
            dateTime = RtcDateTime(now.Year(), now.Month() + 1, dateTime.Day(), dateTime.Hour(), dateTime.Minute(), dateTime.Second());
            return dateTime;
          }
        } else{
          if( precedent ){
            dateTime = RtcDateTime(now.Year(), now.Month() - 1, dateTime.Day(), dateTime.Hour(), dateTime.Minute(), dateTime.Second());
            return dateTime;
          } else return dateTime.TotalSeconds();
        }
        //return DaysSinceFirstOfYear2000<uint32_t>((uint16_t)now.Year(), (uint8_t)(now.Month() + (( ((now.Day() * 24 + now.Hour() * 60) + now.Minute()) * 60 + now.Second() <  date ) ? 1 : 0)), (uint8_t)1) * 86400 + date;
      /*
      case MODE_PROG_S: // ajouter précédent
        if (now.DayOfWeek() == (date / 86400) % 7 && (now.TotalSeconds() % 86400 < date % 86400)) {
          return (now.TotalSeconds() / 86400) * 86400 + date % 86400;
        } else {
          return ((now.TotalSeconds() / 86400) + (((date / 86400) % 7 + 7) - now.DayOfWeek()) % 7) * 86400 + date % 86400;
        }
        */
      case MODE_PROG_HC_HP:
      case MODE_PROG_HP_HC:
        if ( HC_HP.nb_plages == 0) return 0;
        uint8_t tmp = now.Hour() * 2 + ((now.Minute()*60+now.Second()) <= (30*60)) ? 0 : 1;
        int8_t i = (mode() == MODE_PROG_HC_HP ? 1 : 0);
        while ( tmp < HC_HP.horaires[i]) {
        i += 2;
        if ( i >= HC_HP.nb_plages) {
            i = i % 2;
            break;
          }
        }
        if(precedent) {
          i-=2;
          if(i<0) i+=6;
        }

        return ((now.TotalSeconds() / 86400) + (HC_HP.horaires[i] <= tmp) ? 1 : 0) * 86400 + 1800 * HC_HP.horaires[i];

    }
  } else { // non repetition
    if ( (! precedent && this->date > now.TotalSeconds()) // si on cherche dans le futur et que date de la répétition > now (càd date répétition est dans le futur)
      || (precedent && this->date < now.TotalSeconds())) { // et si on recherche dans le passé et que la date de la répétition < now (càd date répétition est dans le passé)
        return this->date; // => renvoi la date du programme
      }
    return 0; // si déja passé (ou, si recherche dans le passé, si pas encore passé) renvoi 0
  }
  return 0;
}


//bool Programme::save(BlocMem* bloc) {
//  if ( ! bloc->ecrit((uint8_t*)&this->date, 4)) return false;
//  if ( ! bloc->ecrit((uint8_t*)&this->cuve, 1)) return false;
//  if ( ! bloc->ecrit((uint8_t*)&this->temp, 2)) return false;
//  if ( ! bloc->ecrit((uint8_t*)&this->repetition, 2)) return false;
//  if ( ! bloc->reserve(3)) return false;
//  return true;
//}

//bool Programme::load(BlocMem* bloc) {
//  if ( ! bloc->lit((uint8_t*)&this->date, 4)) return false;
//  if ( ! bloc->lit((uint8_t*)&this->cuve, 1)) return false;
//  if ( ! bloc->lit((uint8_t*)&this->temp, 2)) return false;
//  if ( ! bloc->lit((uint8_t*)&this->repetition, 2)) return false;
//  if ( ! bloc->reserve(3)) return false;
//  return true;
//}

//void Programme::defaut(void) {
//  this->date = 0;
//}

Programme Programme::operator=(Programme const &programme) {
  Programme r;
  r.date = programme.date;
  r.cuve = programme.cuve;
  r.alarme = programme.alarme;
  r.temp = programme.temp;
  return r;
}

uint8_t Programme::freqRepetition() const{
  if( estRepetition()) return (uint8_t)((date >> 22) & 0b11111) ;
  else return 0;
}

uint8_t Programme::mode(void) const{
  if( estRepetition() ) return (
        (uint8_t)((date >> 27) & 0b001)
        | (uint8_t)((date >> 29) & 0b110)
      );
  return MODE_PROG_NUL;
}

/* structure d'un objet Programme lors de l'enregistrement (taille totale 10 octets)
  uint32_t date   4 octets
  uint8_t cuve    1 octet
  uint16_t temp   2 octets
  reserve         3 octets
*/

uint16_t Programme::save(uint16_t addr) const{
  addr = ecritEEPROM((uint8_t*)&this->date, 4, addr);
  addr = ecritEEPROM((uint8_t*)&this->cuve, 1, addr);
  addr = ecritEEPROM((uint8_t*)&this->temp, 2, addr);
  addr = reserveEEPROM(3,addr);
  return addr;
}
uint16_t Programme::load(uint16_t addr){
  addr = litEEPROM((uint8_t*)&this->date, 4, addr);
  addr = litEEPROM((uint8_t*)&this->cuve, 1, addr);
  addr = litEEPROM((uint8_t*)&this->temp, 2, addr);
  addr+=3; // reserve 3 octets
  return addr;
}

void Programme::defaut(void){
  date = 0;
  cuve = 0xFF;
  alarme = 0xFF;
  temp = TEMP_ARRET;
}

size_t Programme::print(DebugLogger& debug){
  size_t s;
  s=debug.printPGM(FR::TXT_DATE);
  s+=debug.print(':');
  s+=debug.print(this->date,HEX);
  s+=debug.print(' ');
  s+=debug.printPGM(FR::TXT_TEMP);
  s+=debug.print(':');
  s+=debug.print(this->temp,HEX);
  s+=debug.print(' ');
  s+=debug.printPGM(FR::TXT_CUVE);
  s+=debug.print(':');
  s+=debug.print(this->cuve);
  s+=debug.print(" estRepetion:");
  s+=debug.print(this->estRepetition());
  s+=debug.print(" freqRepetition:");
  s+=debug.print(this->freqRepetition());
  s+=debug.print(' ');
  s+=debug.printPGM(FR::TXT_MODE);
  s+=debug.print(':');
  s+=debug.print(this->mode());
  s+=debug.print(" getDate:");
  s+=debug.print(this->getDate());
  return s;
}

size_t Programme::println(DebugLogger& debug){
  size_t s=print(debug);
  s+=debug.println();
  return s;
}


size_t Programme::print(HardwareSerial& serial){
  size_t s=serial.print("date:");
  s+=serial.print(this->date,HEX);
  s+=serial.print(" temp:");
  s+=serial.print(this->temp,HEX);
  s+=serial.print(" cuve:");
  s+=serial.print(this->cuve);
  s+=serial.print(" estRepetion:");
  s+=serial.print(this->estRepetition());
  s+=serial.print(" freqRepetition:");
  s+=serial.print(this->freqRepetition());
  s+=serial.print(" mode:");
  s+=serial.print(this->mode());
  s+=serial.print(" getDate:");
  s+=serial.print(this->getDate());
  return s;
}

size_t Programme::println(HardwareSerial& serial){
  size_t s=print(serial);
  s+=serial.println();
  return s;
}

BlocMemProgramme::BlocMemProgramme() : BlocMem(0, 1, modeBlocMem_Close) {
  _nb=0; _nbOK=0;
}

BlocMemProgramme::BlocMemProgramme(uint16_t addr) : BlocMem(addr, 1, modeBlocMem_LectureEcriture) {
#ifdef DEBUG
  Serial.print("BlocMemProgramme(");
  Serial.print(addr);
  Serial.println(")");
  Serial.print("mode:");
  if( _mode==modeBlocMem_LectureEcriture) Serial.println("modeBlocMem_LectureEcriture");
  else if( _mode==modeBlocMem_Lecture) Serial.println("modeBlocMem_Lecture");
  else if( _mode==modeBlocMem_Ecriture) Serial.println("modeBlocMem_Ecriture");
  else if( _mode==modeBlocMem_Erreur) Serial.println("modeBlocMem_Erreur");
  else if( _mode==modeBlocMem_Close) Serial.println("modeBlocMem_Close");
#endif
  _numCharge = 0xFF; // aucun programme chargé

  if ( _mode == modeBlocMem_Lecture) { // problème de version
    if( litVersion() != 1 ){ // version différente de 1
      ecritVersion(1); // on met la version à 1
      changeMode(modeBlocMem_LectureEcriture);
    }
  }

  if ( _mode == modeBlocMem_LectureEcriture ) {
    if( _taille < TAILLE_ENTETE_BLOCMEM + TAILLE_DATA_ENTETE_BLOCMEMPROGRAMME ){
      #ifdef DEBUG
      Serial.print("reaffectation taille => ");
      Serial.println(TAILLE_ENTETE_BLOCMEM + TAILLE_DATA_ENTETE_BLOCMEMPROGRAMME);
      #endif
      _taille=TAILLE_ENTETE_BLOCMEM + TAILLE_DATA_ENTETE_BLOCMEMPROGRAMME;

      actualiseEntete();
    }
    _nb = (_taille - TAILLE_ENTETE_BLOCMEMPROGRAMME) / (TAILLE_MEM_PROGRAMME + 1);

#ifdef DEBUG
    Serial.print("->   ");
    Serial.print("_taille=");
    Serial.print(_taille);
    Serial.print(" _addr=");
    Serial.print(_addr);
    Serial.print(" _mode=");
    Serial.print((char)_mode);
    Serial.print(" _curseur=");
    Serial.print(_curseur);
    Serial.print(" _nb=");
    Serial.println(_nb);
#endif
    compacte();
    getHC_HP(_HC_HP,true);
  }
}

uint32_t BlocMemProgramme::getDateDernierAcces(void){
  return lit32(this->adresseDateDernierAccess());
}
bool BlocMemProgramme::setDateDernierAcces(uint32_t date){
  return ecrit(date,this->adresseDateDernierAccess());
}
uint32_t BlocMemProgramme::getDateProchain(void){
  return lit32(this->adresseDateProchain());
}
bool BlocMemProgramme::setDateProchain(uint32_t date){
  return ecrit(date,this->adresseDateProchain());
}

uint16_t BlocMemProgramme::adresseElt_EEPROM(uint8_t num) const{
  if ( num >= _nb) return 0;
  /*#ifdef DEBUG
  Serial.print("BlocMemProgramme->adresseElt_EEPROM(");
  Serial.print(num);
  Serial.print("):");
  Serial.print(_addr + TAILLE_ENTETE_BLOCMEM + TAILLE_DATA_ENTETE_BLOCMEMPROGRAMME + ((TAILLE_MEM_PROGRAMME + 1) * num));
  #endif*/
  return (_addr + TAILLE_ENTETE_BLOCMEM + TAILLE_DATA_ENTETE_BLOCMEMPROGRAMME + ((TAILLE_MEM_PROGRAMME + 1) * num));
}

uint16_t BlocMemProgramme::adresseElt(uint8_t num) const{
  if ( num >= _nb) return 0;
  /*#ifdef DEBUG
  Serial.print("BlocMemProgramme->adresseElt(");
  Serial.print(num);
  Serial.print("):");
  Serial.print(TAILLE_DATA_ENTETE_BLOCMEMPROGRAMME + ((TAILLE_MEM_PROGRAMME + 1) * num));
  #endif*/
  return (TAILLE_DATA_ENTETE_BLOCMEMPROGRAMME + ((TAILLE_MEM_PROGRAMME + 1) * num));
}


bool BlocMemProgramme::setAt(uint8_t num, const Programme &programme) {
  if ( num >= _nb) return false;
  #ifdef DEBUG
  Serial.print("BlocMemProgramme->setAt(");
  Serial.print(num);
  Serial.print(")   adresseElt:");
  Serial.print(adresseElt_EEPROM(num));
  Serial.print(" adresseCRC:");
  Serial.print(adresseElt_EEPROM(num) + TAILLE_MEM_PROGRAMME);
  #endif
  // enregistrement du Programme
  programme.save(adresseElt_EEPROM(num));
  // calcul et enregistrement de la crc8
  if ( ! ecrit(crc8OneWireMemoire(adresseElt_EEPROM(num), TAILLE_MEM_PROGRAMME), adresseElt(num) + TAILLE_MEM_PROGRAMME)) return false;
  #ifdef DEBUG
  Serial.print(" crc lue:");
  Serial.print(lit8(adresseElt(num) + TAILLE_MEM_PROGRAMME));
  Serial.print(" crc calculée:");
  Serial.println(crc8OneWireMemoire(adresseElt_EEPROM(num), TAILLE_MEM_PROGRAMME));
  Serial.print("programme.date:");
  Serial.print(programme.date,HEX);
  Serial.print(" programme.temp:");
  Serial.println(programme.temp);
  Programme prog_=Programme();
  prog_.load(adresseElt_EEPROM(num));
  Serial.print(" - load - ");
  Serial.print("prog_.");
  prog_.println(Serial);
  #endif
  actualiseEntete();
  return true;
}

bool BlocMemProgramme::testAt(uint8_t num) {
  if ( num >= _nb) return false;

  return ( crc8OneWireMemoire(_addr + TAILLE_ENTETE_BLOCMEM + TAILLE_DATA_ENTETE_BLOCMEMPROGRAMME + ((TAILLE_MEM_PROGRAMME + 1) * num), TAILLE_MEM_PROGRAMME) == lit8(TAILLE_DATA_ENTETE_BLOCMEMPROGRAMME + ((TAILLE_MEM_PROGRAMME + 1) * num) + TAILLE_MEM_PROGRAMME));
}

bool BlocMemProgramme::getAt(uint8_t num, Programme &programme) {
  if ( num >= _nb) return false;
  if ( _numCharge == num) {
    programme = _programme;
    return true;
  }
  #ifdef DEBUG
  Serial.print("BlocMemProgramme->getAt(");
  Serial.print(num);
  Serial.print(")   adresseElt:");
  Serial.print(adresseElt_EEPROM(num));
  Serial.print(" adresseCRC:");
  Serial.print(adresseElt_EEPROM(num) + TAILLE_MEM_PROGRAMME);
  Serial.print(" crc lue:");
  Serial.print(lit8(adresseElt(num)+ TAILLE_MEM_PROGRAMME));
  Serial.print(" crc calculée:");
  Serial.println(crc8OneWireMemoire(adresseElt_EEPROM(num), TAILLE_MEM_PROGRAMME));
  #endif

  if ( crc8OneWireMemoire(adresseElt_EEPROM(num), TAILLE_MEM_PROGRAMME) != lit8(adresseElt(num) + TAILLE_MEM_PROGRAMME)) return false;

  programme.load(adresseElt_EEPROM(num));
  _programme=programme;
  _numCharge=num;
  #ifdef DEBUG
  programme.println(Serial);
  #endif
  return true;
}

void BlocMemProgramme::compacte(void) {
#ifdef DEBUG
  Serial.println("BlocMemProgramme::compacte()");
#endif
  _nbOK = 0;
  uint8_t k = 0;
  for (uint8_t i = 0; i < _nb; ++i) {
#ifdef DEBUG
    Serial.print("test emplacement:");
    Serial.println(i);
    Serial.print("   crc lue:");
    Serial.print(lit8(adresseElt(i) + TAILLE_MEM_PROGRAMME));
    Serial.print("  crc calculée:");
    Serial.print(crc8OneWireMemoire(adresseElt_EEPROM(i), TAILLE_MEM_PROGRAMME));
    Programme prog_=Programme();
    prog_.load(adresseElt_EEPROM(i));
    Serial.print(" - load - ");
    Serial.print("prog_.");
    prog_.print(Serial);
    Serial.print( "lit32EEPROM():");
    Serial.println(lit32(adresseElt(i)));
#endif
    if ( lit32( adresseElt(i)) != 0
      && crc8OneWireMemoire( adresseElt_EEPROM(i), TAILLE_MEM_PROGRAMME) == lit8(adresseElt(i) + TAILLE_MEM_PROGRAMME) ) { // l'emplacement est pris
#ifdef DEBUG
      Serial.println(" occupé");
#endif
      ++_nbOK;
      if (k != 0) k = 2; // il y a eu un trou entre 2 emplacement pris
    } else {
#ifdef DEBUG
      Serial.println(" libre");
#endif
      if (k == 0) k = 1; //
    }
  }
#ifdef DEBUG
  Serial.print("_nbOK=");
  Serial.println(_nbOK);
#endif

  // k==0 => tout emplacement pris
  // k==1 => compacté
  // k==2 => non compacté
  if ( k == 0 || k == 1) {
#ifdef DEBUG
    Serial.println("déjà compacté");
    if ( k == 0) Serial.println("tout emplacement pris");
    Serial.print("_nbOK=");
    Serial.println(_nbOK);
#endif
    return; // déjà compacté
  }

  k = 0;
  for (uint8_t i = 0; i < _nb; --i) {

    if ( crc8OneWireMemoire(_addr + TAILLE_ENTETE_BLOCMEMPROGRAMME + (TAILLE_MEM_PROGRAMME + 1) * i - TAILLE_MEM_PROGRAMME, TAILLE_MEM_PROGRAMME) == lit8((TAILLE_MEM_PROGRAMME + 1) * i)) { // l'emplacement i est occupé

      for (uint8_t j = k; j < i; ++j) {

        if ( crc8OneWireMemoire(_addr + 5 + (TAILLE_MEM_PROGRAMME + 1) * j - TAILLE_MEM_PROGRAMME, TAILLE_MEM_PROGRAMME) != lit8((TAILLE_MEM_PROGRAMME + 1) * j) && getAt(i, _programme)) { // l'emplacement j est libre
          if ( setAt(j, _programme)) {
#ifdef DEBUG
            Serial.print(i);
            Serial.print("=>");
            Serial.print(j);
#endif
            ++_nbOK;
            delAt(i, false);
            _numCharge = j;
            k = j + 1;
          }
          break;
        }
      }
    }
  }
}

uint8_t BlocMemProgramme::alloc(uint8_t nb, bool reduit) {
  if ( nb > _nb) { // aggrandissement

    #ifdef DEBUG
    Serial.print("aggrandissement (passage de ");
    Serial.print(_nb);
    Serial.print(" a ");
    Serial.print(nb);
    Serial.println(")");
    Serial.print("Avant: taille=");
    Serial.print(_taille);
    Serial.print(" nb=");
    Serial.println(_nb);
    #endif

    if ( _tailleMax != 0 && (TAILLE_ENTETE_BLOCMEMPROGRAMME + nb * (TAILLE_MEM_PROGRAMME + 1)) > _tailleMax ) nb = (_tailleMax - TAILLE_ENTETE_BLOCMEMPROGRAMME) / (TAILLE_MEM_PROGRAMME + 1);
    if ( _addr + (TAILLE_ENTETE_BLOCMEMPROGRAMME + nb * (TAILLE_MEM_PROGRAMME + 1)) > EEPROM.length() ) nb = (EEPROM.length() - _addr - TAILLE_ENTETE_BLOCMEMPROGRAMME) / (TAILLE_MEM_PROGRAMME + 1);
    _taille = TAILLE_ENTETE_BLOCMEMPROGRAMME + nb * (TAILLE_MEM_PROGRAMME + 1);

    for (uint8_t i = _nb; i < nb; ++i) delAt(i);
    _nb = nb;
    actualiseEntete();

  } else if ( reduit) { // reduction
    #ifdef DEBUG
    Serial.print("reduction (passage de ");
    Serial.print(_nb);
    Serial.print(" a ");
    Serial.print(nb);
    Serial.println(")");
    Serial.print("Avant: taille=");
    Serial.print(_taille);
    Serial.print(" nb=");
    Serial.println(_nb);
    #endif
    _taille = TAILLE_ENTETE_BLOCMEMPROGRAMME + nb * (TAILLE_MEM_PROGRAMME + 1);
    _nb = nb;
    actualiseEntete();
  }
  #ifdef DEBUG
  Serial.print("Apres: taille=");
  Serial.print(_taille);
  Serial.print(" nb=");
  Serial.println(_nb);
  #endif
  return _nb;
}


bool BlocMemProgramme::delAt(uint8_t num, bool decal) {
  if ( num >= _nb) return false;
  #ifdef DEBUG
  Serial.print("BlocMemProgramme->delAt(");
  Serial.print(num);
  Serial.print(",");
  Serial.print(decal);
  Serial.println(")");
  #endif

  while ( decal && num < _nbOK - 1) { // si celui que l'on supprime n'est pas le dernier!
    // alors on décale tout
    #ifdef DEBUG
    Serial.print("decalage ");
    Serial.print(num+1);
    Serial.print(" -> ");
    Serial.println(num);
    #endif
    getAt(num + 1, _programme);
    setAt(num++, _programme);
  }

  #ifdef DEBUG
  Serial.print("suppression @");
  Serial.print(adresseElt(num));
  Serial.print("(+");
  Serial.print(_addr + TAILLE_ENTETE_BLOCMEM);
  Serial.println(")");
  #endif
  for (uint8_t i = 0; i < TAILLE_MEM_PROGRAMME + 1; ++i) ecrit((uint8_t)0, adresseElt(num) + i);

  _nbOK--;
  return true;
}

bool BlocMemProgramme::getHC_HP(plageHC_HP& HC_HP,bool forceLecture) const{
  if(! forceLecture ) HC_HP=this->_HC_HP;
  HC_HP.load(this->adresseHC_HP_EEPROM());
  return true;
}

bool BlocMemProgramme::setHC_HP(const plageHC_HP& HC_HP){
  bool resultat=HC_HP.save(this->adresseHC_HP_EEPROM());
  this->actualiseEntete();
  this->_HC_HP=HC_HP;
  return resultat;
}

bool BlocMemProgramme::get(uint8_t num, Programme &programme){
  if ( num > _nbOK) return false;
  return getAt(num, programme);
}
bool BlocMemProgramme::set(uint8_t num, const Programme &programme) {
  if ( num > _nbOK) return false;
  return setAt(num, programme);
}
bool BlocMemProgramme::del(uint8_t num) {
  if ( num > _nbOK) return false;
  return delAt(num);
}

bool BlocMemProgramme::add(const Programme &programme) {
  if ( _nbOK >= _nb) return false;
  return setAt(_nbOK++, programme);
}

uint8_t BlocMemProgramme::_count() {
  uint8_t n = 0;
  for (uint8_t i = 0; i < _nb; ++i) {
    _curseur = _addr + TAILLE_ENTETE_BLOCMEMPROGRAMME + (TAILLE_MEM_PROGRAMME + 1) * i + TAILLE_MEM_PROGRAMME;
    if ( crc8OneWireMemoire(_curseur - TAILLE_MEM_PROGRAMME, TAILLE_MEM_PROGRAMME) == lit8()) ++n;
  }
  _nbOK = n;
  return n;
}

bool BlocMemProgramme::estRepetition(uint8_t num){
  if ( _numCharge != num) {
    get(num, _programme);
    _numCharge = num;
  }
  return _programme.estRepetition();
}
uint8_t BlocMemProgramme::mode(uint8_t num){
  if ( _numCharge != num) {
    get(num, _programme);
    _numCharge = num;
  }
  return _programme.mode();
}
uint8_t BlocMemProgramme::freqRepetition(uint8_t num){
  if ( _numCharge != num) {
    get(num, _programme);
    _numCharge = num;
  }
  return _programme.freqRepetition();
}
RtcDateTime  BlocMemProgramme::getRtcDateTime(uint8_t num){
  if ( _numCharge != num) {
    get(num, _programme);
    _numCharge = num;
  }
  return _programme.getRtcDateTime();
}
uint32_t BlocMemProgramme::getDate(uint8_t num){
  if ( _numCharge != num) {
    get(num, _programme);
    _numCharge = num;
  }
  return _programme.getDate();
}

unsigned long BlocMemProgramme::frequence(uint8_t num){
  if ( _numCharge != num) {
    get(num, _programme);
    _numCharge = num;
  }
  return _programme.frequence();
}

unsigned long BlocMemProgramme::decalage(uint8_t num){
  if ( _numCharge != num) {
    get(num, _programme);
    _numCharge = num;
  }
  return _programme.decalage();
}

uint8_t BlocMemProgramme::prochain(RtcDateTime& now,uint8_t p){
  uint32_t proDate=0xFFFFFFFF;
  uint8_t pro=0xFF;
  uint8_t i;

  if( p==0xFF){
    i=0;
  } else if( p > _nbOK) return 0xFF;
  else {
    if( get(p,_programme)){
      i=p+1;
      if( now.TotalSeconds() == _programme.prochain(now,_HC_HP)){ // le programme n'est pas à la date donnée => recherche du suivant dans le temps
        now=RtcDateTime(_programme.prochain(now,_HC_HP)+1);
      }
    } else return 0xFF;
  }

  for (; i < _nbOK; ++i) {
    if ( get(i, _programme)) {
      if (_programme.prochain(now, _HC_HP) < proDate && _programme.prochain(now, _HC_HP) != 0) {
        proDate = _programme.prochain(now, _HC_HP);
        pro = i;
      }
    }
  }
  if( pro == 0xFF && p!=0xFF ) {
    now=RtcDateTime(now.TotalSeconds()+1);
    return prochain(now);
  }
  return pro;
}

uint8_t BlocMemProgramme::precedent(RtcDateTime& now,uint8_t p){
  uint32_t proDate = 0;
  uint8_t pro = 0xFF;
  uint8_t i=0;

  if( p==0xFF){
    i=0;
  } else if( p > _nbOK) return 0xFF;
  else {
    if( get(p,_programme)){
      i=p+1;
      if( now.TotalSeconds() == _programme.precedent(now,_HC_HP)){ // le programme n'est pas à la date donnée => recherche du suivant dans le temps
        now=RtcDateTime(_programme.precedent(now,_HC_HP)-1);
      }
    } else return 0xFF;
  }

  for (; i < _nbOK; ++i) {
    if ( get(i, _programme)) {
      if (_programme.precedent(now, _HC_HP) > proDate) {
        proDate = _programme.precedent(now, _HC_HP);
        pro = i;
      }
    }
  }
  if( pro == 0xFF && p!=0xFF ) {
    now=RtcDateTime(now.TotalSeconds()-1);
    return precedent(now);
  }
  return pro;
}
