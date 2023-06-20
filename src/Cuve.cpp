#include "Cuve.h"

DebugLogger debugCuve=DebugLogger(CUVE_DEBUG_nom);

// --- Cuve --- //

PileErreur* Cuve::pileErreur=NULL;
Timer* Cuve::timer=NULL;
OneWire* Cuve::ds=NULL;
QueueHandle_t Cuve::qCmdeToCore=NULL;
QueueHandle_t Cuve::qCmdeToTimer=NULL;

void Cuve::begin(PileErreur& _pileErreur,Timer& _timer,OneWire& _ds,QueueHandle_t queueCmdeToCore,QueueHandle_t queueCmdeToTimer, MCP23S17* gpio){
  pileErreur=&_pileErreur;
  timer=&_timer;
  ds=&_ds;
  qCmdeToCore=queueCmdeToCore;
  qCmdeToTimer=queueCmdeToTimer;
  this->EV_F.begin(_pileErreur,_timer,queueCmdeToCore,queueCmdeToTimer, gpio);
  this->sonde.begin(_pileErreur,_timer,_ds,queueCmdeToCore,queueCmdeToTimer);
}

uint16_t Cuve::save(uint16_t addr) {
  addr = this->sonde.save(addr);
  // //addr = ecritEEPROM((byte*)&this->sonde.addr, 8, addr);
  // addr = ecritEEPROM((byte *)&this->tempConsigneCuve, 2, addr);
  // EEPROM.write(addr, this->nom); ++addr;
  eeprom_update_word((uint16_t*)addr, this->tempConsigneCuve);addr+=2;
  eeprom_update_byte((uint8_t*)addr, this->nom);addr++;
  addr = this->EV_F.save(addr);
  addr = this->EV_C.save(addr);
  return addr;
}

bool Cuve::save(BlocMem* bloc) {
  if ( ! this->sonde.save(bloc)) return false;
  //if ( ! bloc->ecrit((byte*)&this->sonde.addr, 8)) return false;
  if ( ! bloc->ecrit((byte*)&this->tempConsigneCuve, 2)) return false;
  if ( ! bloc->ecrit((byte*)&this->nom, 1)) return false;
  if ( ! this->EV_F.save(bloc)) return false;
  if ( ! this->EV_C.save(bloc)) return false;
  return true;
}

uint16_t Cuve::load(uint16_t addr) {
  this->defaut();
  addr = this->sonde.load(addr);
  // //addr = litEEPROM((byte*)&this->sonde.addr, 8, addr);
  // addr = litEEPROM((byte*)&this->tempConsigneCuve, 2, addr);
  // this->nom = EEPROM.read(addr); ++addr;
  this->tempConsigneCuve = eeprom_read_word((uint16_t*)addr);addr+=2;
  this->nom=eeprom_read_byte((uint8_t*)addr);addr+=1;
  addr = this->EV_F.load(addr);
  addr = this->EV_C.load(addr);
  return addr;
}

bool Cuve::load(BlocMem* bloc) {
  if ( ! this->sonde.load(bloc)) return false;
  //if ( ! bloc->lit((byte*)&this->sonde.addr, 8)) return false;
  if ( ! bloc->lit((byte*)&this->tempConsigneCuve, 2)) return false;
  if ( ! bloc->lit((byte*)&this->nom, 1)) return false;
  if ( ! this->EV_F.load(bloc)) return false;
  if ( ! this->EV_C.load(bloc)) return false;
  return true;
}

void Cuve::defaut(void) {
  this->sonde.defaut();
  this->tempConsigneCuve = TEMP_LIGNE_NON_CONFIGURE;
  this->nom = ' ';
  this->EV_F.defaut();
  this->EV_C.defaut();
  this->dateMesure=0;
}

bool Cuve::estConfigure() {
  return (tempConsigneCuve != TEMP_LIGNE_NON_CONFIGURE && tempConsigneCuve != TEMP_SONDE_NON_CONFIGURE && (EV_F.estConfigure() || EV_C.estConfigure()));
}

