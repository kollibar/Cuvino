#include "SondeV2.h"

DebugLogger debugSondeV2=DebugLogger(SONDE_DEBUG_nom);

PileErreur* SondeV2::pileErreur=NULL;
Timer* SondeV2::timer=NULL;
QueueHandle_t SondeV2::qCmdeToCore=NULL;
QueueHandle_t SondeV2::qCmdeToTimer=NULL;

void SondeV2::begin(PileErreur& _pileErreur,Timer& _timer,QueueHandle_t queueCmdeToCore,QueueHandle_t queueCmdeToTimer){
  pileErreur=&_pileErreur;
  timer=&_timer;
  qCmdeToCore=queueCmdeToCore;
  qCmdeToTimer=queueCmdeToTimer;
}


SondeV2::SondeV2(OneWire* ds, signed char num){
  this->num=num;
  switch(num){
    case SONDE_DS3231:
    case AUCUNE_SONDE:
      this->ds=NULL;
      for(unsigned char i=0;i<8;++i) addr[i]=0;
      break;
    default:
      this->ds=ds;
      this->chercheAdresse(); // comme l'adresse de la sonde n'est pas donnée, il faut la chercher !
      this->setPrecision(this->precision);
  }
}

SondeV2::SondeV2(OneWire* ds, signed char num, unsigned char _addr[8]){
  this->num=num;
  switch(num){
    case SONDE_DS3231:
    case AUCUNE_SONDE:
      this->ds=NULL;
      for(unsigned char i=0;i<8;++i) addr[i]=0;
      break;
    default:
      this->ds=ds;
      for(unsigned char i=0;i<8;++i) addr[i]=_addr[i];
      if( isDS18B20()) this->setPrecision(this->precision);
  }
}

void SondeV2::chercheAdresse(){
  switch (num) {
    case AUCUNE_SONDE:
      for(unsigned char i=0;i<8;++i) addr[i]=0;
      break;
    case SONDE_DS3231:
      addr[0]='D';
      addr[1]='S';
      addr[2]='3';
      addr[3]='2';
      addr[4]='3';
      addr[5]='1';
      addr[6]=0;
      addr[7]=0;
      break;
    default:
      getDS18B20(*this->ds, &this->addr[0], 1);
  }
}

