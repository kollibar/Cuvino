#include "CuveV2.h"

DebugLogger debugCuveV2=DebugLogger(CUVE_DEBUG_nom);

// --- Cuve --- //

PileErreur* CuveV2::pileErreur=NULL;
Timer* CuveV2::timer=NULL;
OneWire* CuveV2::ds=NULL;
QueueHandle_t CuveV2::qCmdeToCore=NULL;
QueueHandle_t CuveV2::qCmdeToTimer=NULL;

void CuveV2::begin(PileErreur& _pileErreur,Timer& _timer,OneWire& _ds,QueueHandle_t queueCmdeToCore,QueueHandle_t queueCmdeToTimer, MCP23S17* gpio){
  pileErreur=&_pileErreur;
  timer=&_timer;
  ds=&_ds;
  qCmdeToCore=queueCmdeToCore;
  qCmdeToTimer=queueCmdeToTimer;
  ElectroVanne3::begin(_pileErreur,_timer,queueCmdeToCore,queueCmdeToTimer, gpio);
  SondeV2::begin(_pileErreur,_timer,queueCmdeToCore,queueCmdeToTimer);
}

uint16_t CuveV2::save(uint16_t addr) {
  //addr = this->sonde->save(addr);
  //addr = ecritEEPROM((byte*)&this->sonde->addr, 8, addr);

  // EEPROM.write(addr, this->nom); ++addr;

  // addr=ecritEEPROM((unsigned char*)&this->sonde->num, 1, addr);
  // addr=reserveEEPROM(15, addr);
  // addr=ecritEEPROM((unsigned char*)&this->sonde->a, 1, addr);
  // addr=ecritEEPROM((unsigned char*)&this->sonde->b, 1, addr);

  // addr = ecritEEPROM((byte *)&this->tempConsigneCuve, 2, addr);
  // addr = ecritEEPROM((byte *)&this->tempConsigneMin, 2, addr);

  // EEPROM.write(addr, this->pourcentageActiviteChaud); ++addr;
  // EEPROM.write(addr, this->pourcentageActiviteFroid); ++addr;

  eeprom_update_byte((uint8_t*)addr,this->nom);addr+=1;
  eeprom_update_byte((uint8_t*)addr,this->sonde->num);addr+=1;
  addr+=15;
  eeprom_update_byte((uint8_t*)addr,this->sonde->a);addr+=1;
  eeprom_update_byte((uint8_t*)addr,this->sonde->b);addr+=1;

  eeprom_update_word((uint16_t*)addr,this->tempConsigneCuve);addr+=2;
  eeprom_update_word((uint16_t*)addr,this->tempConsigneMin);addr+=2;

  eeprom_update_byte((uint8_t*)addr,this->pourcentageActiviteChaud);addr+=1;
  eeprom_update_byte((uint8_t*)addr,this->pourcentageActiviteFroid);addr+=1;
  
  addr = this->EV_F.save(addr);
  addr = this->EV_C.save(addr);
  return addr;
}

bool CuveV2::save(BlocMem* bloc) {
  //if ( ! this->sonde->save(bloc)) return false;
  //if ( ! bloc->ecrit((byte*)&this->sonde->addr, 8)) return false;

  if ( ! bloc->ecrit((byte*)&this->nom, 1)) return false;

  if ( ! bloc->ecrit((byte*)&this->sonde->num, 1)) return false;
  bloc->reserve(15);
  if ( ! bloc->ecrit((byte*)&this->sonde->a, 1)) return false;
  if ( ! bloc->ecrit((byte*)&this->sonde->b, 1)) return false;

  if ( ! bloc->ecrit((byte*)&this->tempConsigneCuve, 2)) return false;
  if ( ! bloc->ecrit((byte*)&this->tempConsigneMin, 2)) return false;

  if ( ! bloc->ecrit((byte*)&this->pourcentageActiviteChaud, 1)) return false;
  if ( ! bloc->ecrit((byte*)&this->pourcentageActiviteFroid, 1)) return false;
  
  if ( ! this->EV_F.save(bloc)) return false;
  if ( ! this->EV_C.save(bloc)) return false;
  return true;
}

