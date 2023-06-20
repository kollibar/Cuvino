#include "Sonde.h"

DebugLogger debugSonde=DebugLogger(SONDE_DEBUG_nom);
SemaphoreHandle_t semaphoreAccesSonde = NULL;

PileErreur* Sonde::pileErreur=NULL;
Timer* Sonde::timer=NULL;
OneWire* Sonde::ds=NULL;
QueueHandle_t Sonde::qCmdeToCore=NULL;
QueueHandle_t Sonde::qCmdeToTimer=NULL;

void Sonde::begin(PileErreur& _pileErreur,Timer& _timer,OneWire& _ds,QueueHandle_t queueCmdeToCore,QueueHandle_t queueCmdeToTimer){
  pileErreur=&_pileErreur;
  timer=&_timer;
  ds=&_ds;
  qCmdeToCore=queueCmdeToCore;
  qCmdeToTimer=queueCmdeToTimer;
}

Sonde::Sonde(){
  addr[0]=0;
}

Sonde::Sonde(unsigned char _addr[8]){
  for(unsigned char i=0;i<8;++i) addr[i]=_addr[i];
  this->setPrecision(this->precision);
}

void Sonde::defaut(void){
  this->addr[0]=0;
  this->a=0;
  this->b=0;
  this->date=0;
  this->precision=PRECISION_HAUTE;
}

bool Sonde::setPrecision(unsigned char _precision){
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

uint16_t Sonde::save(uint16_t addr) {
  // addr = ecritEEPROM((byte*)&(this->addr), 8, addr);
  // EEPROM.write(addr, a); ++addr;
  // EEPROM.write(addr, b); ++addr;
  eeprom_update_block(&this->addr, (void*)addr,8); addr+=8;
  eeprom_update_byte((uint8_t*)addr,a);addr+=1;
  eeprom_update_byte((uint8_t*)addr,b);addr+=1;
  #ifdef DEBUG
  debugSonde.print(F("Sonde::save(addr="));
  debugSonde.print(addr);
  debugSonde.print(F(") a="));
  debugSonde.print(this->a);
  debugSonde.print(F(", b="));
  debugSonde.println(this->b);
  #endif
  return addr;
}

bool Sonde::save(BlocMem* bloc) {
  if ( ! bloc->ecrit((byte*)&addr, 8)) return false;
  if ( ! bloc->ecrit((byte*)&a, 1)) return false;
  if ( ! bloc->ecrit((byte*)&b, 1)) return false;
  #ifdef DEBUG
  debugSonde.print(F("F(onde::save(bloc) a=")));
  debugSonde.print(this->a);
  debugSonde.print(F(", b="));
  debugSonde.println(this->b);
  #endif
  return true;
}

uint16_t Sonde::load(uint16_t addr) {
  defaut();
  // addr = litEEPROM((byte*)&(this->addr), 8, addr);
  // a = EEPROM.read(addr); ++addr;
  // b = EEPROM.read(addr); ++addr;

  eeprom_read_block((void*)&this->addr, (void*) addr, 8);addr+=8;
  a=eeprom_read_byte((uint8_t*)addr);addr+=1;
  b=eeprom_read_byte((uint8_t*)addr);addr+=1;
  #ifdef DEBUG
  debugSonde.print(F("Sonde::load(addr="));
  debugSonde.print(addr);
  debugSonde.print(F(") a="));
  debugSonde.print(this->a);
  debugSonde.print(F(", b="));
  debugSonde.println(this->b);
  #endif
  return addr;
}

bool Sonde::load(BlocMem* bloc) {
  if ( ! bloc->lit((byte*)&addr, 8)) return false;
  if ( ! bloc->lit((byte*)&a, 1)) return false;
  if ( ! bloc->lit((byte*)&b, 1)) return false;
  #ifdef DEBUG
  debugSonde.print(F("Sonde::load(bloc) a="));
  debugSonde.print(this->a);
  debugSonde.print(F(", b="));
  debugSonde.println(this->b);
  #endif
  return true;
}

void Sonde::print(HardwareSerial& serial){
  serial.print(F("Sonde("));
  for(unsigned char i=0;i<8;++i) {
    if( addr[i]<16)serial.print(0);
    serial.print(addr[i],HEX);
  }
  serial.print(')');
}

void Sonde::print(DebugLogger& debug){
  debug.print(F("Sonde("));
  for(unsigned char i=0;i<8;++i) {
    if( addr[i]<16) debug.print(0);
    debug.print(addr[i],HEX);
  }
  debug.print(')');
}

bool Sonde::demandeMesureTemp() {
  /* demande le lancement d'une mesure de température sur le bus 1wire, à la sonde d'addr donnée. La mesure sera dispo dans 750ms */
  if (addr[0] != DS18B20) // Vérifie qu'il s'agit bien d'un DS18B20
    return false;         // Si ce n'est pas le cas on retourne une erreur

  if( millis() - date < (750 / (this->precision+1)) ) return true; // il y a déjà une demande de mesure de température effetuée il y a moins de 800 ms

  ds->reset();             // On reset le bus 1-Wire
  ds->select(addr);        // On sélectionne le DS18B20

  ds->write(0x44, 1);      // On lance une prise de mesure de température
  date=millis();
  return true;
}

bool Sonde::mesureFaite(){
  return ((millis() - date) > (750 / (this->precision+1)) && (millis() - date) < 5000);
}

