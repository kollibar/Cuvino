#include "EV3.h"

DebugLogger debugEV3=DebugLogger(EV3_DEBUG_nom);


unsigned char sizeListeEV(){
  unsigned char n=0;
  #ifdef CR01
    ++n;
  #endif
  #ifdef CR01modif
    ++n;
  #endif
  #ifdef CR05
    ++n;
  #endif
  #ifdef CR05_A
    ++n;
  #endif
  #ifdef CR01_MO
    ++n;
  #endif
  #ifdef CR05_MO
    ++n;
  #endif
  #ifdef CR01_MF
    ++n;
  #endif
  #ifdef CR05_MF
    ++n;
  #endif
  #ifdef CR01_M
    ++n;
  #endif
  #ifdef CR05_M
    ++n;
  #endif
  #ifdef EV_RELAI
    ++n;
  #endif
  return n;
}

unsigned char EVinListe(unsigned char num){
  unsigned char n=0;
  #ifdef CR01    
    if( num == n) return CR05;
    ++n;
  #endif
  #ifdef CR05_A
    if( num == n) return CR05_A;
    ++n;
  #endif
  #ifdef CR01_MO
    if( num == n) return CR01_MO;
    ++n;
  #endif
  #ifdef CR05_MO
    if( num == n) return CR05_MO;
    ++n;
  #endif
  #ifdef CR01_MF
    if( num == n) return CR01_MF;
    ++n;
  #endif
  #ifdef CR05_MF
    if( num == n) return CR05_MF;
    ++n;
  #endif
  #ifdef CR01_M
    if( num == n) return CR01_M;
    ++n;
  #endif
  #ifdef CR05_M
    if( num == n) return CR05_M;
    ++n;
  #endif
  #ifdef EV_RELAI
    if( num == n) return EV_RELAI;
    ++n;
  #endif
  return 0;
}

const char* TXT_EV(unsigned char num){
  switch (num) {
    case EV_NON_CONFIGURE:
      return FR::TXT_NON_CONFIGURE;
  #ifdef CR01
    case CR01:
      return FR::TXT_CR01;
  #endif
  #ifdef CR01modif
    case CR01modif:
      return FR::TXT_CR01modif;
  #endif
  #ifdef CR05
    case CR05:
      return FR::TXT_CR05;
  #endif
  #ifdef CR05_A
    case CR05_A:
      return FR::TXT_CR05_A;
  #endif
  #ifdef CR01_MO
    case CR01_MO:
      return FR::TXT_CR01_MO;
  #endif
  #ifdef CR05_MO
    case CR05_MO:
      return FR::TXT_CR05_MO;
  #endif
  #ifdef CR01_MF
  case CR01_MF:
    return FR::TXT_CR01_MF;
  #endif
  #ifdef CR05_MF
  case CR05_MF:
    return FR::TXT_CR05_MF;
  #endif
  #ifdef CR01_M
    case CR01_M:
      return FR::TXT_CR01_M;
  #endif
  #ifdef CR05_M
    case CR05_M:
      return FR::TXT_CR05_M;
  #endif
  #ifdef EV_RELAI
    case EV_RELAI:
      return FR::TXT_EV_RELAI;
  #endif
    default:
      return FR::TXT_ERREUR;
  }
}

bool EV_OK(unsigned char num){
  switch (num) {
    case EV_NON_CONFIGURE:
      return false;
  #ifdef CR01
    case CR01:
  #endif
  #ifdef CR01modif
    case CR01modif:
  #endif
  #ifdef CR05
    case CR05:
  #endif
  #ifdef CR05_A
    case CR05_A:
  #endif
  #ifdef CR01_MO
    case CR01_MO:
  #endif
  #ifdef CR05_MO
    case CR05_MO:
  #endif
  #ifdef CR01_MF
    case CR01_MF:
  #endif
  #ifdef CR05_MF
    case CR05_MF:
  #endif
  #ifdef CR01_M
    case CR01_M:
  #endif
  #ifdef CR05_M
    case CR05_M:
  #endif
  #ifdef EV_RELAI
    case EV_RELAI:
  #endif
      return true;
    default:
      return false;
  }
}


ISR(PCINT0_vect){ // interruption sur PCINT0:7 
    // dans ce cas, activé sur PB4 et PB5 uniquement 
    // PB4 étant INTB du mcp23s17 et PB5, INTA

    // A FAIRE
}

uint8_t ElectroVanne3::numModuleErr=0;
PileErreur* ElectroVanne3::pileErreur=NULL;
Timer* ElectroVanne3::timer=NULL;
QueueHandle_t ElectroVanne3::qCmdeToCore=NULL;
QueueHandle_t ElectroVanne3::qCmdeToTimer=NULL;
ElectroVanne3* ElectroVanne3::listeEV[16]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
MCP23S17* ElectroVanne3::gpio=NULL;

bool ElectroVanne3::OK() const{
   return EV_OK(this->mode);
}

void ElectroVanne3::begin(PileErreur& _pileErreur,Timer& _timer, QueueHandle_t queueCmdeToCore,QueueHandle_t queueCmdeToTimer, MCP23S17* _gpio){
  pileErreur=&_pileErreur;
  timer=&_timer;
  qCmdeToCore=queueCmdeToCore;
  qCmdeToTimer=queueCmdeToTimer;
  if( numModuleErr==0) numModuleErr=_pileErreur.getNumModuleSuiv();
  Serial.println(F("Initialisation MCP23S17"));
  ElectroVanne3::gpio=_gpio;


  // initialise les interruptions sur le atmega2560
  DDRB &= ~(0b00110000); // met les pins PB4 et PB5 en entré (0 => input, 1 => output)
  PORTB |= (0b00110000); // met les pins PB4 et PB5 en pullup (1=> pullup, 0=> pas de pullup)

  PCMSK0 |= (0b00110000);    // active les interruptions sur les pins PB4(PCINT4) et PB5 (PCINT5)
  PCICR |= PCIE0; // active les interruptions sur le port B (PCINT0:7)
}