signed int Cuve::getTemperature(){
  if( ! this->sonde.isSondeTemp() ) return TEMP_ERREUR;
  if( this->sonde.isDS18B20() ) return sonde.getTemperature();

  #ifdef DEBUG
  if( dateMesure != 0 && (millis() - dateMesure) < 5000 ){
  #else
  if( dateMesure != 0 && (millis() - dateMesure) < 15000 ){
  #endif
    return mesure;
  }

  this->sonde.demandeMesureTemp();

  unsigned long delai=800 / (this->sonde.getPrecision()+1);

  if( xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED ){
    delay(delai); // si le noyau FreeRTOS est inactif
  } else {
    vTaskDelay( delai / portTICK_PERIOD_MS ); // si le noyau FreeRTOS est actif
  }

  mesure=this->sonde.litTemperature();
  dateMesure=millis();

  return mesure;
}
signed int Cuve::getEcart(){
  return (this->getTemperature() - tempConsigneCuve);
}

bool Cuve::setPrecisionMesure(unsigned char precision){
  return this->sonde.setPrecision(precision);
}
unsigned char Cuve::getPrecisionMesure() const{
  return this->sonde.getPrecision();
}

bool Cuve::controlTemp(int decalage){
  signed int  tempObj, temp;

  if (this->sonde.isSondeTemp()) {
    /*this->sonde.demandeMesureTemp();
    vTaskDelay( (750 / (this->sonde.getPrecision() + 1)) / portTICK_PERIOD_MS);
    temp = this->sonde.litTemperature();*/
    temp=this->sonde.getTemperature();
    if( temp == 1360 ) temp=this->sonde.getTemperature(); // si température = 85° => erreur de mesure on recommence
    if( temp == 1360 ) temp=this->sonde.getTemperature(); // si température = 85° => erreur de mesure on recommence une 2e fois
    // A FAIRE si plus de 2 erreur de lecture de température..
    #ifdef DEBUG
    this->sonde.print(debugCuve);
    #endif
  }
  else {
    if (this->sonde.addr[0] == 0) temp = TEMP_ERREUR_ABS_SONDE;
    else temp = TEMP_ERREUR_PAS_SONDE;
    return false;
  }

  if ( this->tempConsigneCuve  == TEMP_ARRET) { //si arrêt de la régulation
    if ( this->EV_F.getMode() != EV_NON_CONFIGURE && this->EV_F.getPosition() != FERME) { // EV froid
      this->EV_F.bougeEV(FERME);
    }
    if ( this->EV_C.getMode() != EV_NON_CONFIGURE && this->EV_C.getPosition() != FERME) { // EV chaud
      this->EV_C.bougeEV(FERME);
    }
    return true;
  }

  tempObj = this->tempConsigneCuve + decalage;

  if (this->tempConsigneCuve <= 2000 && this->tempConsigneCuve >= -880) { // si la température de consigne est comprise dans la plage du capteur
    if (this->sonde.addr[0] == DS18B20) {
      //contrôle de la ligne en interronpant le froid
#ifdef DEBUG
      debugCuve.printPGM(CUVE_DEBUG_LIGNE);
      debugCuve.println((char)this->nom);
      debugCuve.printPGM(FR::TXT_TEMPERATURE);
      debugCuve.write(' ');
      debugCuve.printPGM(FR::TXT_OBJECTIF);
      debugCuve.write(':');
      debugCuve.print(tempObj >> 4, DEC);
      debugCuve.write(' ');
      debugCuve.printPGM(FR::TXT_MESUREE);
      debugCuve.write(':');
      debugCuve.println(temp >> 4, DEC);

      debugCuve.printPGM(FR::TXT_EV);
      debugCuve.write(' ');
      debugCuve.printPGM(FR::TXT_FROID);
      debugCuve.print("  ");
      debugCuve.printPGM(FR::TXT_MODE);
      debugCuve.write(':');
      debugCuve.printPGM(TXT_EV(this->EV_F.getMode()));
      debugCuve.write('(');
      debugCuve.print(this->EV_F.getMode(),BIN);
      debugCuve.write(')');
      debugCuve.print(" / ");
      debugCuve.printPGM(FR::TXT_POSITION_ACTUELLE);
      debugCuve.write(':');
      debugCuve.print(this->EV_F.getPosition());
      debugCuve.write('(');
      if ( this->EV_F.getPosition() == OUVERT) debugCuve.printPGM(FR::TXT_OUVERT);
      else if ( this->EV_F.getPosition() == FERME) debugCuve.printPGM(FR::TXT_FERME);
      debugCuve.println(')');

      debugCuve.printPGM(FR::TXT_EV);
      debugCuve.write(' ');
      debugCuve.printPGM(FR::TXT_CHAUD);
      debugCuve.print("  ");
      debugCuve.printPGM(FR::TXT_MODE);
      debugCuve.write(':');
      debugCuve.printPGM(TXT_EV(this->EV_C.getMode()));
      debugCuve.write('(');
      debugCuve.print(this->EV_C.getMode(),BIN);
      debugCuve.write(')');
      debugCuve.print(" / ");
      debugCuve.printPGM(FR::TXT_POSITION_ACTUELLE);
      debugCuve.write(':');
      debugCuve.print(this->EV_C.getPosition());
      debugCuve.write('(');
      if ( this->EV_C.getPosition() == OUVERT) debugCuve.printPGM(FR::TXT_OUVERT);
      else if ( this->EV_C.getPosition() == FERME) debugCuve.printPGM(FR::TXT_FERME);
      debugCuve.println(')');
#endif

      if ( this->EV_F.getMode() != EV_NON_CONFIGURE) { // EV froid
        this->EV_F.etatEV();// actualise la position
        if ( this->EV_C.getMode() != EV_NON_CONFIGURE) tempObj += _1_2_ECART_CHAUD_FROID; // si présence EV froid -> on compare par rapport à une température obj legèrement plus haut (d'1/2 ecart chaud froid)

        if ( this->EV_F.getPosition() == OUVERT && temp <= (tempObj - HISTERESIS_SIMPLE) ) {
          this->EV_F.bougeEV(FERME);
          #ifdef DEBUG
            debugCuve.printPGM(FR::TXT_FERMETURE);
            debugCuve.write(' ');
            debugCuve.printPGM(FR::TXT_EV);
            debugCuve.write(' ');
            debugCuve.printPGM(FR::TXT_FROID);
          #endif
        } else if ( this->EV_F.getPosition() == FERME && temp >= (tempObj + HISTERESIS_SIMPLE) ) {
          this->EV_F.bougeEV(OUVERT);
          #ifdef DEBUG
          debugCuve.printPGM(FR::TXT_OUVERTURE);
          debugCuve.write(' ');
          debugCuve.printPGM(FR::TXT_EV);
          debugCuve.write(' ');
          debugCuve.printPGM(FR::TXT_FROID);
          #endif
        }
      }

      if ( this->EV_C.getMode() != EV_NON_CONFIGURE) { // EV chaud
        this->EV_C.etatEV();// actualise la position
        if ( this->EV_F.getMode() != EV_NON_CONFIGURE) tempObj -= (_1_2_ECART_CHAUD_FROID << 1); // si présence EV chaud -> on compare par rapport à une température obj legèrement plus bas (d'1/2 ecart chaud froid)

        if ( this->EV_C.getPosition() == OUVERT && temp >= (tempObj + HISTERESIS_SIMPLE) ) {
          this->EV_C.bougeEV(FERME);
          #ifdef DEBUG
          debugCuve.printPGM(FR::TXT_FERMETURE);
          debugCuve.write(' ');
          debugCuve.printPGM(FR::TXT_EV);
          debugCuve.write(' ');
          debugCuve.printPGM(FR::TXT_CHAUD);
          #endif
        }else if ( this->EV_C.getPosition() == FERME && temp <= (tempObj - HISTERESIS_SIMPLE) ) {
          this->EV_C.bougeEV(OUVERT);
          #ifdef DEBUG
          debugCuve.printPGM(FR::TXT_OUVERTURE);
          debugCuve.write(' ');
          debugCuve.printPGM(FR::TXT_EV);
          debugCuve.write(' ');
          debugCuve.printPGM(FR::TXT_CHAUD);
          #endif
        }
      }
    }
  }
  return true;
}

void Cuve::print(HardwareSerial& serial){
  signed int temp;
  serial.print("Cuve(");

  serial.print(nom);
  serial.print(",");

  if( tempConsigneCuve<0) {
    serial.print('-');
    temp=-tempConsigneCuve;
  }
  else temp=tempConsigneCuve;

  serial.print(temp>>4);

  temp=temp&0b1111;
  if( temp!=0){
    serial.print('.');
    if( temp==1) serial.print("0625");
    else serial.print((temp)*625);
  }
  serial.print("°,");

  sonde.print(serial);
  serial.print(", EV froid:");
  EV_F.print(serial);
  serial.print(", EV chaud:");
  EV_C.print(serial);
  serial.print(")");
}

void Cuve::print(DebugLogger& debug){
  signed int temp;
  debug.print("Cuve(");

  debug.print(nom);
  debug.print(',');

  if( tempConsigneCuve<0) {
    debug.print('-');
    temp=-tempConsigneCuve;
  }
  else temp=tempConsigneCuve;

  debug.print(temp>>4);

  temp=temp&0b1111;
  if( temp!=0){
    debug.print('.');
    if( temp==1) debug.print("0625");
    else debug.print((temp)*625);
  }
  debug.print("°,");

  sonde.print(debug);
  debug.print(", EV froid:");
  EV_F.print(debug);
  debug.print(", EV chaud:");
  EV_C.print(debug);
  debug.println(')');
}


void Cuve::initEV(){
  this->EV_C.initEV();
  this->EV_F.initEV();
}

Cuve &Cuve::operator=(const Cuve &source){
  if (&source != this){
    this->sonde=source.sonde;
    this->EV_C=source.EV_C;
    this->EV_F=source.EV_F;
    this->tempConsigneCuve=source.tempConsigneCuve;
    this->nom=source.nom;
  }
  return *this;
}

bool Cuve::operator==(const Cuve& cuve) const{
  return (this==&cuve);
}

bool Cuve::operator!=(const Cuve& cuve) const{
  return (this!=&cuve);
}