signed int Sonde::litTemperature(const bool correction) {
  /* lit la temperature de la dernière mesure effctué avec la sonde à l'addr donnée*/

  if (addr[0] != DS18B20) { // Vérifie qu'il s'agit bien d'un DS18B20
    if (addrIsDS3231(addr)) return getTemperatureDS3231(); // si il s'agit de DS3231 (RTC avec compensation de température)
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

signed int Sonde::getTemperature(const bool correction) {
  /* demande une mesure de la température et attend (800ms) que la mesure soit faite, puis retourne la valeur */

  if (addr[0] != DS18B20) { // Vérifie qu'il s'agit bien d'un DS18B20
    if (addrIsDS3231(addr)) return getTemperatureDS3231(); // si il s'agit de DS3231 (RTC avec compensation de température)
    return TEMP_ERREUR;   // Si ce n'est pas le cas on retourne une erreur
  }

  //prendRessourceSonde(); //on bloque l'acces aux sondes

  ds->reset();             // On reset le bus 1-Wire
  ds->select(addr);        // On sélectionne le DS18B20

  ds->write(0x44, 1);      // On lance une prise de mesure de température

  unsigned char data[9];
  unsigned long delai=750 / (this->precision+1);

  if( millis() - date < delai){ // une demande de mesure de température a été effctuée il y a moins de 800 ms
    delai=delai-(millis()-date);
  } else { // sinon on fait une demande de mesure de température
    ds->reset();             // On reset le bus 1-Wire
    ds->select(addr);        // On sélectionne le DS18B20

    ds->write(0x44, 1);      // On lance une prise de mesure de température
  }

  // Et on attend la fin de la mesure
  if( xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED ){
    delay(delai); // si le noyau FreeRTOS est inactif
  } else {
    vTaskDelay( delai / portTICK_PERIOD_MS ); // si le noyau FreeRTOS est actif
  }

  ds->reset();             // On reset le bus 1-Wire
  ds->select(addr);        // On sélectionne le DS18B20
  ds->write(0xBE,1);         // On envoie une demande de lecture du scratchpad

  for (unsigned char i = 0; i < 9; ++i) // On lit le scratchpad
    data[i] = ds->read();       // Et on stock les octets reçus

  //rendRessourceSonde(); // on déblocke l'acces aux sondes

  // Calcul de la température en 1/16e de degré Celsius
  if( correction ) return (correctionMesure( ((data[1] << 8) | data[0])<< this->precision ));
  else return ( ((data[1] << 8) | data[0]) << this->precision );
}

signed int Sonde::correctionMesure(signed int mesure){
  #ifdef DEBUG
  debugSonde.print(F("correctionMesure("));
  debugSonde.print(mesure);
  debugSonde.print(F(")=>"));
  #endif
  if( this->a != 0 ){
    mesure=(signed int)(((255+(signed long)a)*(signed long)mesure)/255);
  }
  if( this->b != 0 ){
    mesure=mesure+(signed int)b;
  }
  #ifdef DEBUG
  debugSonde.print(mesure);
  debugSonde.print(F(" (a="));
  debugSonde.print(a);
  debugSonde.print(F(", b="));
  debugSonde.print(b);
  debugSonde.println(")");
  #endif
  return mesure;
}

bool Sonde::calcCorrection(signed int mesure,signed int tempReelle){
  this->a=0;
  this->b=(signed char)(tempReelle-mesure);
  #ifdef DEBUG
  debugSonde.print(F("calcCorrection("));
  debugSonde.print(mesure);
  debugSonde.print(F(","));
  debugSonde.print(tempReelle);
  debugSonde.print(F(")=>a="));
  debugSonde.print(this->a);
  debugSonde.print(F(",b="));
  debugSonde.println(this->b);
  #endif
  return true;
}
bool Sonde::calcCorrection(signed int mesure1,signed int tempReelle1,signed int mesure2,signed int tempReelle2){
  this->a=(signed char)((255*tempReelle1-255*tempReelle1)/(mesure1-mesure2)-255);
  this->b=(signed char)(tempReelle1-(((255+(signed long)a)*mesure1)/255));
  #ifdef DEBUG
  debugSonde.print(F("calcCorrection(");
  debugSonde.print(mesure1);
  debugSonde.print(',');
  debugSonde.print(tempReelle1);
debugSonde.print(',');
  debugSonde.print(mesure2);
  debugSonde.print(','');
  debugSonde.print(tempReelle2);
  debugSonde.print(F(")=>a="));
  debugSonde.print(this->a);
  debugSonde.print(F(",b="));
  debugSonde.println(this->b);
  #endif
  return true;
}

bool Sonde::isSondeTemp(){
  return (addr[0]==DS18B20 || (addr[0]=='D' && addr[1] == 'S' && addr[2] == '3' && addr[3] == '2' && addr[4] == '3' && addr[5] == '1' && addr[6] == 0));
}

bool Sonde::isDS18B20(){
  return (addr[0]==DS18B20);
}

bool Sonde::isDS3231(){
  return (addr[0]=='D' && addr[1] == 'S' && addr[2] == '3' && addr[3] == '2' && addr[4] == '3' && addr[5] == '1' && addr[6] == 0);
}

Sonde &Sonde::operator=(const Sonde &source){
  if (&source != this){
    for(unsigned char i=0;i<8;++i) this->addr[i]=source.addr[i];
  }
  this->a=source.a;
  this->b=source.b;
  return *this;
}

bool Sonde::operator==(const Sonde& s) const{
  for(unsigned char i=0;i<8;++i) {
    if(this->addr[i]!=s.addr[i]) return false;
  }
  return true;
}

bool Sonde::operator!=(const Sonde& s) const{
  for(unsigned char i=0;i<8;++i) {
    if(this->addr[i]!=s.addr[i]) return true;
  }
  return false;
}