void ElectroVanne3::print(HardwareSerial& serial){
  serial.print(F("ElectroVanne3(num:"));

  serial.print(numero);
  serial.print(F(", mode:"));
  serial.print(mode);

  serial.print(')');
}

void ElectroVanne3::print(DebugLogger& debug){
  debug.printPGM(FR::TXT_ELECTROVANNE,true);
  debug.print(F("2(num:"));

  debug.print(numero);
  debug.print(F(", "));
  debug.printPGM(FR::TXT_MODE);
  debug.print(':');
  debug.print(mode);

  debug.print(')');
}

ElectroVanne3& ElectroVanne3::operator=(const ElectroVanne3 &source){
  if (&source != this){
    this->numero=source.numero;
    this->mode=source.mode;

    this->position=source.position;
    this->tempsOuvertureVanne=source.tempsOuvertureVanne;

    this->mvt_date=source.mvt_date;
    this->mvt_pos_deb=source.mvt_pos_deb;
    this->mvt_pos_fin=source.mvt_pos_fin;
    this->mvt_timerID=source.mvt_timerID;
    this->mvt_futur=source.mvt_futur;
  }
  return *this;
}

uint16_t ElectroVanne3::save(uint16_t addr) {
  eeprom_update_byte((uint8_t*)addr, this->numero);addr+=1;
  eeprom_update_byte((uint8_t*)addr, this->mode);addr+=1;
  eeprom_update_word((uint16_t*)addr, this->tempsOuvertureVanne);addr+=2;
  // EEPROM.write(addr, this->numero); ++addr;
  // EEPROM.write(addr, this->mode); ++addr;
  // addr = ecritEEPROM((uint8_t *)&this->tempsOuvertureVanne, 2, addr);
  return addr;
}

bool ElectroVanne3::save(BlocMem* bloc) {
  if ( ! bloc->ecrit((uint8_t*)&this->numero, 1)) return false;
  if ( ! bloc->ecrit((uint8_t*)&this->mode, 1)) return false;
  if ( ! bloc->ecrit((uint8_t*)&this->tempsOuvertureVanne, 2)) return false;
  return true;
}

bool ElectroVanne3::load(BlocMem* bloc) {
  if ( ! bloc->lit((uint8_t*)&this->numero, 1)) return false;
  if ( ! bloc->lit((uint8_t*)&this->mode, 1)) return false;
  if ( ! bloc->lit((uint8_t*)&this->tempsOuvertureVanne, 2)) return false;
  return true;
}

uint16_t ElectroVanne3::load(uint16_t addr) {
  this->defaut();
  // this->numero = EEPROM.read(addr); ++addr;
  // this->mode = EEPROM.read(addr); ++addr;
  // addr = litEEPROM((uint8_t *)&this->tempsOuvertureVanne, 2, addr);
  this->numero = eeprom_read_byte((uint8_t*)addr);addr+=1;
  this->mode = eeprom_read_byte((uint8_t*)addr);addr+=1;
  this->tempsOuvertureVanne = eeprom_read_word((uint16_t*)addr);addr+=1;
  return addr;
}

void ElectroVanne3::defaut(void) {
  this->tempsOuvertureVanne = 0;
  this->position = 100;
  this->mode = EV_NON_CONFIGURE;
  this->setNumero(127);

  this->brocheEV=0;

  this->mvt_timerID = 0;
  this->mvt_futur = 0;
}

bool ElectroVanne3::estConfigure() const {
  return (this->mode != EV_NON_CONFIGURE && this->numero != 127);
}

uint8_t ElectroVanne3::getMasqCmde(int8_t numero, uint8_t mode, int8_t type){
  uint8_t masq;

  debugEV3.print(F("GetMasq "));
  debugEV3.println(type);
  
  if( mode & EV_CMD_2_FILS) { // si commande sur 2 fils
    switch(type){
      case EV3_TYPE_MASQ_TOUT_PIN:
        masq = 0b11;
        break;
      case EV3_TYPE_MASQ_PIN_OUVERTURE:
        masq = 0b01;
        break;
      case EV3_TYPE_MASQ_PIN_FERMETURE:
        masq = 0b10;
        break;
      default:
        return 0x00;
    }
    masq=((masq)<<(((numero - 1)%4)*2));
    debugEV3.printPGM(EV3_DEBUG_FLECHE);
    debugEV3.println(masq,BIN);
  } else { // commande sur 1 fils
    if( ( type == EV3_TYPE_MASQ_TOUT_PIN )
      || ( mode & MAINTIENT_OUVETURE && EV3_TYPE_MASQ_PIN_OUVERTURE )
      || ( mode & MAINTIENT_FERMETURE && EV3_TYPE_MASQ_PIN_FERMETURE )
    ){
      if( numero < 0) masq=( 0b10 << ((( (-numero - 1) )%4)*2));
      else masq = ( 0b01 << ((( (numero - 1) )%4)*2));
    } else {
      masq = 0;
    }
  }
  return masq;
}