uint16_t CuveV2::load(uint16_t addr) {
  this->defaut();
  //addr = this->sonde->load(addr);
  signed char sondeNum;


  this->nom = eeprom_read_byte((uint8_t*)addr);addr++;
  sondeNum = eeprom_read_byte((uint8_t*)addr);addr++;
  addr+=15; // reserve
  this->sonde->a=eeprom_read_byte((uint8_t*)addr); addr++;
  this->sonde->b=eeprom_read_byte((uint8_t*)addr); addr++;

  sonde=Brochage::sondeV2(sondeNum);

  this->tempConsigneCuve=eeprom_read_word((uint16_t*)addr); addr+=2;
  this->tempConsigneMin=eeprom_read_word((uint16_t*)addr); addr+=2;

  this->pourcentageActiviteChaud = eeprom_read_byte((uint8_t*)addr);addr++;
  this->pourcentageActiviteFroid = eeprom_read_byte((uint8_t*)addr);addr++;
  
  // this->nom = EEPROM.read(addr); ++addr;
  // addr=litEEPROM((unsigned char*)&sondeNum, 1, addr);
  // addr=reserveEEPROM(15, addr);
  // addr=litEEPROM((unsigned char*)&this->sonde->a, 1, addr);
  // addr=litEEPROM((unsigned char*)&this->sonde->b, 1, addr);

  // sonde=Brochage::sondeV2(sondeNum);

  // //addr = litEEPROM((byte*)&this->sonde->addr, 8, addr);
  // addr = litEEPROM((byte*)&this->tempConsigneCuve, 2, addr);
  // addr = litEEPROM((byte*)&this->tempConsigneMin, 2, addr);
  
  // this->pourcentageActiviteChaud = EEPROM.read(addr); ++addr;
  // this->pourcentageActiviteFroid = EEPROM.read(addr); ++addr;
  
  addr = this->EV_F.load(addr);
  addr = this->EV_C.load(addr);
  return addr;
}

bool CuveV2::load(BlocMem* bloc) {
  //if ( ! this->sonde->load(bloc)) return false;
  //if ( ! bloc->lit((byte*)&this->sonde->addr, 8)) return false;
  signed char sondeNum;
  if ( ! bloc->lit((byte*)&this->nom, 1)) return false;

  if ( ! bloc->lit((byte*)&sondeNum, 1)) return false;
  sonde=Brochage::sondeV2(sondeNum);
  bloc->reserve(15);
  if ( ! bloc->lit((byte*)&this->sonde->a, 1)) return false;
  if ( ! bloc->lit((byte*)&this->sonde->b, 1)) return false;

  if ( ! bloc->lit((byte*)&this->tempConsigneCuve, 2)) return false;
  if ( ! bloc->lit((byte*)&this->tempConsigneMin, 2)) return false;

  if ( ! bloc->lit((byte*)&this->pourcentageActiviteChaud, 1)) return false;
  if ( ! bloc->lit((byte*)&this->pourcentageActiviteFroid, 1)) return false;
  
  if ( ! this->EV_F.load(bloc)) return false;
  if ( ! this->EV_C.load(bloc)) return false;
  return true;
}

void CuveV2::defaut(void) {
  this->sonde=NULL;
  this->sondeNum=AUCUNE_SONDE;
  this->tempConsigneCuve = TEMP_LIGNE_NON_CONFIGURE;
  this->tempConsigneMin = TEMP_LIGNE_NON_CONFIGURE;
  this->nom = ' ';
  this->EV_F.defaut();
  this->EV_C.defaut();
  this->dateMesure=0;

  this->pourcentageActiviteChaud=255;
  this->pourcentageActiviteFroid=255;
}

bool CuveV2::estConfigure() {
  return (tempConsigneCuve != TEMP_LIGNE_NON_CONFIGURE && tempConsigneCuve != TEMP_SONDE_NON_CONFIGURE && (EV_F.estConfigure() || EV_C.estConfigure()));
}

bool CuveV2::hasSondeTemp(){
  return ( this->sonde != NULL && this->sonde->isSondeTemp() );
}

bool CuveV2::setConsigned(signed int c){
  this->tempConsigneCuve=c;
  return true;
}

bool CuveV2::setConsigne(int16_t max, int16_t min){
  this->tempConsigneCuve=max;
  this->tempConsigneMin=min;
}

bool CuveV2::setArret(signed int c){
  this->tempConsigneCuve=c;
  return true;
}

signed int CuveV2::getConsigne(){
  return this->tempConsigneCuve;
}

int16_t CuveV2::getConsigneMax(){
  return this->tempConsigneCuve;
}

int16_t CuveV2::getConsigneMin(){
  return this->tempConsigneMin;
}

unsigned long CuveV2::timeToWait(){
  if( this->sonde == NULL ) return 0;
  else return this->sonde->timeToWait();
}

bool CuveV2::demandeMesureTemp(){
  if( this->sonde == NULL ) return TEMP_ERREUR;
  if( ! this->sonde->isSondeTemp() ) return TEMP_ERREUR;
  return this->sonde->demandeMesureTemp();
}