bool SondeV2::setPrecision(unsigned char _precision){
  /* Reset le bus 1-Wire, sélectionne le capteur et envoie une demande d'écriture du scratchpad */

  if( ! this->isDS18B20() ) return false;
  if( this->precision == _precision) return true;
  ds->reset();
  ds->select(addr);
  ds->write(0x4E);

  /* Ecrit dans le scratchpad */
  ds->write(0x00);
  ds->write(0x00);
  switch (_precision) {
    case PRECISION_HAUTE:
      ds->write(0b01111111);
      break;
    case PRECISION_MOYENNE:
      ds->write(0b01111111);
      break;
    case PRECISION_BASSE:
      ds->write(0b01111111);
      break;
    case PRECISION_MAUVAISE:
      ds->write(0b01111111);
      break;
  }

  /* Fin d'écriture */
  ds->reset();
  this->precision=_precision;

  return true;
}
/*

void SondeV2::defaut(void){
  for(unsigned char i=0;i<8;++i) this->addr[i]=0;
  this->a=0;
  this->b=0;
  this->date=0;
  this->precision=PRECISION_HAUTE;
  this->num=AUCUNE_SONDE;
}

uint16_t SondeV2::save(uint16_t addr,unsigned char v) {
  if( ! isSondeTemp()){
    for(unsigned char i=0;i<7;++i) this->addr[i]=0;
    this->addr[8]=num;
  } else if( isDS3231() ){
    this->addr[8]=num;
  }

  addr = ecritEEPROM((byte*)&(this->addr), 8, addr);
  if( isDS18B20() ){ // cas du 18b20, il faut rajouter le numero de sonde à la place du code 28 caractéristique du DS18b20
    EEPROM.write(addr-8,this->num);
  }
  EEPROM.write(addr, a); ++addr;
  EEPROM.write(addr, b); ++addr;
  #ifdef DEBUG
  debugSondeV2.print("SondeV2::save(addr=");
  debugSondeV2.print(addr);
  debugSondeV2.print(") a=");
  debugSondeV2.print(this->a);
  debugSondeV2.print(", b=");
  debugSondeV2.println(this->b);
  #endif
  return addr;
}

bool SondeV2::save(BlocMem* bloc,unsigned char v) {
  bool ds18b20=false;
  if( ! isSondeTemp()){
    for(unsigned char i=0;i<7;++i) addr[i]=0;
    addr[8]=num;
  } else if( isDS3231() ){
    addr[8]=num;
  }
  else if( isDS18B20()){
    ds18b20=true;
    addr[0]=num;
  }

  if ( ! bloc->ecrit((byte*)&addr, 8)) {
    if( ds18b20 ) addr[0]=DS18B20;
    return false;
  }
  if( ds18b20 ) addr[0]=DS18B20;

  if ( ! bloc->ecrit((byte*)&a, 1)) return false;
  if ( ! bloc->ecrit((byte*)&b, 1)) return false;
  #ifdef DEBUG
  debugSondeV2.print("SondeV2::save(bloc) a=");
  debugSondeV2.print(this->a);
  debugSondeV2.print(", b=");
  debugSondeV2.println(this->b);
  #endif
  return true;
}

uint16_t SondeV2::load(uint16_t addr,unsigned char v) {
  defaut();
  addr = litEEPROM((byte*)&(this->addr), 8, addr);

  if( (this->addr[0]=='D' && this->addr[1]=='S' && this->addr[2]=='3' && this->addr[3]=='2' && this->addr[4]=='3' && this->addr[5]=='1')
    || (this->addr[0]==0 && this->addr[1]==0 && this->addr[2]==0 && this->addr[3]==0 && this->addr[4]==0 && this->addr[5]==0) )  num=this->addr[8];
  else {
    num=this->addr[0];
    this->addr[0]=DS18B20;
  }
  this->ds=Brochage::oneWireSonde(num);

  a = EEPROM.read(addr); ++addr;
  b = EEPROM.read(addr); ++addr;

  return addr;
}

bool SondeV2::load(BlocMem* bloc,unsigned char v) {
  defaut();
  if ( ! bloc->lit((byte*)&addr, 8)) return false;
  if( (this->addr[0]=='D' && this->addr[1]=='S' && this->addr[2]=='3' && this->addr[3]=='2' && this->addr[4]=='3' && this->addr[5]=='1')
    || (this->addr[0]==0 && this->addr[1]==0 && this->addr[2]==0 && this->addr[3]==0 && this->addr[4]==0 && this->addr[5]==0) )  num=this->addr[8];
  else {
    num=this->addr[0];
    this->addr[0]=DS18B20;
  }
  this->ds=Brochage::oneWireSonde(num);
  if ( ! bloc->lit((byte*)&a, 1)) return false;
  if ( ! bloc->lit((byte*)&b, 1)) return false;
  return true;
}*/

void SondeV2::print(HardwareSerial& serial){
  serial.print("SondeV2(");
  serial.print(num);
  serial.print(',');
  for(unsigned char i=0;i<8;++i) {
    if( addr[i]<16)serial.print(0);
    serial.print(addr[i],HEX);
  }
  serial.println(')');

  if( isSondeTemp() ){
    serial.print("! isSondeTemp ");
    if( isDS3231() ) serial.print("isDS3231");
    else if( isDS18B20() ) serial.print("isDS18B20");
  } else{
    serial.print("! isSondeTemp ");
  }
  serial.println();
}

void SondeV2::print(DebugLogger& debug){
  debug.print("SondeV2(");
  debug.print(num);
  debug.print(',');
  for(unsigned char i=0;i<8;++i) {
    if( addr[i]<16) debug.print(0);
    debug.print(addr[i],HEX);
  }
  debug.println(')');
  if( isSondeTemp() ){
    debug.print("! isSondeTemp ");
    if( isDS3231() ) debug.print("isDS3231");
    else if( isDS18B20() ) debug.print("isDS18B20");
  } else{
    debug.print("! isSondeTemp ");
  }
  debug.println();
}

