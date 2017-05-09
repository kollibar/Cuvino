#include "CuveV2.h"

DebugLogger debugCuveV2=DebugLogger(CUVE_DEBUG_nom);

// --- Cuve --- //

PileErreur* CuveV2::pileErreur=NULL;
Timer* CuveV2::timer=NULL;
OneWire* CuveV2::ds=NULL;
QueueHandle_t CuveV2::qCmdeToCore=NULL;
QueueHandle_t CuveV2::qCmdeToTimer=NULL;

void CuveV2::begin(PileErreur& _pileErreur,Timer& _timer,OneWire& _ds,QueueHandle_t queueCmdeToCore,QueueHandle_t queueCmdeToTimer){
  pileErreur=&_pileErreur;
  timer=&_timer;
  ds=&_ds;
  qCmdeToCore=queueCmdeToCore;
  qCmdeToTimer=queueCmdeToTimer;
  this->EV_F.begin(_pileErreur,_timer,_ds,queueCmdeToCore,queueCmdeToTimer);
  this->sonde->begin(_pileErreur,_timer,queueCmdeToCore,queueCmdeToTimer);
}

uint16_t CuveV2::save(uint16_t addr) {
  //addr = this->sonde->save(addr);
  //addr = ecritEEPROM((byte*)&this->sonde->addr, 8, addr);

  addr=ecritEEPROM((unsigned char*)&this->sonde->num, 1, addr);
  addr=reserveEEPROM(7, addr);
  addr=ecritEEPROM((unsigned char*)&this->sonde->a, 1, addr);
  addr=ecritEEPROM((unsigned char*)&this->sonde->b, 1, addr);

  addr = ecritEEPROM((byte *)&this->tempConsigneCuve, 2, addr);
  EEPROM.write(addr, this->nom); ++addr;
  addr = this->EV_F.save(addr);
  addr = this->EV_C.save(addr);
  return addr;
}

bool CuveV2::save(BlocMem* bloc) {
  //if ( ! this->sonde->save(bloc)) return false;
  //if ( ! bloc->ecrit((byte*)&this->sonde->addr, 8)) return false;

  if ( ! bloc->ecrit((byte*)&this->sonde->num, 1)) return false;
  bloc->reserve(7);
  if ( ! bloc->ecrit((byte*)&this->sonde->a, 1)) return false;
  if ( ! bloc->ecrit((byte*)&this->sonde->b, 1)) return false;

  if ( ! bloc->ecrit((byte*)&this->tempConsigneCuve, 2)) return false;
  if ( ! bloc->ecrit((byte*)&this->nom, 1)) return false;
  if ( ! this->EV_F.save(bloc)) return false;
  if ( ! this->EV_C.save(bloc)) return false;
  return true;
}

uint16_t CuveV2::load(uint16_t addr) {
  this->defaut();
  //addr = this->sonde->load(addr);
  signed char sondeNum;
  addr=litEEPROM((unsigned char*)&sondeNum, 1, addr);
  addr=reserveEEPROM(7, addr);
  addr=litEEPROM((unsigned char*)&this->sonde->a, 1, addr);
  addr=litEEPROM((unsigned char*)&this->sonde->b, 1, addr);

  sonde=Brochage::sondeV2(sondeNum);

  //addr = litEEPROM((byte*)&this->sonde->addr, 8, addr);
  addr = litEEPROM((byte*)&this->tempConsigneCuve, 2, addr);
  this->nom = EEPROM.read(addr); ++addr;
  addr = this->EV_F.load(addr);
  addr = this->EV_C.load(addr);
  return addr;
}

bool CuveV2::load(BlocMem* bloc) {
  //if ( ! this->sonde->load(bloc)) return false;
  //if ( ! bloc->lit((byte*)&this->sonde->addr, 8)) return false;
  signed char sondeNum;
  if ( ! bloc->lit((byte*)&sondeNum, 1)) return false;
  sonde=Brochage::sondeV2(sondeNum);
  bloc->reserve(7);
  if ( ! bloc->lit((byte*)&this->sonde->a, 1)) return false;
  if ( ! bloc->lit((byte*)&this->sonde->b, 1)) return false;

  if ( ! bloc->lit((byte*)&this->tempConsigneCuve, 2)) return false;
  if ( ! bloc->lit((byte*)&this->nom, 1)) return false;
  if ( ! this->EV_F.load(bloc)) return false;
  if ( ! this->EV_C.load(bloc)) return false;
  return true;
}