signed int CuveV2::getTemperature(){
  if( this->sonde == NULL ) return TEMP_ERREUR;
  if( ! this->sonde->isSondeTemp() ) return TEMP_ERREUR;
  if( this->sonde->isDS18B20() ) return sonde->getTemperature();

  #ifdef DEBUG
  if( dateMesure != 0 && (millis() - dateMesure) < 5000 ){
    return mesure;
  }
  #else
  if( dateMesure != 0 && (millis() - dateMesure) < 15000 ){
    return mesure;
  }
  #endif
    

  this->sonde->demandeMesureTemp();

/*
  unsigned long delai=750 / (this->sonde->getPrecision()+1);

  if( xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED ){
    delay(delai); // si le noyau FreeRTOS est inactif
  } else {
    vTaskDelay( delai / portTICK_PERIOD_MS ); // si le noyau FreeRTOS est actif
  }


  mesure=this->sonde->getTemperature();
  */
  mesure=this->sonde->getTemperature();
  dateMesure=millis();

  return mesure;
}

signed int CuveV2::getEcart(){
  return (this->getTemperature() - tempConsigneCuve);
}

bool CuveV2::setPrecisionMesure(unsigned char precision){
  return this->sonde->setPrecision(precision);
}
unsigned char CuveV2::getPrecisionMesure() const{
  return this->sonde->getPrecision();
}