uint16_t ElectroVanne3::getMasqEtat(int8_t numero, uint8_t mode){
  uint16_t masq;
  if( mode & EV_ETAT_2_FILS) { // si etat EV sur 2 fils
    Serial.println(F("Etat sur 2 fils"));

    // gestions de pisnde l'EV sur le mcp23s17
    masq=(0b11)<<((abs(numero) - 1)*2);  // masq des pins à paramétrer (uint16_t pour parametrage des 2 ports en même temps)

  } else if( mode & EV_ETAT_1_FILS ){
    /* non supporté */
    masq = 0;
  } else if( mode & EV_ETAT_ANALOGIQUE ){
    /* non supporté */
    masq = 0;
  }
  return masq;
}



void ElectroVanne3::initBroches(){
  ElectroVanne3::initBroches(this->numero, this->mode);
}
void ElectroVanne3::relacheBroches(){
  ElectroVanne3::relacheBroches(this->numero, this->mode);
}

void ElectroVanne3::relacheBroches(int8_t numero, uint8_t mode){
  if( mode == EV_NON_CONFIGURE || numero == 127 ) return;

  uint8_t masq=ElectroVanne3::getMasqCmde(numero, mode);
  if(  numero  <= 4 && numero >= -4 ){
    PORTH &= ~masq; // mise à 0 des broches correspondante
    DDRH &= ~masq; // mise en input des broches
  } else {
    PORTC &= ~masq;
    DDRC &= ~masq; // mise en input des broches
  }

  uint16_t masq16=ElectroVanne3::getMasqEtat(numero, mode);
  ElectroVanne3::gpio->writeRegWord(MCP23S17_GPINTEN, ElectroVanne3::gpio->readRegWord(MCP23S17_GPINTEN) & ~masq16 ); // désactive les interruptions (càd à 0 dans le registre GPINTEN)
  ElectroVanne3::gpio->writeRegWord(MCP23S17_GPPU, ElectroVanne3::gpio->readRegWord(MCP23S17_GPPU) & ~masq16 ); // enlève les pullup
  ElectroVanne3::gpio->writeRegWord(MCP23S17_IPOL, ElectroVanne3::gpio->readRegWord(MCP23S17_IPOL) & ~masq16 ); // remet la polarité à normal (càd à 0 dans le registre IPOL)
}

void ElectroVanne3::initBroches(int8_t numero, uint8_t mode) {
  Serial.println(F("InitBroche"));
  if( mode == EV_NON_CONFIGURE) return;

  uint8_t masq=ElectroVanne3::getMasqCmde(numero, mode);
  
  if(  numero  <= 4 && numero >= -4 ){
    DDRH |= masq; // mise en output des broches
    PORTH &= ~masq; // mise à 0 des broches correspondante
  } else {
    DDRC |= masq; // mise en output des broches
    PORTC &= ~masq;
  }

  if( mode & EV_ETAT_2_FILS) { // si etat EV sur 2 fils
    Serial.println(F("Etat sur 2 fils"));

    // gestions de pisnde l'EV sur le mcp23s17
    uint16_t masq=ElectroVanne3::getMasqEtat(numero, mode);

    ElectroVanne3::gpio->writeRegWord(MCP23S17_IODIR, ElectroVanne3::gpio->readRegWord(MCP23S17_IODIR) | masq ); // met les pins de l'EV en input (càd à 1 dans le registre IODIR)
    ElectroVanne3::gpio->writeRegWord(MCP23S17_GPPU, ElectroVanne3::gpio->readRegWord(MCP23S17_GPPU) | masq ); // met les pins de l'EV en pullup (càd à 1 dans le registre GPPU)
    ElectroVanne3::gpio->writeRegWord(MCP23S17_IPOL, ElectroVanne3::gpio->readRegWord(MCP23S17_IPOL) | masq ); // inverse la polarité (càd à 1 dans le registre IPOL)
    ElectroVanne3::gpio->writeRegWord(MCP23S17_GPINTEN, ElectroVanne3::gpio->readRegWord(MCP23S17_GPINTEN) | masq ); // active les interruptions (càd à 1 dans le registre GPINTEN)

    // a voir si on garde comme ça ?
    ElectroVanne3::gpio->writeRegWord(MCP23S17_DEFVAL, ElectroVanne3::gpio->readRegWord(MCP23S17_DEFVAL) | masq ); // valeur par défaut de déclenchement d'interruption
    ElectroVanne3::gpio->writeRegWord(MCP23S17_INTCON, ElectroVanne3::gpio->readRegWord(MCP23S17_INTCON) | masq ); // interruption sur le registre DEFVAL (sur valeur par défaut défini dans le registre defval et non sur toiut état)
    
  } else if( mode & EV_ETAT_1_FILS ){
    /* non supporté */
  } else if( mode & EV_ETAT_ANALOGIQUE ){
    /* non supporté */
    
  }
}

ElectroVanne3::ElectroVanne3(int8_t numero, uint8_t mode){
  if( numero != 127 ) this->setNumero(numero);
  if( mode != EV_NON_CONFIGURE ) this->setMode(mode);
}
ElectroVanne3::~ElectroVanne3(){
  if( this->numero != 127 ) ElectroVanne3::deleteEVAt(this);
}

bool ElectroVanne3::emplacementEVlibre(int8_t numero, bool cmde_2fils){
  if( cmde_2fils ){ // cas cmde 2 fils
    if( numero < 0 || ElectroVanne3::getEVAt(numero) != NULL || ElectroVanne3::getEVAt(-numero) != NULL ) return false;
    else return true;
  } else {  // cas cmde 1 fils
    if( ElectroVanne3::getEVAt(numero) != NULL || 
      ( numero < 0 && ElectroVanne3::getEVAt(-numero) != NULL && (ElectroVanne3::getEVAt(-numero)->mode & EV_CMD_2_FILS ) )
    ) return false;
    else return true;
  }
}