bool SondeV2::demandeMesureTemp() {
  /* demande le lancement d'une mesure de température sur le bus 1wire, à la sonde d'addr donnée. La mesure sera dispo dans 750ms */
  if (addr[0] != DS18B20) // Vérifie qu'il s'agit bien d'un DS18B20
    return false;         // Si ce n'est pas le cas on retourne une erreur

  if( (millis() - this->date) < (750 / (this->precision+1)) ) return true; // il y a déjà une demande de mesure de température effetuée il y a moins de 800 ms


  ds->reset();             // On reset le bus 1-Wire
  ds->select(addr);        // On sélectionne le DS18B20

  ds->write(0x44, 1);      // On lance une prise de mesure de température
  date=millis();
  return true;
}

bool SondeV2::mesureFaite(){
  return ((millis() - date) > (750 / (this->precision+1)) && (millis() - date) < 5000);
}

signed int SondeV2::waitForMesure(const bool correction) {
  if( !isSondeTemp() )return TEMP_ERREUR;
  if( isDS3231() ) return litTemperature(correction);

  unsigned long delai = millis() - date;
  if( delai > (750 / (this->precision+1)) && delai < 5000 ) return litTemperature(correction); // mesure déjà faite

  if( delai > (750 / (this->precision+1)))  {
    demandeMesureTemp(); // si pas de mesure déjà faite on en demande une
    delai=(750 / (this->precision+1));
  } else {
    delai=(750 / (this->precision+1))-delai;
  }

  if( delai >= 0){
    if( xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED ){
      delay(delai); // si le noyau FreeRTOS est inactif
    } else {
      vTaskDelay( delai / portTICK_PERIOD_MS ); // si le noyau FreeRTOS est actif
    }
  }

  signed int mesure =litTemperature(correction);

  if( mesure == 1360 ){ // erreur de mesure, on réessaye une 2e fois (seulement une 2e fois)
    demandeMesureTemp();
    delai=(750 / (this->precision+1));
    if( xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED ){
      delay(delai); // si le noyau FreeRTOS est inactif
    } else {
      vTaskDelay( delai / portTICK_PERIOD_MS ); // si le noyau FreeRTOS est actif
    }
    mesure=litTemperature(correction);
  }

  return mesure;
}

signed int SondeV2::litTemperature(const bool correction) {
  /* lit la temperature de la dernière mesure effctué avec la sonde à l'addr donnée*/

  if (addr[0] != DS18B20) { // Vérifie qu'il s'agit bien d'un DS18B20
    if( isDS3231() ) return getTemperatureDS3231(); // si il s'agit de DS3231 (RTC avec compensation de température)
    //if (addrIsDS3231(addr)) return getTemperatureDS3231();
    return TEMP_ERREUR;   // Si ce n'est pas le cas on retourne une erreur
  }


  unsigned char data[9];

  ds->reset();             // On reset le bus 1-Wire
  ds->select(addr);        // On sélectionne la sonde
  ds->write(0xBE,1);         // On envoie une demande de lecture du scratchpad

  for (unsigned char i = 0; i < 9; ++i) // On lit le scratchpad
    data[i] = ds->read();       // Et on stock les octets reçus

  // Calcul de la température en 1/16e de degré Celsius

  if( correction ) return (correctionMesure( ((data[1] << 8) | data[0])<< this->precision ));
  else return ( ((data[1] << 8) | data[0]) << this->precision );
}