void CuveV2::defaut(void) {
  this->sonde=NULL;
  this->sondeNum=AUCUNE_SONDE;
  this->tempConsigneCuve = TEMP_LIGNE_NON_CONFIGURE;
  this->nom = ' ';
  this->EV_F.defaut();
  this->EV_C.defaut();
  this->dateMesure=0;
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

bool CuveV2::setArret(signed int c){
  this->tempConsigneCuve=c;
  return true;
}

signed int CuveV2::getConsigne(){
  return this->tempConsigneCuve;
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
  #else
  if( dateMesure != 0 && (millis() - dateMesure) < 15000 ){
  #endif
    return mesure;
  }

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
  signed int  tempObj, temp;

  if( this->tempConsigneCuve == TEMP_MAINTENANCE ) return true;
  if( this->tempConsigneCuve < TEMP_ARRET ) return false;

  if (this->sonde->isSondeTemp()) {
    /*this->sonde->demandeMesureTemp();
    vTaskDelay( (750 / (this->sonde->getPrecision() + 1)) / portTICK_PERIOD_MS);
    temp = this->sonde->getTemperature();*/
    temp=this->getTemperature();
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
    if ( this->EV_F.mode != EV_NON_CONFIGURE && this->EV_F.position != FERME) { // EV froid
      this->EV_F.bougeEV(FERME);
    }
    if ( this->EV_C.mode != EV_NON_CONFIGURE && this->EV_C.position != FERME) { // EV chaud
      this->EV_C.bougeEV(FERME);
    }
    return true;
  }

  tempObj = this->tempConsigneCuve + decalage;

  if (this->tempConsigneCuve <= 2000 && this->tempConsigneCuve >= -880) { // si la température de consigne est comprise dans la plage du capteur
    if (this->sonde->isDS18B20() ) {
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
      debugCuveV2.print("  ");
      debugCuveV2.printPGM(FR::TXT_MODE);
      debugCuveV2.write(':');
      debugCuveV2.printPGM(TXT_EV(this->EV_F.mode));
      debugCuveV2.write('(');
      debugCuveV2.print(this->EV_F.mode,BIN);
      debugCuveV2.write(')');
      debugCuveV2.print(" / ");
      debugCuveV2.printPGM(FR::TXT_POSITION_ACTUELLE);
      debugCuveV2.write(':');
      debugCuveV2.print(this->EV_F.position);
      debugCuveV2.write('(');
      if ( this->EV_F.position == OUVERT) debugCuveV2.printPGM(FR::TXT_OUVERT);
      else if ( this->EV_F.position == FERME) debugCuveV2.printPGM(FR::TXT_FERME);
      debugCuveV2.println(')');

      debugCuveV2.printPGM(FR::TXT_EV);
      debugCuveV2.write(' ');
      debugCuveV2.printPGM(FR::TXT_CHAUD);
      debugCuveV2.print("  ");
      debugCuveV2.printPGM(FR::TXT_MODE);
      debugCuveV2.write(':');
      debugCuveV2.printPGM(TXT_EV(this->EV_C.mode));
      debugCuveV2.write('(');
      debugCuveV2.print(this->EV_C.mode,BIN);
      debugCuveV2.write(')');
      debugCuveV2.print(" / ");
      debugCuveV2.printPGM(FR::TXT_POSITION_ACTUELLE);
      debugCuveV2.write(':');
      debugCuveV2.print(this->EV_C.position);
      debugCuveV2.write('(');
      if ( this->EV_C.position == OUVERT) debugCuveV2.printPGM(FR::TXT_OUVERT);
      else if ( this->EV_C.position == FERME) debugCuveV2.printPGM(FR::TXT_FERME);
      debugCuveV2.println(')');
#endif

      if ( this->EV_F.mode != EV_NON_CONFIGURE) { // EV froid
        this->EV_F.etatEV();// actualise la position
        if ( this->EV_C.mode != EV_NON_CONFIGURE) tempObj += _1_2_ECART_CHAUD_FROID; // si présence EV froid -> on compare par rapport à une température obj legèrement plus haut (d'1/2 ecart chaud froid)

        if ( this->EV_F.position == OUVERT && temp <= (tempObj - HISTERESIS_SIMPLE) ) {
          this->EV_F.bougeEV(FERME);
          #ifdef DEBUG
            debugCuveV2.printPGM(FR::TXT_FERMETURE);
            debugCuveV2.write(' ');
            debugCuveV2.printPGM(FR::TXT_EV);
            debugCuveV2.write(' ');
            debugCuveV2.printPGM(FR::TXT_FROID);
          #endif
        } else if ( this->EV_F.position == FERME && temp >= (tempObj + HISTERESIS_SIMPLE) ) {
          this->EV_F.bougeEV(OUVERT);
          #ifdef DEBUG
          debugCuveV2.printPGM(FR::TXT_OUVERTURE);
          debugCuveV2.write(' ');
          debugCuveV2.printPGM(FR::TXT_EV);
          debugCuveV2.write(' ');
          debugCuveV2.printPGM(FR::TXT_FROID);
          #endif
        }
      }

      if ( this->EV_C.mode != EV_NON_CONFIGURE) { // EV chaud
        this->EV_C.etatEV();// actualise la position
        if ( this->EV_F.mode != EV_NON_CONFIGURE) tempObj -= (_1_2_ECART_CHAUD_FROID << 1); // si présence EV chaud -> on compare par rapport à une température obj legèrement plus bas (d'1/2 ecart chaud froid)

        if ( this->EV_C.position == OUVERT && temp >= (tempObj + HISTERESIS_SIMPLE) ) {
          this->EV_C.bougeEV(FERME);
          #ifdef DEBUG
          debugCuveV2.printPGM(FR::TXT_FERMETURE);
          debugCuveV2.write(' ');
          debugCuveV2.printPGM(FR::TXT_EV);
          debugCuveV2.write(' ');
          debugCuveV2.printPGM(FR::TXT_CHAUD);
          #endif
        }else if ( this->EV_C.position == FERME && temp <= (tempObj - HISTERESIS_SIMPLE) ) {
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
    }
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