ElectroVanne3* ElectroVanne3::getEVAt(int8_t numero){
  if( numero > 8 || numero < -8 || numero == 0 ) return NULL;
  return listeEV[ElectroVanne3::getIndex(numero)];
}

bool ElectroVanne3::setEVAt(ElectroVanne3* EV){
  if( EV->numero > 8 || EV->numero < -8 || EV->numero == 0 ) return false;
  listeEV[ElectroVanne3::getIndex(EV->numero)]=EV;
  return true;
}

uint8_t ElectroVanne3::getIndex(int8_t num){
  return ( num > 0 )?(num-1):(-num+7);
}

void ElectroVanne3::deleteEVAt(ElectroVanne3* EV){
  if( EV->numero == 127 ) return;

  listeEV[ElectroVanne3::getIndex(EV->numero)]=NULL;
}

int8_t ElectroVanne3::getPosition(){
  return this->position;
}

uint8_t ElectroVanne3::getMode(){
  return this->mode;
}
uint8_t ElectroVanne3::getNumero(){
  return this->numero;
}

int8_t ElectroVanne3::getMvt_pos_deb(){
  return this->mvt_pos_deb;
}
int8_t ElectroVanne3::getMvt_pos_fin(){
  return this->mvt_pos_fin;
}
erreurEV ElectroVanne3::setNumero(int8_t numero, bool force){
  if( numero == this->numero ) return ERREUR_EV_OK;

  if( numero == 127){
    this->relacheBroches();
    ElectroVanne3::deleteEVAt(this);
    return ERREUR_EV_OK;
  }

  if(  numero > 8 || numero < -8 || numero == 0 ) { // numéro hors de la plage -8,8 (excepté 0)
    this->setNumero(127); // impossible
    return ERREUR_EV_NUMERO_INCOMPATIBLE;
  }
  
  if( numero < 0 && (mode & EV_CMD_2_FILS )){ // commande 2 fils et numéro négatif
    this->setNumero(127); // impossible
    return ERREUR_EV_NUMERO_NEGATIF_INCOMPATIBLE_MODE_CMD_2_FILS;
  }

  if( ! force && ! ElectroVanne3::emplacementEVlibre(numero, mode & EV_CMD_2_FILS ) ) {  // electrovanne deja existante sur ce/ces pin(s)
      this->setNumero(127); // impossible
      return ERREUR_EV_DEJA_CONFIGURE_A_CE_NUMERO;
  }

  if( this->numero != 127 ) {
    ElectroVanne3::deleteEVAt(this);
    this->relacheBroches();
  }

  this->numero=numero;
  ElectroVanne3::setEVAt(this);
  
  return ERREUR_EV_OK;
}

void ElectroVanne3::initEV() {
  if ( mode == EV_NON_CONFIGURE) return;

  this->initBroches();
  if ( mode != EV_NON_CONFIGURE) {
#ifdef DEBUG_EV3
    debugEV3.printPGM(EV3_DEBUG_FERMETURE);
    debugEV3.write(' ');
    debugEV3.printPGM(EV3_DEBUG_electrovanne);
    debugEV3.write(' ');
    debugEV3.println(numero);
#endif
    this->bougeEV(FERME);
  }
}

/**
 * Met le mode de l'EV au mode données
 * @param uint8_t mode
 * 
 * @return erreurEV code d'erreur de retour
 * 
*/
erreurEV ElectroVanne3::setMode(uint8_t mode){

  if( this->numero != 127 ){
    if( ! ElectroVanne3::emplacementEVlibre(numero, mode & EV_CMD_2_FILS ) ){
      return ERREUR_EV_DEJA_CONFIGURE_A_CE_NUMERO;
    }

    this->relacheBroches();
  }
  
  switch(mode){
    case CR05:
    case CR01modif:
    case CR01:
    case CR01_MO:
    case CR05_MO:
    case CR01_MF:
    case CR05_MF:
    case CR01_M:
    case CR05_M:
    case EV_RELAI_OUVRANT:
    case EV_RELAI_FERMANT:

      if( numero < 0 && (mode & EV_CMD_2_FILS )){ // numéro négatif et commande 2 fils => incompatible
        this->mode=EV_NON_CONFIGURE;
        return ERREUR_EV_NUMERO_INCOMPATIBLE;
      }
      // tout bon, on enregistre le mode
      this->mode=mode;
      return ERREUR_EV_OK;
    default:  // mode inconnue ou non supporté
      this->mode=EV_NON_CONFIGURE;
      if( mode &  EV_ETAT_ANALOGIQUE ) return ERREUR_TYPE_EV_NON_SUPPORTEE;
      else return ERREUR_EV_INCONNUE;
  }
}

erreurEV ElectroVanne3::setTempsOuverture(uint16_t tps){
  this->tempsOuvertureVanne = tps;
  return ERREUR_EV_OK;
}

uint16_t ElectroVanne3::getTempsOuverture(){
  return this->tempsOuvertureVanne;
}

int8_t ElectroVanne3::etatEV() {
  return ElectroVanne3::etatEV(this->numero, this->mode);
}