bool CuveV2::controlTemp(int decalage){
  int16_t  tempObj, temp;

  uint16_t s=(uint16_t)(millis()>>14);  // environ des 1/4 de minutes depuis réinit

  if( this->tempConsigneCuve == TEMP_MAINTENANCE ) return true;
  if( this->tempConsigneCuve < TEMP_ARRET ) return false;

  if (this->sonde->isSondeTemp()) {
    /*this->sonde->demandeMesureTemp();
    vTaskDelay( (750 / (this->sonde->getPrecision() + 1)) / portTICK_PERIOD_MS);
    temp = this->sonde->getTemperature();*/
    temp=this->getTemperature();
    if( temp == TEMP_ERREUR_COMMUNICATION_SONDE ){ // erreur de communication avec la sonde
      // A FAIRE
      return false;
    }
    if( temp < (-20*16) || temp > (100*16) ){ // si temp n'est pas dans la plage -20° - 100°C => erreur de communication vraisemblable
      temp=this->getTemperature(); // on réessaye une 2e fois

      // A FAIRE si plus de 2 erreur de lecture de température..
      if( temp < (-20*16) || temp > (100*16) ) return false;
    }

    #ifdef DEBUG
    this->sonde->print(debugCuveV2);
    #endif
  }
  else {
    if (this->sonde->addr[0] == 0) temp = TEMP_ERREUR_ABS_SONDE;
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
    if (this->sonde->isSondeOK() ) {
      //contrôle de la ligne en interronpant le froid
#ifdef DEBUG
      debugCuveV2.printPGM(CUVE_DEBUG_LIGNE);
      debugCuveV2.println((char)this->nom);
      debugCuveV2.printPGM(FR::TXT_TEMPERATURE);
      debugCuveV2.write(' ');
      debugCuveV2.printPGM(FR::TXT_OBJECTIF);
      debugCuveV2.write(':');
      debugCuveV2.print(tempObj >> 4, DEC);
      debugCuveV2.write(' ');
      debugCuveV2.printPGM(FR::TXT_MESUREE);
      debugCuveV2.write(':');
      debugCuveV2.println(temp >> 4, DEC);

      debugCuveV2.printPGM(FR::TXT_EV);
      debugCuveV2.write(' ');
      debugCuveV2.printPGM(FR::TXT_FROID);
      debugCuveV2.print(' '');
      debugCuveV2.printPGM(FR::TXT_MODE);
      debugCuveV2.write(':');
      debugCuveV2.printPGM(TXT_EV(this->EV_F.getMode()));
      debugCuveV2.write('(');
      debugCuveV2.print(this->EV_F.getMode(),BIN);
      debugCuveV2.write(')');
      debugCuveV2.print(F(" / "));
      debugCuveV2.printPGM(FR::TXT_POSITION_ACTUELLE);
      debugCuveV2.write(':');
      debugCuveV2.print(this->EV_F.getPosition());
      debugCuveV2.write('(');
      if ( this->EV_F.getPosition() == OUVERT) debugCuveV2.printPGM(FR::TXT_OUVERT);
      else if ( this->EV_F.getPosition() == FERME) debugCuveV2.printPGM(FR::TXT_FERME);
      debugCuveV2.println(')');

      debugCuveV2.printPGM(FR::TXT_EV);
      debugCuveV2.write(' ');
      debugCuveV2.printPGM(FR::TXT_CHAUD);
      debugCuveV2.print('  ');
      debugCuveV2.printPGM(FR::TXT_MODE);
      debugCuveV2.write(':');
      debugCuveV2.printPGM(TXT_EV(this->EV_C.getMode()));
      debugCuveV2.write('(');
      debugCuveV2.print(this->EV_C.getMode(),BIN);
      debugCuveV2.write(')');
      debugCuveV2.print(F(" / "));
      debugCuveV2.printPGM(FR::TXT_POSITION_ACTUELLE);
      debugCuveV2.write(':');
      debugCuveV2.print(this->EV_C.getPosition());
      debugCuveV2.write('(');
      if ( this->EV_C.getPosition() == OUVERT) debugCuveV2.printPGM(FR::TXT_OUVERT);
      else if ( this->EV_C.getPosition() == FERME) debugCuveV2.printPGM(FR::TXT_FERME);
      debugCuveV2.println(')');
#endif

      // actualisation des positions
      if ( this->EV_F.OK() ) this->EV_F.etatEV();
      if ( this->EV_C.OK() ) this->EV_C.etatEV();

        if( this->EV_C.OK() && this->EV_F.OK() ) { // EV Chaud et EV Froid OK
          #ifdef DEBUG
          debugCuveV2.println(F("EV Froid et EV Chaud"));
          #endif

          if( this->tempConsigneMin < -32000  ){  // si une seule température configurée

            if( this->EV_F.getPosition() == OUVERT){ // si EV froid OUVERT
              if( temp <= (tempObj - HISTERESIS_SIMPLE + _1_2_ECART_CHAUD_FROID )){
                this->EV_F.bougeEV(FERME);
                #ifdef DEBUG
                  debugCuveV2.printPGM(FR::TXT_FERMETURE);
                  debugCuveV2.write(' ');
                  debugCuveV2.printPGM(FR::TXT_EV);
                  debugCuveV2.write(' ');
                  debugCuveV2.printPGM(FR::TXT_FROID);
                #endif
              }
            } else if( this->EV_C.getPosition() == OUVERT ){ // si EV Chaud OUVERT
              if( temp > (tempObj + HISTERESIS_SIMPLE - _1_2_ECART_CHAUD_FROID )){
                this->EV_C.bougeEV(FERME);
                #ifdef DEBUG
                  debugCuveV2.printPGM(FR::TXT_FERMETURE);
                  debugCuveV2.write(' ');
                  debugCuveV2.printPGM(FR::TXT_EV);
                  debugCuveV2.write(' ');
                  debugCuveV2.printPGM(FR::TXT_CHAUD);
                #endif
              }
            } else { // EV Froid  et EV chaud FERME
              if( temp > (tempObj + HISTERESIS_SIMPLE + _1_2_ECART_CHAUD_FROID )){
                this->EV_F.bougeEV(OUVERT);
                #ifdef DEBUG
                  debugCuveV2.printPGM(FR::TXT_OUVERTURE);
                  debugCuveV2.write(' ');
                  debugCuveV2.printPGM(FR::TXT_EV);
                  debugCuveV2.write(' ');
                  debugCuveV2.printPGM(FR::TXT_FROID);
                #endif
              } else if( temp < (tempObj - HISTERESIS_SIMPLE - _1_2_ECART_CHAUD_FROID )){
                this->EV_C.bougeEV(OUVERT);
                #ifdef DEBUG
                  debugCuveV2.printPGM(FR::TXT_OUVERTURE);
                  debugCuveV2.write(' ');
                  debugCuveV2.printPGM(FR::TXT_EV);
                  debugCuveV2.write(' ');
                  debugCuveV2.printPGM(FR::TXT_CHAUD);
                #endif
              }
            }
          } else {
            if( temp < this->tempConsigneMin + decalage ){
              this->EV_F.bougeEV(FERME);
              this->EV_C.bougeEV(OUVERT);
            } else if( temp > this->tempConsigneCuve + decalage ){
              this->EV_F.bougeEV(OUVERT);
              this->EV_C.bougeEV(FERME);
            }
          }

        } else if( this->EV_F.OK() ){ // EV Froid OK mais pas d'EV chaud
        #ifdef DEBUG
        debugCuveV2.println(F("EV Froid seule"));
        #endif
          if ( this->EV_F.getPosition() == OUVERT && temp <= (tempObj - HISTERESIS_SIMPLE) ) {
            this->EV_F.bougeEV(FERME);
            #ifdef DEBUG
              debugCuveV2.printPGM(FR::TXT_FERMETURE);
              debugCuveV2.write(' ');
              debugCuveV2.printPGM(FR::TXT_EV);
              debugCuveV2.write(' ');
              debugCuveV2.printPGM(FR::TXT_FROID);
            #endif
          } else if ( this->EV_F.getPosition() == FERME && temp >= (tempObj + HISTERESIS_SIMPLE) ) {
            this->EV_F.bougeEV(OUVERT);
            #ifdef DEBUG
            debugCuveV2.printPGM(FR::TXT_OUVERTURE);
            debugCuveV2.write(' ');
            debugCuveV2.printPGM(FR::TXT_EV);
            debugCuveV2.write(' ');
            debugCuveV2.printPGM(FR::TXT_FROID);
            #endif
          }
        } else if( this->EV_C.OK()) { // EV Chaud OK mais pas d'EV Froid

        if( this->tempConsigneMin > -32000 ){
          tempObj = this->tempConsigneMin + decalage;
        }

        #ifdef DEBUG
        debugCuveV2.println(F("EV Chaud seule"));
        #endif

        if ( this->EV_C.getPosition() == OUVERT && temp >= (tempObj + HISTERESIS_SIMPLE) ) {
          this->EV_C.bougeEV(FERME);
          #ifdef DEBUG
          debugCuveV2.printPGM(FR::TXT_FERMETURE);
          debugCuveV2.write(' ');
          debugCuveV2.printPGM(FR::TXT_EV);
          debugCuveV2.write(' ');
          debugCuveV2.printPGM(FR::TXT_CHAUD);
          #endif
        }else if ( this->EV_C.getPosition() == FERME && temp <= (tempObj - HISTERESIS_SIMPLE) ) {
          this->EV_C.bougeEV(OUVERT);
          #ifdef DEBUG
          debugCuveV2.printPGM(FR::TXT_OUVERTURE);
          debugCuveV2.write(' ');
          debugCuveV2.printPGM(FR::TXT_EV);
          debugCuveV2.write(' ');
          debugCuveV2.printPGM(FR::TXT_CHAUD);
          #endif
        }

        } else { // ni EV Froid, ni EV Chaud
          #ifdef DEBUG
            debugCuveV2.println(F("aucune EV fonctionnelles"));
          #endif
        }

    } // isSondeOK
  }
  return true;
}

void CuveV2::print(HardwareSerial& serial){
  signed int temp;
  serial.print(F("Cuve("));

  serial.print(nom);
  serial.print(',');

  if( tempConsigneCuve<0) {
    serial.print('-');
    temp=-tempConsigneCuve;
  }
  else temp=tempConsigneCuve;

  serial.print(temp>>4);

  temp=temp&0b1111;
  if( temp!=0){
    serial.print('.');
    if( temp==1) serial.print(F("0625"));
    else serial.print((temp)*625);
  }
  serial.print(F("°,"));

  serial.print(F("Sonde "));
  serial.print(this->sonde->num);
  serial.print(' ');
  serial.print('@');
  serial.println((unsigned int)this->sonde);

  sonde->print(serial);
  serial.print(F(", EV froid:"));
  EV_F.print(serial);
  serial.print(F(", EV chaud:"));
  EV_C.print(serial);
  serial.print(')');
}

void CuveV2::print(DebugLogger& debug){
  signed int temp;
  debug.print(F("Cuve("));

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
    if( temp==1) debug.print(F("0625"));
    else debug.print((temp)*625);
  }
  debug.print(F("°,"));

  debug.printPGM(FR::TXT_SONDE);
  debug.print(' ');
  debug.print(this->sonde->num);
  debug.print(' ');
  debug.print('@');
  debug.println((unsigned int)this->sonde);

  sonde->print(debug);
  debug.print(F(", EV froid:"));
  EV_F.print(debug);
  debug.print(F(", EV chaud:"));
  EV_C.print(debug);
  debug.println(')');
}


void CuveV2::initEV(){
  this->EV_C.initEV();
  this->EV_F.initEV();
}

CuveV2 &CuveV2::operator=(const CuveV2 &source){
  if (&source != this){
    this->sonde=source.sonde;
    this->EV_C=source.EV_C;
    this->EV_F=source.EV_F;
    this->tempConsigneCuve=source.tempConsigneCuve;
    this->nom=source.nom;
  }
  return *this;
}

bool CuveV2::operator==(const CuveV2& cuve) const{
  return (this==&cuve);
}

bool CuveV2::operator!=(const CuveV2& cuve) const{
  return (this!=&cuve);
}