signed int SondeV2::getTemperature(const bool correction) {
  /* demande une mesure de la température et attend (800ms) que la mesure soit faite, puis retourne la valeur */

  if (addr[0] != DS18B20) { // Vérifie qu'il s'agit bien d'un DS18B20
    if( isDS3231() ) return getTemperatureDS3231(); // si il s'agit de DS3231 (RTC avec compensation de température)
    //if (addrIsDS3231(addr)) return getTemperatureDS3231();
    return TEMP_ERREUR;   // Si ce n'est pas le cas on retourne une erreur
  }

  unsigned long delai=750 / (this->precision+1);

  if( millis() - date < delai){ // une demande de mesure de température a été effctuée il y a moins de 800 ms
    delai=delai-(millis()-date);
  } else { // sinon on fait une demande de mesure de température
    demandeMesureTemp();
  }

  // Et on attend la fin de la mesure
  if( xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED ){
    delay(delai); // si le noyau FreeRTOS est inactif
  } else {
    vTaskDelay( delai / portTICK_PERIOD_MS ); // si le noyau FreeRTOS est actif
  }

  signed long mesure=litTemperature(correction);

  if( mesure == 1360 ){ // erreur de mesure, on réessaye une 2e fois (seulement une 2e fois)
    demandeMesureTemp();
    if( xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED ){
      delay(delai); // si le noyau FreeRTOS est inactif
    } else {
      vTaskDelay( delai / portTICK_PERIOD_MS ); // si le noyau FreeRTOS est actif
    }
    mesure=litTemperature(correction);
  }

  return mesure;
}

signed int SondeV2::correctionMesure(signed int mesure){
  #ifdef DEBUG
  debugSondeV2.print("correctionMesure(");
  debugSondeV2.print(mesure);
  debugSondeV2.print(")=>");
  #endif
  if( this->a != 0 ){
    mesure=(signed int)(((255+(signed long)a)*(signed long)mesure)/255);
  }
  if( this->b != 0 ){
    mesure=mesure+(signed int)b;
  }
  #ifdef DEBUG
  debugSondeV2.print(mesure);
  debugSondeV2.print(" (a=");
  debugSondeV2.print(a);
  debugSondeV2.print(", b=");
  debugSondeV2.print(b);
  debugSondeV2.println(")");
  #endif
  return mesure;
}

bool SondeV2::calcCorrection(signed int mesure,signed int tempReelle){
  this->a=0;
  this->b=(signed char)(tempReelle-mesure);
  #ifdef DEBUG
  debugSondeV2.print("calcCorrection(");
  debugSondeV2.print(mesure);
  debugSondeV2.print(",");
  debugSondeV2.print(tempReelle);
  debugSondeV2.print(")=>a=");
  debugSondeV2.print(this->a);
  debugSondeV2.print(",b=");
  debugSondeV2.println(this->b);
  #endif
  return true;
}

bool SondeV2::calcCorrection(signed int mesure1,signed int tempReelle1,signed int mesure2,signed int tempReelle2){
  this->a=(signed char)((255*tempReelle1-255*tempReelle1)/(mesure1-mesure2)-255);
  this->b=(signed char)(tempReelle1-(((255+(signed long)a)*mesure1)/255));
  #ifdef DEBUG
  debugSondeV2.print("calcCorrection(");
  debugSondeV2.print(mesure1);
  debugSondeV2.print(",");
  debugSondeV2.print(tempReelle1);
  debugSondeV2.print(",");
  debugSondeV2.print(mesure2);
  debugSondeV2.print(",");
  debugSondeV2.print(tempReelle2);
  debugSondeV2.print(")=>a=");
  debugSondeV2.print(this->a);
  debugSondeV2.print(",b=");
  debugSondeV2.println(this->b);
  #endif
  return true;
}

bool SondeV2::isSondeTemp(){
  return( num == SONDE_DS3231 || (num != AUCUNE_SONDE && addr[0]==DS18B20));
  //return ((addr[0]==DS18B20 && num != AUCUNE_SONDE) || (num == SONDE_DS3231 && addr[0]=='D' && addr[1] == 'S' && addr[2] == '3' && addr[3] == '2' && addr[4] == '3' && addr[5] == '1' && addr[6] == 0));
}

bool SondeV2::isDS18B20(){
  return (num != AUCUNE_SONDE && addr[0]==DS18B20);
}

bool SondeV2::isDS3231(){
  return( num == SONDE_DS3231 );
  //return (num == SONDE_DS3231 && addr[0]=='D' && addr[1] == 'S' && addr[2] == '3' && addr[3] == '2' && addr[4] == '3' && addr[5] == '1' && addr[6] == 0);
}