int8_t ElectroVanne3::etatEV(int8_t numero, uint8_t mode){
  uint8_t position;
  #ifdef DEBUG_EV3
    debugEV3.printlnPGM(EV3_DEBUG_etatEV);
  #endif
  if( mode & EV_ETAT_2_FILS ){

    numero=abs(numero) - 1;
    // lit l'état des pin de position
    uint8_t r = (uint8_t) (( ElectroVanne3::gpio->readRegWord(MCP23S17_GPIO) & ((0b11)<<(numero*2)) ) >> (numero*2) );


    #ifdef DEBUG_EV3
      debugEV3.print(ElectroVanne3::gpio->readRegWord(MCP23S17_GPIO), BIN);
      debugEV3.printPGM(EV3_DEBUG_FLECHE);
      debugEV3.print(r,BIN);

      debugEV3.print('(');


      if( r == 0b01 ) debugEV3.printPGM(EV3_DEBUG_OUVERT);
      else if( r == 0b10 ) debugEV3.printPGM(EV3_DEBUG_FERME);
      else debugEV3.printPGM(EV3_DEBUG_PARTIEL);
      debugEV3.println(')');

      debugEV3.setWriteMode(modeDebug);
    #endif

    switch(r){
      case 0b01:
        position = OUVERT;
        break;
      case 0b10:
        position = FERME;
        break;
      case 0:
        position = 0;
        break;
      case 0b11:
        position = POSITION_ERREUR;
    }

    return position;
  }
  /*  non encore supporté *
  else if( (mode & EV_ETAT_ANALOGIQUE) != 0){
    if(digitalRead(brocheEtatEV) == LOW) return 0;
    uint16_t val = analogRead(brocheEtatEV);
    if ( val < 500) position=0;
    else if ( val < 850 ) position=FERME;
    else position=OUVERT;

    return position;
  }
  else if( mode & EV_ETAT_1_FILS ){
    // A FAIRE
  }
  else if( mode & EV_ETAT_SUR_CMD ){

    // A FAIRE
  }*/
  return POSITION_ERREUR;
}


bool ElectroVanne3::ouvre(){
  return bougeEV(OUVERT);
}

bool ElectroVanne3::ferme(){
  return bougeEV(FERME);
}

bool ElectroVanne3::bougeEV(int8_t mvt) {
  if( ! this->estConfigure() ) return false;
  return bougeEVtotal(mvt);
#ifdef DEBUG_EV3
  debugEV3.printPGM(EV3_DEBUG_bougeEV);
  debugEV3.print(numero);
  debugEV3.print(',');
  debugEV3.print(mvt);
  debugEV3.print('-');
  if( mvt == FERME ) debugEV3.printPGM(EV3_DEBUG_FERME);
  else if( mvt == OUVERT) debugEV3.printPGM(EV3_DEBUG_OUVERT);
  else debugEV3.printPGM(EV3_DEBUG_AUTRE);
  debugEV3.println(')');
#endif
  if (mvt == 0) return true; // aucun mvt demandé

  if( (mode & EV_OUVERTURE_PARTIELLE) != 0){ // electrovanne à mvt partiel
    /*
    if ( mvt_timerID != 0) { // un mouvement est déjà en cours... on met celui-ci en attente
      mvt_futur += mvt;
      return true;
    }
    return bougeEVpartiel(mvt);
    */
  } else { // EV à mvt complet seulement
    if( mvt != OUVERT && mvt != FERME) return false; // si demande de mvt partiel => impossible
    return bougeEVtotal(mvt);
  }
}

void ElectroVanne3::repos(){ // position de repos de l'EV
  if( this->mvt_timerID != 0 ){
    this->timer->stop(this->mvt_timerID);
    this->mvt_timerID=0;
  }

  return ElectroVanne3::repos(this->numero, this->mode);
}

void ElectroVanne3::repos(int8_t numero, uint8_t mode){ // position de repos de l'EV
#ifdef DEBUG_EV3
  debugEV3.printPGM(EV3_DEBUG_repos);
  debugEV3.print(numero);
  debugEV3.println(')');

  debugEV3.printPGM(FR::TXT_MODE);
  debugEV3.write(':');
  debugEV3.print(mode,BIN);

  debugEV3.write(' ');
  debugEV3.print(F("EV_CMD_2_FILS:"));
  debugEV3.print(EV_CMD_2_FILS,BIN);
  debugEV3.write(' ');
  debugEV3.print(F("mode & EV_CMD_2_FILS:"));
  debugEV3.println(mode & EV_CMD_2_FILS,BIN);
#endif
  
    uint8_t masq=getMasqCmde(numero, mode);

    if( numero <= 4 ) PORTH &= ~masq;
    else PORTC &= ~masq;
  
    #ifdef DEBUG_EV3
    char port;
    if( numero <= 4 ) port='H';
    else port = 'C';
    
    for(uint8_t i=0; i<8; i++){
      if( masq & (1<<i) ) {
        debugEV3.print(F("Port_"));
        debugEV3.print(port);
        debugEV3.print(i);
        debugEV3.print(F("=0 / "));
      }
    }
    debugEV3.println();
    #endif
}

bool ElectroVanne3::demarreMvt(int8_t numero, uint8_t mode, int8_t mvt){
  ElectroVanne3::repos(numero, mode);
  if( mvt != OUVERT && mvt != FERME ) return false;

  uint8_t masq = ElectroVanne3::getMasqCmde(numero, mode, mvt);

  #ifdef DEBUG_EV3
    debugEV3.print(F("demarre mouvement "));
    if( mvt == FERME ) debugEV3.print(F("ferme"));
    if( mvt == OUVERT ) debugEV3.print(F("ouvert"));
    char port;
    if( numero <= 4 ) port='H';
    else port = 'C';
    
    for(uint8_t i=0; i<8; i++){
      if( masq & (1<<i) ) {
        debugEV3.print(F("Port_"));
        debugEV3.print(port);
        debugEV3.print(i);
        debugEV3.print(F("=1 / "));
      }
    }
    debugEV3.println();
    #endif

  if( numero <= 4 ){
    PORTH |= masq;
  } else {
    PORTC |= masq;
  }
}

bool ElectroVanne3::bougeEVtotal(int8_t mvt){
  if( ! this->estConfigure() ) return false;

  const cmdeInterne cmde=cmdeActEtatTouteEV;
  #ifdef DEBUG_EV3
    debugEV3.printPGM(EV3_DEBUG_bougEVtotal);
    debugEV3.print(numero);
    debugEV3.print(',');
    debugEV3.print(mvt);
    debugEV3.print('-');
    if( mvt == FERME ) debugEV3.printPGM(EV3_DEBUG_FERME);
    else if( mvt == OUVERT) debugEV3.printPGM(EV3_DEBUG_OUVERT);
    else debugEV3.printPGM(EV3_DEBUG_AUTRE);
    debugEV3.println(')');
  #endif

  if( mvt == 0 ) return true; // pas de mvt
  if( mvt != OUVERT && mvt != FERME) return false; // mvt incompatible

  if( mvt == etatEV() ) return true; // si l'EV est déjà dans la position demandée

  if( mvt_timerID != 0 ){ // mvt en cours
    timer->stop(this->mvt_timerID); // on stop l'attente de fin du mvt
    this->mvt_timerID = 0;
  }

  repos(); // mise au repos de l'EV

  if( mode & EV_CMD_2_FILS ){ // commande sur 2 fils

    ElectroVanne3::demarreMvt(this->numero, this->mode, mvt);

    this->mvt_date=millis();

    #ifdef DEBUG_EV3
      if( mvt == FERME) {
        debugEV3.printPGM(EV3_DEBUG_FERMETURE);
        debugEV3.write(' ');
        debugEV3.printPGM(EV3_DEBUG_digitalWrite);
        debugEV3.print(brocheEV);
        debugEV3.printPGM(EV3_DEBUG_PLUS1);
        debugEV3.write(',');
        debugEV3.printPGM(EV3_DEBUG_HIGH);
        debugEV3.println(')');
      }else{
        debugEV3.printPGM(EV3_DEBUG_OUVERTURE);
        debugEV3.write(' ');
        debugEV3.printPGM(EV3_DEBUG_digitalWrite);
        debugEV3.print(brocheEV);
        debugEV3.write(',');
        debugEV3.printPGM(EV3_DEBUG_HIGH);
        debugEV3.println(')');
      }
    #endif

    if( mvt == OUVERT) this->mvt_pos_fin=OUVERT;
    else this->mvt_pos_fin=FERME;

    // déclenchement du timer pour arrêt ou controle
    if ( (this->mode & ( EV_ETAT_SUR_CMD | EV_ETAT_2_FILS | EV_ETAT_ANALOGIQUE)) == 0) { // pas de capteur de fin de course -> ouverture ou fermeture complète programmée pour 10s
      this->mvt_timerID = timer->after(10000, ElectroVanne3::finMvtTotal, this);
    } else { // sinon verification toutes les 0,5s de l'état de l'EV
      this->mvt_timerID = timer->every(500, ElectroVanne3::finMvtTotal, this, 16);
    }
    this->position=0;

  } else {  // commande sur 1 fils
    uint8_t masq;
    if( this->numero < 0 ) masq = 0b10 << (((-this->numero-1)%4)*2);
    else masq = 0b01 << (((this->numero-1)%4)*2);

    if( ( ( mode & MAINTIENT_OUVETURE) && mvt == OUVERT ) || ( (mode & MAINTIENT_FERMETURE) && mvt == FERME ) ) {
      if( this->numero <= 4 && this->numero >= -4 ) PORTH |= masq;
      else PORTC |= masq;
    }
    this->position=mvt;
  }
  xQueueSend(qCmdeToTimer,&cmde,0); // actualisation de la position des EV !
  return true;
}

void ElectroVanne3::finMvtTotal(void* data) {
  ElectroVanne3* EV = (ElectroVanne3*) data;
  const cmdeInterne cmde=cmdeActEtatTouteEV;

#ifdef DEBUG_EV3
  debugEV3.printPGM(EV3_DEBUG_finMvtTotal);
  debugEV3.print(EV->numero);
  debugEV3.println(')');
#endif

  if ( (EV->mode & ( EV_ETAT_SUR_CMD | EV_ETAT_2_FILS | EV_ETAT_ANALOGIQUE)) == 0) { // pas de capteur de fin de course -> aucun moyen de savoir où on est!!
    if (EV->mvt_pos_fin == FERME ) EV->position = FERME; // enregistrement de la nouvelle position
    else EV->position = OUVERT;

    if( EV->mvt_timerID != 0) timer->stop(EV->mvt_timerID);
    EV->mvt_timerID = 0;

    if( ( EV->mvt_pos_fin == FERME && (EV->mode&MAINTIENT_FERMETURE)==0)
      || ( EV->mvt_pos_fin == OUVERT && (EV->mode&MAINTIENT_OUVETURE)==0)) EV->repos();

    xQueueSend(qCmdeToTimer,&cmde,0); // actualisation de la position des EV !

  } else { // capteur de fin de course

    if ( EV->etatEV() == EV->mvt_pos_fin ) { // le mouvement s'est terminé !!
      timer->stop(EV->mvt_timerID); // fin de l'appel régulier du suivi du mvt
      EV->mvt_timerID = 0;

      if (EV->mvt_pos_fin == FERME ) EV->position = FERME; // enregistrement de la nouvelle position
      else EV->position = OUVERT;

      if( ( EV->mvt_pos_fin == FERME && (EV->mode&MAINTIENT_FERMETURE)==0)
        || ( EV->mvt_pos_fin == OUVERT && (EV->mode&MAINTIENT_OUVETURE)==0)) EV->repos();

      xQueueSend(qCmdeToTimer,&cmde,0); // actualisation de la position des EV !

      if ( EV->mvt_pos_deb == - EV->mvt_pos_fin) { // si la position de début est l'inverse de la position fin
        // on pourrais ici vérifier l'ordre de grandeur du temps d'ouverture de la vanne
        // A FAIRE
      }
    } else { // le mouvement n'est pas encore terminé
      if ( EV->tempsOuvertureVanne==0 || ((millis() - EV->mvt_date) < (EV->tempsOuvertureVanne * 2)) ) { // si la durée de déplacement inférieur au temps normal x 2
        return; // quitte pour revenir plus tard (au prochain appel du suivi du mvt)
      }
      // sinon il y a un problème !!
#ifdef DEBUG_EV3
      debugEV3.printPGM(EV3_DEBUG_electrovanne);
      debugEV3.println(EV->numero);
      debugEV3.printlnPGM(EV3_DEBUG_TMP_OUV_BCP_TROP_LONG);
      debugEV3.printPGM(FR::TXT_TEMP);
      debugEV3.write(' ');
      debugEV3.printPGM(EV3_DEBUG_OUVERTURE);
      debugEV3.write(':');
      debugEV3.println((millis() - EV->mvt_date));
      debugEV3.printPGM(EV3_DEBUG_tempsOuvertureVanne);
      debugEV3.println(EV->tempsOuvertureVanne);
#endif
      EV->repos();

      pileErreur->addErreur(numModuleErr,ERREUR_EV_TEMPS_TROP_LONG,EV->numero);
      EV->position = ERREUR_EV_TEMPS_TROP_LONG; // met une erreur dans la position
      timer->stop(EV->mvt_timerID); // fin de l'appel régulier du suivi du mvt
      EV->mvt_timerID = 0;
    
      xQueueSend(qCmdeToTimer,&cmde,0); // actualisation de la position des EV !
    }
  }
}


int16_t ElectroVanne3::testEV(int8_t numero, uint8_t mode) {
  uint16_t tempsOuvertureVanne;
  if ( (mode & ( EV_ETAT_SUR_CMD | EV_ETAT_2_FILS | EV_ETAT_ANALOGIQUE)) == 0) { // pas de capteur de fin de courses -> test impossible
    return 0;
  }

  /* test de l'électrovanne */

#ifdef DEBUG_EV3
  debugEV3.printPGM(EV3_DEBUG_TEST_EV);
  debugEV3.print(numero);
  debugEV3.printlnPGM(EV3_DEBUG_ET_CAPTEUR);
#endif

  int8_t etatVanne = ElectroVanne3::etatEV(numero, mode);
  int8_t etatInitVanne = etatVanne;
  unsigned long date;
  tempsOuvertureVanne = 0;

  if (etatVanne != FERME && etatVanne != OUVERT) { // position initiale ni fermé, ni ouvert -> ouverture de la vanne
    ElectroVanne3::demarreMvt(numero, mode, OUVERT);
    date = millis();
    do {
      timer->update();

      if( xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED ){
        delay(100); // si le noyau FreeRTOS est inactif
      } else {
        vTaskDelay( 100 / portTICK_PERIOD_MS ); // si le noyau FreeRTOS est actif
      }

      etatVanne = ElectroVanne3::etatEV(numero, mode);
    } while (millis() - date < 11000 && etatVanne != FERME && etatVanne != OUVERT); // on boucle pendant 11s ou jusqu'à que l'état de l'electrovanne change
    if (etatVanne == 0) { // l'état de l'electrovanne est encore à 0 --> l'electrovanne est bloqué ou mal branchée
#ifdef DEBUG_EV3
      debugEV3.printlnPGM(EV3_DEBUG_ERR_AUCUN_MVT);
#endif
      ElectroVanne3::repos(numero,mode);
      return - ERREUR_EV_AUCUN_MVT_OBS;
    }
    if ( etatVanne == FERME) { // l'état de l'electrovanne s'est fermée alors qu'on a demandé une ouverture... inversion soit des fils de test, soit des branchement electrovanne
#ifdef DEBUG_EV3
      debugEV3.printlnPGM(EV3_DEBUG_ERR_EV_CAPT_INV);
#endif
      ElectroVanne3::repos(numero,mode);
      return - ERREUR_EV_EV_OU_CAPTEUR_INVERSE;
    }
#ifdef DEBUG_EV3
    debugEV3.printlnPGM(EV3_DEBUG_OUV_PART_OK);
#endif
  }
  if (etatVanne == FERME) { // si la vanne est fermé, test de l'ouverture
    ElectroVanne3::demarreMvt(numero, mode, OUVERT);
    date = millis();
    do {
      timer->update();

      if( xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED ){
        delay(100); // si le noyau FreeRTOS est inactif
      } else {
        vTaskDelay( 100 / portTICK_PERIOD_MS ); // si le noyau FreeRTOS est actif
      }

      etatVanne = ElectroVanne3::etatEV(numero, mode);
    } while (millis() - date < 11000 && etatVanne != OUVERT); // on boucle pendant 11s ou jusqu'à que l'état de l'electrovanne change

    tempsOuvertureVanne = (unsigned int)(millis() - date); // calcul du temps d'ouverture de l'electrovanne


    if (etatVanne == FERME) { // l'état de l'electrovanne est encore à FERME --> l'electrovanne est bloqué ou mal branchée
#ifdef DEBUG_EV3
      debugEV3.printlnPGM(EV3_DEBUG_ERR_EV_OUVRE_PAS);
#endif
      ElectroVanne3::repos(numero,mode);
      return - ERREUR_EV_OUVRE_PAS;
    }
    if ( etatVanne == 0) { // l'état de l'electrovanne est entre ouvert et ferme. soit capteur ouvert mal branché, soit electrovanne lente
#ifdef DEBUG_EV3
      debugEV3.printlnPGM(EV3_DEBUG_ERR_EV_OUVERTURE_BLOQUE_MILIEU);
#endif
      ElectroVanne3::repos(numero,mode);
      return - ERREUR_EV_OUVERTURE_BLOQUE_MILIEU;
    }

#ifdef DEBUG_EV3
    debugEV3.printlnPGM(EV3_DEBUG_FERME_EV_OK);
#endif
  }

  // test de la fermeture de la vanne
  ElectroVanne3::demarreMvt(numero, mode, FERME);
  date = millis();
  do {
    timer->update();

    if( xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED ){
      delay(100); // si le noyau FreeRTOS est inactif
    } else {
      vTaskDelay( 100 / portTICK_PERIOD_MS ); // si le noyau FreeRTOS est actif
    }

    etatVanne = ElectroVanne3::etatEV(numero, mode);
  } while (millis() - date < 11000 && etatVanne != FERME); // on boucle pendant 11s ou jusqu'à que l'état de l'electrovanne change

  if ( tempsOuvertureVanne == 0) { // encore aucune mesure de temps d'electrovanne
    tempsOuvertureVanne = (unsigned int)(millis() - date);
  } else {
    tempsOuvertureVanne = (tempsOuvertureVanne + (unsigned int)(millis() - date)) / 2; // moyenne des temps d'ouverture/fermeture
  }


  if (etatVanne == OUVERT) { // l'état de l'electrovanne est encore à FERME --> l'electrovanne est bloqué ou mal branchée
#ifdef DEBUG_EV3
    debugEV3.printlnPGM(EV3_DEBUG_ERR_EV_FERME_PAS);
#endif
    ElectroVanne3::repos(numero,mode);
    return - ERREUR_EV_FERME_PAS;
  }
  if ( etatVanne == 0) { // l'état de l'electrovanne est entre ouvert et ferme. soit capteur ouvert mal branché, soit electrovanne lente
#ifdef DEBUG_EV3
    debugEV3.printlnPGM(EV3_DEBUG_ERR_EV_FERMETURE_BLOQUE_MILIEU);
#endif
    ElectroVanne3::repos(numero,mode);
    return - ERREUR_EV_FERMETURE_BLOQUE_MILIEU;
  }
#ifdef DEBUG_EV3
  debugEV3.printlnPGM(EV3_DEBUG_FERME_EV_OK);
#endif


  if (etatInitVanne != FERME) { // si l'on a pas déjà fait le test de l'ouverture
    ElectroVanne3::demarreMvt(numero, mode, OUVERT);
    date = millis();
    do {
      timer->update();

      if( xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED ){
        delay(100); // si le noyau FreeRTOS est inactif
      } else {
        vTaskDelay( 100 / portTICK_PERIOD_MS ); // si le noyau FreeRTOS est actif
      }

      etatVanne = ElectroVanne3::etatEV(numero, mode);
    } while (millis() - date < 11000 && etatVanne != OUVERT); // on boucle pendant 11s ou jusqu'à que l'état de l'electrovanne change

    tempsOuvertureVanne = (tempsOuvertureVanne + (unsigned int)(millis() - date)) / 2; // moyenne des temps d'ouverture/fermeture


    if (etatVanne == FERME) { // l'état de l'electrovanne est encore à FERME --> l'electrovanne est bloqué ou mal branchée
#ifdef DEBUG_EV3
      debugEV3.printlnPGM(EV3_DEBUG_ERR_EV_OUVRE_PAS);
#endif
      ElectroVanne3::repos(numero,mode);
      return - ERREUR_EV_OUVRE_PAS;
    }
    if ( etatVanne == 0) { // l'état de l'electrovanne est entre ouvert et ferme. soit capteur ouvert mal branché, soit electrovanne lente
#ifdef DEBUG_EV3
      debugEV3.printlnPGM(EV3_DEBUG_ERR_EV_OUVERTURE_BLOQUE_MILIEU);
#endif
      ElectroVanne3::repos(numero,mode);
      return - ERREUR_EV_OUVERTURE_BLOQUE_MILIEU;
    }
#ifdef DEBUG_EV3
    debugEV3.printlnPGM(EV3_DEBUG_FERME_EV_OK);
#endif
  }
#ifdef DEBUG_EV3
  debugEV3.printPGM(EV3_DEBUG_tempsOuvertureVanne);
  debugEV3.print(tempsOuvertureVanne);
  debugEV3.write(' ');
  debugEV3.printlnPGM(EV3_DEBUG_MS);
  debugEV3.printPGM(EV3_DEBUG_electrovanne);
  debugEV3.write(' ');
  debugEV3.print(numero);
  debugEV3.printPGM(EV3_DEBUG_ET_CAPTEUR);
  debugEV3.printlnPGM(EV3_DEBUG_OK);
#endif
  ElectroVanne3::repos(numero,mode);
  return (int16_t)tempsOuvertureVanne;
}
