#include "EV.h"

/*
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
  #ifdef CR01_MF
    ++n;
  #endif
  #ifdef CR01_MAINTIENT
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
    if( num == n) return CR01;
    ++n;
  #endif
  #ifdef CR01modif
    if( num == n) return CR01modif;
    ++n;
  #endif
  #ifdef CR05
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
  #ifdef CR01_MF
    if( num == n) return CR01_MF;
    ++n;
  #endif
  #ifdef CR01_MAINTIENT
    if( num == n) return CR01_MAINTIENT;
    ++n;
  #endif
  #ifdef EV_RELAI
    if( num == n) return EV_RELAI;
    ++n;
  #endif
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
  #ifdef CR01_MF
  case CR01_MF:
    return FR::TXT_CR01_MF;
  #endif
  #ifdef CR01_MAINTIENT
    case CR01_MAINTIENT:
      return FR::TXT_CR01_MAINTIENT;
  #endif
  #ifdef EV_RELAI
    case EV_RELAI:
      return FR::TXT_EV_RELAI;
  #endif
    default:
      return FR::TXT_ERREUR;
  }
}
*/


uint8_t ElectroVanne::numModuleErr=0;
PileErreur* ElectroVanne::pileErreur=NULL;
Timer* ElectroVanne::timer=NULL;
OneWire* ElectroVanne::ds=NULL;

void ElectroVanne::begin(PileErreur& _pileErreur,Timer& _timer,OneWire& _ds){
  pileErreur=&_pileErreur;
  timer=&_timer;
  ds=&_ds;
  if( numModuleErr==0) numModuleErr=_pileErreur.getNumModuleSuiv();
}

void ElectroVanne::print(HardwareSerial& serial){
  serial.print("ElectroVanne(num:");

  serial.print(numero);
  serial.print(", mode:");
  serial.print(mode);

  serial.print(")");
}

ElectroVanne &ElectroVanne::operator=(const ElectroVanne &source){
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

uint16_t ElectroVanne::save(uint16_t addr) {
  addr = ecritEEPROM((uint8_t *)&this->tempsOuvertureVanne, 2, addr);
  EEPROM.write(addr, this->mode); ++addr;
  EEPROM.write(addr, this->numero); ++addr;
  return addr;
}

bool ElectroVanne::save(BlocMem* bloc) {
  if ( ! bloc->ecrit((uint8_t*)&this->tempsOuvertureVanne, 2)) return false;
  if ( ! bloc->ecrit((uint8_t*)&this->mode, 1)) return false;
  if ( ! bloc->ecrit((uint8_t*)&this->numero, 1)) return false;
  return true;
}

bool ElectroVanne::load(BlocMem* bloc) {
  if ( ! bloc->lit((uint8_t*)&this->tempsOuvertureVanne, 2)) return false;
  if ( ! bloc->lit((uint8_t*)&this->mode, 1)) return false;
  if ( ! bloc->lit((uint8_t*)&this->numero, 1)) return false;
  return true;
}

uint16_t ElectroVanne::load(uint16_t addr) {
  this->defaut();
  addr = litEEPROM((uint8_t *)&this->tempsOuvertureVanne, 2, addr);
  this->mode = EEPROM.read(addr); ++addr;
  this->numero = EEPROM.read(addr); ++addr;
  return addr;
}

bool ElectroVanne::loadPort(){
  return true;
}

void ElectroVanne::defaut(void) {
  this->tempsOuvertureVanne = 0;
  this->position = 100;
  this->mode = EV_NON_CONFIGURE;
  this->numero = 255;

  this->mvt_timerID = 0;
  this->mvt_futur = 0;
}

bool ElectroVanne::estConfigure() const {
  return (mode != EV_NON_CONFIGURE);
}


void ElectroVanne::initBroches() {
  switch (mode) {
#ifdef CR05
    case CR05:
      initBrochesCR05();
#endif
#ifdef CR05_A
    case CR05_A:
      initBrochesCR05_A();
#endif
#ifdef CR01modif
    case CR01modif:
      initBrochesCR01modif();
#endif
#ifdef CR01
    case CR01:
      initBrochesCR01();
#endif
#ifdef CR07
    case CR07:
      return initBrochesCR07();
#endif
  }
}

void ElectroVanne::initBrochesCR01modif() {
  switch (numero) {
    case 0:
#ifdef BIT_E_EV_0
      DDR_E_EV &= ~(1 << BIT_E_EV_0); // mise en entré de la broche du capteur d'état de l'EV
      PORT_E_EV &= ~(1 << BIT_E_EV_0); // on enlève le pullup de la broche du capteur
#endif
      DDR_EV_0 |= MASQUE_EV_0; //mise des broches de l'EV en sortie
      PORT_EV_0 &= ~(MASQUE_EV_0); // mise des broches à 0
      break;
#if NB_EV_MAX >1
    case 1:
#ifdef BIT_E_EV_1
      DDR_E_EV &= ~(1 << BIT_E_EV_1); // mise en entré de la broche du capteur d'état de l'EV
      PORT_E_EV &= ~(1 << BIT_E_EV_1); // on enlève le pullup de la broche du capteur
#endif
      DDR_EV_1 |= MASQUE_EV_1; //mise des broches de l'EV en sortie
      PORT_EV_1 &= ~(MASQUE_EV_1); // mise des broches à 0
      break;
#endif
#if NB_EV_MAX >2
    case 2:
#ifdef BIT_E_EV_2
      DDR_E_EV &= ~(1 << BIT_E_EV_2); // mise en entré de la broche du capteur d'état de l'EV
      PORT_E_EV &= ~(1 << BIT_E_EV_2); // on enlève le pullup de la broche du capteur
#endif
      DDR_EV_2 |= MASQUE_EV_2; //mise des broches de l'EV en sortie
      PORT_EV_2 &= ~(MASQUE_EV_2); // mise des broches à 0
      break;
#endif
#if NB_EV_MAX >3
    case 3:
#ifdef BIT_E_EV_3
      DDR_E_EV &= ~(1 << BIT_E_EV_3); // mise en entré de la broche du capteur d'état de l'EV
      PORT_E_EV &= ~(1 << BIT_E_EV_3); // on enlève le pullup de la broche du capteur
#endif
      DDR_EV_3 |= MASQUE_EV_3; //mise des broches de l'EV en sortie
      PORT_EV_3 &= ~(MASQUE_EV_3); // mise des broches à 0
      break;
#endif
  }
}

void ElectroVanne::initBrochesCR01() {
  switch (numero) {
    case 0:
      DDR_EV_0 |= MASQUE_EV_0; //mise des broches de l'EV en sortie
      PORT_EV_0 &= ~(MASQUE_EV_0); // mise des broches à 0
      break;
#if NB_EV_MAX >1
    case 1:
      DDR_EV_1 |= MASQUE_EV_1; //mise des broches de l'EV en sortie
      PORT_EV_1 &= ~(MASQUE_EV_1); // mise des broches à 0
      break;
#endif
#if NB_EV_MAX >2
    case 2:
      DDR_EV_2 |= MASQUE_EV_2; //mise des broches de l'EV en sortie
      PORT_EV_2 &= ~(MASQUE_EV_2); // mise des broches à 0
      break;
#endif
#if NB_EV_MAX >3
    case 3:
      DDR_EV_3 |= MASQUE_EV_3; //mise des broches de l'EV en sortie
      PORT_EV_3 &= ~(MASQUE_EV_3); // mise des broches à 0
      break;
#endif
  }
}

void ElectroVanne::initBrochesCR05() {
  switch (numero) {
    case 0:
#ifdef BIT_E_EV_0_O
      DDR_E_EV_0 &= ~(1 << BIT_E_EV_0_O | 1 << BIT_E_EV_0_F); // mise en entré des broches du capteur d'état de l'EV
      PORT_E_EV_0 |= (1 << BIT_E_EV_0_O | 1 << BIT_E_EV_0_F); // on met le pullup des broches du capteur
#endif
      DDR_EV_0 |= MASQUE_EV_0; //mise des broches de l'EV en sortie
      PORT_EV_0 &= ~(MASQUE_EV_0); // mise des broches à 0
      break;
#if NB_EV_MAX >1
    case 1:
#ifdef BIT_E_EV_1_O
      DDR_E_EV_1 &= ~(1 << BIT_E_EV_1_O | 1 << BIT_E_EV_1_F); // mise en entré des broches du capteur d'état de l'EV
      PORT_E_EV_1 |= (1 << BIT_E_EV_1_O | 1 << BIT_E_EV_1_F); // on met le pullup des broches du capteur
#endif
      DDR_EV_1 |= MASQUE_EV_1; //mise des broches de l'EV en sortie
      PORT_EV_1 &= ~(MASQUE_EV_1); // mise des broches à 0
      break;
#endif
#if NB_EV_MAX >2
    case 2:
#ifdef BIT_E_EV_2_O
      DDR_E_EV_2 &= ~(1 << BIT_E_EV_2_O | 1 << BIT_E_EV_2_F); // mise en entré des broches du capteur d'état de l'EV
      PORT_E_EV_2 |= (1 << BIT_E_EV_2_O | 1 << BIT_E_EV_2_F); // on met le pullup des broches du capteur
#endif
      DDR_EV_2 |= MASQUE_EV_2; //mise des broches de l'EV en sortie
      PORT_EV_2 &= ~(MASQUE_EV_2); // mise des broches à 0
      break;
#endif
#if NB_EV_MAX >3
    case 3:
#ifdef BIT_E_EV_3_O
      DDR_E_EV_3 &= ~(1 << BIT_E_EV_3_O | 1 << BIT_E_EV_3_F); // mise en entré des broches du capteur d'état de l'EV
      PORT_E_EV_3 |= (1 << BIT_E_EV_3_O | 1 << BIT_E_EV_3_F); // on met le pullup des broches du capteur
#endif
      DDR_EV_3 |= MASQUE_EV_3; //mise des broches de l'EV en sortie
      PORT_EV_3 &= ~(MASQUE_EV_3); // mise des broches à 0
      break;
#endif
  }
}

void ElectroVanne::initBrochesCR05_A() {
  switch (numero) {
    case 0:
#ifdef BIT_E_EV_0
      DDR_E_EV &= ~(1 << BIT_E_EV_0); // mise en entré de la broche du capteur d'état de l'EV
      PORT_E_EV &= ~(1 << BIT_E_EV_0); // on enlève le pullup de la broche du capteur
#endif
      DDR_EV_0 |= MASQUE_EV_0; //mise des broches de l'EV en sortie
      PORT_EV_0 &= ~(MASQUE_EV_0); // mise des broches à 0
      break;
#if NB_EV_MAX >1
    case 1:
#ifdef BIT_E_EV_1
      DDR_E_EV &= ~(1 << BIT_E_EV_1); // mise en entré de la broche du capteur d'état de l'EV
      PORT_E_EV &= ~(1 << BIT_E_EV_1); // on enlève le pullup de la broche du capteur
#endif
      DDR_EV_1 |= MASQUE_EV_1; //mise des broches de l'EV en sortie
      PORT_EV_1 &= ~(MASQUE_EV_1); // mise des broches à 0
      break;
#endif
#if NB_EV_MAX >2
    case 2:
#ifdef BIT_E_EV_2
      DDR_E_EV &= ~(1 << BIT_E_EV_2); // mise en entré de la broche du capteur d'état de l'EV
      PORT_E_EV &= ~(1 << BIT_E_EV_2); // on enlève le pullup de la broche du capteur
#endif
      DDR_EV_2 |= MASQUE_EV_2; //mise des broches de l'EV en sortie
      PORT_EV_2 &= ~(MASQUE_EV_2); // mise des broches à 0
      break;
#endif
#if NB_EV_MAX >3
    case 3:
#ifdef BIT_E_EV_3
      DDR_E_EV &= ~(1 << BIT_E_EV_3); // mise en entré de la broche du capteur d'état de l'EV
      PORT_E_EV &= ~(1 << BIT_E_EV_3); // on enlève le pullup de la broche du capteur
#endif
      DDR_EV_3 |= MASQUE_EV_3; //mise des broches de l'EV en sortie
      PORT_EV_3 &= ~(MASQUE_EV_3); // mise des broches à 0
      break;
#endif
  }
}

void ElectroVanne::initEV() {
  if ( mode == EV_NON_CONFIGURE) return;

  initBroches();
  if ( mode != EV_NON_CONFIGURE) {
#ifdef DEBUG
    Serial.print("fermeture de l'EV ");
    Serial.println(numero);
#endif
    bougeEV(FERME);
  }
}

int8_t ElectroVanne::etatEV() {
  switch (mode) {
  #ifdef CR05
      case CR05:
        return etatEVCR05();
  #endif
#ifdef CR05_A
    case CR05_A:
      return etatEVCR05_A();
#endif
#ifdef CR01modif
    case CR01modif:
      return etatEVCR01modif();
#endif
#ifdef CR01
    case CR01:
      return 0; // aucun retour possible
#endif
#ifdef CR07
    case CR07:
      return etatEVCR07();
#endif
  }
  return 0;
}

int8_t ElectroVanne::etatEVCR05() {
  // A FAIRE
  switch(numero){
    #ifdef PIN_E_EV_0
      case 0:
        if( (PIN_E_EV_0 & (1 << BIT_E_EV_0_F)) == 0 ) return FERME;
        if( (PIN_E_EV_0 & (1 << BIT_E_EV_0_O)) == 0 ) return OUVERT;
        return 0;
    #endif
    #ifdef PIN_E_EV_1
      case 1:
        if( (PIN_E_EV_1 & (1 << BIT_E_EV_1_F)) == 0 ) return FERME;
        if( (PIN_E_EV_1 & (1 << BIT_E_EV_1_O)) == 0 ) return OUVERT;
        return 0;
    #endif
    #ifdef PIN_E_EV_2
      case 2:
        if( (PIN_E_EV_2 & (1 << BIT_E_EV_2_F)) == 0 ) return FERME;
        if( (PIN_E_EV_2 & (1 << BIT_E_EV_2_O)) == 0 ) return OUVERT;
        return 0;
    #endif
    #ifdef PIN_E_EV_0
      case 3:
        if( (PIN_E_EV_3 & (1 << BIT_E_EV_3_F)) == 0 ) return FERME;
        if( (PIN_E_EV_3 & (1 << BIT_E_EV_3_O)) == 0 ) return OUVERT;
        return 0;
    #endif
  }
  return 0;
}

int8_t ElectroVanne::etatEVCR05_A() {
  uint16_t val = 0;
  switch (numero) {
    case 0:
#ifdef BIT_E_EV_0
      if ( (PIN_E_EV & (1 << BIT_E_EV_0)) == 0) return 0;
#endif
      val = analogRead(BROCHE_E_EV_0);
      break;
#if NB_EV_MAX>1
    case 1:
#ifdef BIT_E_EV_1
      if ( (PIN_E_EV & (1 << BIT_E_EV_1)) == 0) return 0;
#endif
      val = analogRead(BROCHE_E_EV_1);
      break;
#if NB_EV_MAX>2
    case 2:
#ifdef BIT_E_EV_2
      if ( (PIN_E_EV & (1 << BIT_E_EV_2)) == 0) return 0;
#endif
      val = analogRead(BROCHE_E_EV_2);
      break;
#if NB_EV_MAX>3
    case 3:
#ifdef BIT_E_EV_3
      if ( (PIN_E_EV & (1 << BIT_E_EV_3)) == 0) return 0;
#endif
      val = analogRead(BROCHE_E_EV_3);
      break;
#endif
#endif
#endif
  }
  if ( val < 500) return 0;
  if ( val < 850 ) return FERME;
  return OUVERT;
}

int8_t ElectroVanne::etatEVCR01modif() {
  uint8_t etatInit;
  int8_t result = 1;
  int16_t val = 0;
  switch (numero) {
    case 0:
      etatInit = PORT_EV_0 & MASQUE_EV_0; // memorisation de l'état initial des broches
      PORT_EV_0 |= MASQUE_EV_0;       // mise des 2 broches à l'état haut
#ifdef BIT_E_EV_0
      result = PIN_E_EV & (1 << BIT_E_EV_0); // test si un 1 est lisible sur la broche (càd U>2,7V) => OUVERT ou FERME sinon Entre les 2
      if ( result != 0) { // si soit ouvert soit ferme
#endif
        val = analogRead(BROCHE_E_EV_0);
#ifdef BIT_E_EV_0
      }
#endif
      PORT_EV_0 &= ~MASQUE_EV_0 & etatInit; // remise à l'état initial
      break;
#if NB_EV_MAX > 1
    case 1:
      etatInit = PORT_EV_1 & MASQUE_EV_1; // memorisation de l'état initial des broches
      PORT_EV_1 |= MASQUE_EV_1;       // mise des 2 broches à l'état haut
#ifdef BIT_E_EV_1
      result = PIN_E_EV & (1 << BIT_E_EV_1); // test si un 1 est lisible sur la broche (càd U>2,7V) => OUVERT ou FERME sinon Entre les 2
      if ( result != 0) { // si soit ouvert soit ferme
#endif
        val = analogRead(BROCHE_E_EV_1);
#ifdef BIT_E_EV_1
      }
#endif
      PORT_EV_1 &= ~MASQUE_EV_1 & etatInit; // remise à l'état initial
      break;
#endif
#if NB_EV_MAX > 2
    case 2:
      etatInit = PORT_EV_2 & MASQUE_EV_2; // memorisation de l'état initial des broches
      PORT_EV_2 |= MASQUE_EV_2;       // mise des 2 broches à l'état haut
#ifdef BIT_E_EV_2
      result = PIN_E_EV & (1 << BIT_E_EV_2); // test si un 1 est lisible sur la broche (càd U>2,7V) => OUVERT ou FERME sinon Entre les 2
      if ( result != 0) { // si soit ouvert soit ferme
#endif
        val = analogRead(BROCHE_E_EV_2);
#ifdef BIT_E_EV_2
      }
#endif
      PORT_EV_2 &= ~MASQUE_EV_2 & etatInit; // remise à l'état initial
      break;
#endif
#if NB_EV_MAX > 3
    case 3:
      etatInit = PORT_EV_3 & MASQUE_EV_3; // memorisation de l'état initial des broches
      PORT_EV_3 |= MASQUE_EV_3;       // mise des 2 broches à l'état haut
#ifdef BIT_E_EV_3
      result = PIN_E_EV & (1 << BIT_E_EV_3); // test si un 1 est lisible sur la broche (càd U>2,7V) => OUVERT ou FERME sinon Entre les 2
      if ( result != 0) { // si soit ouvert soit ferme
#endif
        val = analogRead(BROCHE_E_EV_3);
#ifdef BIT_E_EV_3
      }
#endif
      PORT_EV_3 &= ~MASQUE_EV_3 & etatInit; // remise à l'état initial
      break;
#endif
  }
  if (result != 0) {
    result = 0;
    if ( val > 900) result = OUVERT;
    else if ( val > 700) result = FERME;
  }
  return result;
}


bool ElectroVanne::bougeEV(int8_t mvt) {
#ifdef DEBUG
  Serial.print("bougeEV(");
  Serial.print(numero);
  Serial.print(',');
  Serial.print(mvt);
  Serial.println(')');
#endif
  if (mvt == 0) return true; // aucun mvt demandé

  if ( mvt_timerID != 0) { // un mouvement est déjà en cours... on met celui-ci en attente
    mvt_futur += mvt;
    return true;
  }

  uint8_t sens_mvt;
  if ( mvt < 0) sens_mvt = -10;
  else sens_mvt = 10;

  mvt_date = millis();
  mvt_pos_deb = position;

  if ( tempsOuvertureVanne == 0) { // si la mesure de durée d'ouverture n'a pas été faite
    if ( mode == CR01 || mode == CR01modif || mode==CR01_MO) {
      mvt_pos_fin = (sens_mvt == OUVERT) ? OUVERT : 0; // si on ne connais pas la durée, on ne peut faire qu'une ouverture ou fermeture complète
      mvt_timerID = timer->after(10000, ElectroVanne::finMvt, this);
    } else {
      mvt_pos_fin = (sens_mvt == OUVERT) ? OUVERT : 0; // si on ne connais pas la durée, on ne peut faire qu'une ouverture ou fermeture complète
      mvt_timerID = timer->every(500, ElectroVanne::finMvt, this);
    }
  } else {
    if ( mvt == OUVERT || mvt == FERME) { // si ouverture ou fermeture complète
      if ( mode == CR01|| mode == CR01modif || mode==CR01_MO) { // pas de capteur de fin de course -> ouverture ou fermeture complète programmée pour 10s
        mvt_pos_fin = (mvt == OUVERT) ? OUVERT : 0;
        mvt_timerID = timer->after(10000, ElectroVanne::finMvt, this);
      } else { // sinon verification toutes les 0,5s de l'état de l'EV
        mvt_pos_fin = (mvt == OUVERT) ? OUVERT : 0;
        mvt_timerID = timer->every(500, ElectroVanne::finMvt, this);
      }
    } else {
      if ( position + mvt  >= 10 || position - mvt <= 0) { // le déplacement aboutis à une fermeture complète
        if ( mode == CR01|| mode == CR01modif || mode==CR01_MO) { // pas de capteur de fin de course -> ouverture ou fermeture complète programmée pour 10s
          mvt_pos_fin = (mvt == OUVERT) ? OUVERT : 0;
          mvt_timerID = timer->after(10000, ElectroVanne::finMvt, this);
        } else { // sinon verification toutes les 0,5s de l'état de l'EV
          mvt_pos_fin = (sens_mvt == OUVERT) ? OUVERT : 0;
          mvt_timerID = timer->every(500, ElectroVanne::finMvt, this);
        }
      } else { // ouverture/fermeture partielle
        mvt_pos_fin = position + mvt;
        mvt_timerID = timer->after(tempsOuvertureVanne / 10 * abs((unsigned int)mvt), ElectroVanne::finMvt, this);
      }
    }
  }

  switch (numero) {
    case 0:
#ifdef DEBUG
      Serial.print("init -- PORT_EV_0:");
      Serial.println(PORT_EV_0 & MASQUE_EV_0, BIN);
#endif
      PORT_EV_0 &= ~MASQUE_EV_0;       // mise des 2 broches à l'état bas (arrêt des 2 lignes)
#ifdef DEBUG
      Serial.print("mise à 0 -- PORT_EV_0:");
      Serial.println(PORT_EV_0 & MASQUE_EV_0, BIN);
#endif
      PORT_EV_0 |= (1 << (( sens_mvt == OUVERT) ? BIT_EV_0_O : BIT_EV_0_F)); // mise à l'état haut de la broche OUVERT ou mise à l'état haut de la broche FERME
#ifdef DEBUG
      Serial.print("activation -- PORT_EV_0:");
      Serial.println(PORT_EV_0 & MASQUE_EV_0, BIN);
#endif
      break;
#if NB_EV_LIGNE > 1
    case 1:
#ifdef DEBUG
      Serial.print("init -- PORT_EV_0:");
      Serial.println(PORT_EV_0 & MASQUE_EV_0, BIN);
#endif
      PORT_EV_1 &= ~MASQUE_EV_1;       // mise des 2 broches à l'état bas (arrêt des 2 lignes)
#ifdef DEBUG
      Serial.print("mise à 0 -- PORT_EV_0:");
      Serial.println(PORT_EV_0 & MASQUE_EV_0, BIN);
#endif
      PORT_EV_1 |= (1 << (( sens_mvt == OUVERT) ? BIT_EV_1_O : BIT_EV_1_F)); // mise à l'état haut de la broche OUVERT ou mise à l'état haut de la broche FERME
#ifdef DEBUG
      Serial.print("activation -- PORT_EV_0:");
      Serial.println(PORT_EV_0 & MASQUE_EV_0, BIN);
#endif
      break;
#endif
#if NB_EV_LIGNE > 2
    case 2:
      PORT_EV_2 &= ~MASQUE_EV_2;       // mise des 2 broches à l'état bas (arrêt des 2 lignes)
      PORT_EV_2 |= (1 << (( sens_mvt == OUVERT) ? BIT_EV_2_O : BIT_EV_2_F)); // mise à l'état haut de la broche OUVERT ou mise à l'état haut de la broche FERME
      break;
#endif
#if NB_EV_LIGNE > 3
    case 3:
      PORT_EV_3 &= ~MASQUE_EV_3;       // mise des 2 broches à l'état bas (arrêt des 2 lignes)
      PORT_EV_3 |= (1 << (( sens_mvt == OUVERT) ? BIT_EV_3_O : BIT_EV_3_F)); // mise à l'état haut de la broche OUVERT ou mise à l'état haut de la broche FERME
      break;
#endif
  }

  return true;
}


void ElectroVanne::finMvt(void* data) {
#ifdef DEBUG
  Serial.println("finMvt()");
#endif
  ElectroVanne* EV = (ElectroVanne*) data;

  if ( EV->mode == CR01 || EV->mode == CR01modif || EV->mode == CR01_MO) { // mode CR01 -> aucun moyen de savoir où on est!!
    EV->position = EV->mvt_pos_fin;
    timer->stop(EV->mvt_timerID);
  } else {

    if ( EV->mvt_pos_fin == OUVERT || EV->mvt_pos_fin == FERME) { // cas d'un déplacement complet
      if ( EV->etatEV() == EV->mvt_pos_fin ) { // le mouvement s'est terminé !!
        timer->stop(EV->mvt_timerID); // fin de l'appel régulier du suivi du mvt
        if (EV->mvt_pos_fin == FERME ) EV->position = 0; // enregistrement de la nouvelle position
        else EV->position = OUVERT;

        if ( EV->mvt_pos_deb == - EV->mvt_pos_fin) { // si la position de début est l'inverse de la position fin
          // on pourrais ici vérifier l'ordre de grandeur du temps d'ouverture de la vanne
          // A FAIRE
        }
      } else { // le mouvement n'est pas encore terminé
        if ( (millis() - EV->mvt_date) < (EV->tempsOuvertureVanne + EV->tempsOuvertureVanne / 5) ) { // si la durée de déplacement inférieur au temps normal + 20%
          return; // quitte pour revenir plus tard (au prochain appel du suivi du mvt)
        }
        // sinon il y a un problème !!
#ifdef DEBUG
        Serial.println("temps de mouvement de l'electrovanne beaucoup trop long ! ERREUR !");
#endif
        pileErreur->addErreur(numModuleErr,ERREUR_EV_TEMPS_TROP_LONG,EV->numero);
        EV->position = ERREUR_EV_TEMPS_TROP_LONG; // met une erreur dans la position
        timer->stop(EV->mvt_timerID); // fin de l'appel régulier du suivi du mvt
      }
    } else { // cas d'un déplacement partiel
      // on a aucun moyen de vérifier la position -> on considère qu'on y est !
      EV->position = EV->mvt_pos_fin;
      // dans ce cas, aucun timer à arreter ... juste à arreter le déplacement
    }
  }

  if( EV->mode != CR01_MO || EV->mvt_pos_fin != OUVERT || EV->mvt_futur != 0){
  // arrêt de tous les mouvements sur la cuve
    switch (EV->numero) {
      case 0:
  #ifdef DEBUG
        Serial.print("ElectroVanne::finMvt-avant -- PORT_EV_0:");
        Serial.println(PORT_EV_0 & MASQUE_EV_0, BIN);
  #endif
        PORT_EV_0 &= ~MASQUE_EV_0;       // mise des 2 broches à l'état bas (arrêt des 2 lignes)
  #ifdef DEBUG
        Serial.print("ElectroVanne::finMvt-apres -- PORT_EV_0:");
        Serial.println(PORT_EV_0 & MASQUE_EV_0, BIN);
  #endif
        break;
  #if NB_EV_MAX > 1
      case 1:
        PORT_EV_1 &= ~MASQUE_EV_1;       // mise des 2 broches à l'état bas (arrêt des 2 lignes)
        break;
  #endif
  #if NB_EV_MAX > 2
      case 2:
        PORT_EV_2 &= ~MASQUE_EV_2;       // mise des 2 broches à l'état bas (arrêt des 2 lignes)
        break;
  #endif
  #if NB_EV_MAX > 3
      case 3:
        PORT_EV_3 &= ~MASQUE_EV_3;       // mise des 2 broches à l'état bas (arrêt des 2 lignes)
        break;
  #endif
    }
  }
  // réinitialise le blocMvt de la ligne
  EV->mvt_timerID = 0;
  if (EV->mvt_futur != 0) { // si un mouvement futur est programmé
    EV->bougeEV(EV->mvt_futur);
    EV->mvt_futur = 0;
  }
}


uint8_t ElectroVanne::testEV() {
  switch (mode) {
#ifdef CR05
    case CR05:
      return testEVCR05_CR01modif();
#endif
#ifdef CR05_A
    case CR05_A:
      return testEVCR05_CR01modif();
#endif
#ifdef CR01modif
    case CR01modif:
      return testEVCR05_CR01modif();
#endif
#ifdef CR01
    case CR01:
      return testEVCR01();
#endif
#ifdef CR01_MO
    case CR01_MO:
      return testEVCR01();
#endif
#ifdef CR07
    case CR07:
      return testEVCR07();
#endif
  }
  return ERREUR_TYPE_EV_INCONNU;
}

uint8_t ElectroVanne::testEVCR01() {
  tempsOuvertureVanne = 0;
  return 0;
}

uint8_t ElectroVanne::testEVCR05_CR01modif() {
  /* test de l'électrovanne */

#ifdef DEBUG
  Serial.print("Test de l'electrovanne ");
  Serial.print(numero);
  Serial.print(" & capteur");
#endif

  int8_t etatVanne = etatEV();
  int8_t etatInitVanne = etatVanne;
  unsigned long date;
  tempsOuvertureVanne = 0;

  if (etatVanne == 0) { // position initiale ni fermé, ni ouvert -> ouverture de la vanne
    bougeEV(OUVERT);
    date = millis();
    do {
      timer->update();
      etatVanne = etatEV();
    } while (millis() - date < 11000 && etatVanne == 0); // on boucle pendant 11s ou jusqu'à que l'état de l'electrovanne change
    if (etatVanne == 0) { // l'état de l'electrovanne est encore à 0 --> l'electrovanne est bloqué ou mal branchée
#ifdef DEBUG
      Serial.println("Erreur lors de l'ouverture partielle de l'electrovanne. Aucun changement n'est observé. Electrovanne bloquée ou mal branchée ou capteur mal branché");
#endif
      return ERREUR_EV_AUCUN_MVT_OBS;
    }
    if ( etatVanne == FERME) { // l'état de l'electrovanne s'est fermée alors qu'on a demandé une ouverture... inversion soit des fils de test, soit des branchement electrovanne
#ifdef DEBUG
      Serial.println("Fermeture lors de l'ouverture de l'electrovanne. Electrovanne ou capteur inversés");
#endif
      return ERREUR_EV_EV_OU_CAPTEUR_INVERSE;
    }
#ifdef DEBUG
    Serial.println("ouverture partielle de l'electrovanne OK");
#endif
  }
  if (etatVanne == FERME) { // si la vanne est fermé, test de l'ouverture
    bougeEV(OUVERT);
    date = millis();
    do {
      timer->update();
      etatVanne = etatEV();
    } while (millis() - date < 11000 && etatVanne != OUVERT); // on boucle pendant 11s ou jusqu'à que l'état de l'electrovanne change

    tempsOuvertureVanne = (unsigned int)millis() - date; // calcul du temps d'ouverture de l'electrovanne


    if (etatVanne == FERME) { // l'état de l'electrovanne est encore à FERME --> l'electrovanne est bloqué ou mal branchée
#ifdef DEBUG
      Serial.println("L’electrovanne est encore fermé alors qu’une ouverture a été demandé. Electrovanne bloquée ou inversée.");
#endif
      return ERREUR_EV_OUVRE_PAS;
    }
    if ( etatVanne == 0) { // l'état de l'electrovanne est entre ouvert et ferme. soit capteur ouvert mal branché, soit electrovanne lente
#ifdef DEBUG
      Serial.println("Lors de l'ouverture, l'electrovanne semble s'être arrété en cours. Soit l'electroanne est lente (>10s), soit elle est bloqué, soit le capteur de l'état ouvert a un defaut.");
#endif
      return ERREUR_EV_OUVERTURE_BLOQUE_MILIEU;
    }

#ifdef DEBUG
    Serial.println("fermeture de l'electrovanne OK");
#endif
  }

  // test de la fermeture de la vanne
  bougeEV(FERME);
  date = millis();
  do {
    timer->update();
    etatVanne = etatEV();
  } while (millis() - date < 11000 && etatVanne != FERME); // on boucle pendant 11s ou jusqu'à que l'état de l'electrovanne change

  if ( tempsOuvertureVanne == 0) { // encore aucune mesure de temps d'electrovanne
    tempsOuvertureVanne = (unsigned int)(millis() - date);
  } else {
    tempsOuvertureVanne = (tempsOuvertureVanne + (unsigned int)(millis() - date)) / 2; // moyenne des temps d'ouverture/fermeture
  }


  if (etatVanne == OUVERT) { // l'état de l'electrovanne est encore à FERME --> l'electrovanne est bloqué ou mal branchée
#ifdef DEBUG
    Serial.println("L’electrovanne est encore ouverte alors qu’une fermeture a été demandée. Electrovanne bloquée ou inversée.");
#endif
    return ERREUR_EV_FERME_PAS;
  }
  if ( etatVanne == 0) { // l'état de l'electrovanne est entre ouvert et ferme. soit capteur ouvert mal branché, soit electrovanne lente
#ifdef DEBUG
    Serial.println("Lors de la fermeture, l’électrovanne semble s'être arrêtée en cours. Soit électrovanne est lente (>10s), soit elle est bloquée, soit le capteur de l'état fermé a un défaut.");
#endif
    return ERREUR_EV_FERMETURE_BLOQUE_MILIEU;
  }
#ifdef DEBUG
  Serial.println("fermeture de l'electrovanne OK");
#endif


  if (etatInitVanne != FERME) { // si l'on a pas déjà fait le test de l'ouverture
    bougeEV( OUVERT);
    date = millis();
    do {
      timer->update();
      etatVanne = etatEV();
    } while (millis() - date < 11000 && etatVanne != OUVERT); // on boucle pendant 11s ou jusqu'à que l'état de l'electrovanne change

    tempsOuvertureVanne = (tempsOuvertureVanne + (unsigned int)(millis() - date)) / 2; // moyenne des temps d'ouverture/fermeture


    if (etatVanne == FERME) { // l'état de l'electrovanne est encore à FERME --> l'electrovanne est bloqué ou mal branchée
#ifdef DEBUG
      Serial.println("L’electrovanne est encore fermé alors qu’une ouverture a été demandé. Electrovanne bloquée ou inversée.");
#endif
      return ERREUR_EV_OUVRE_PAS;
    }
    if ( etatVanne == 0) { // l'état de l'electrovanne est entre ouvert et ferme. soit capteur ouvert mal branché, soit electrovanne lente
#ifdef DEBUG
      Serial.println("Lors de l'ouverture, l'electrovanne semble s'être arrété en cours. Soit l'electroanne est lente (>10s), soit elle est bloqué, soit le capteur de l'état ouvert a un defaut.");
#endif
      return ERREUR_EV_OUVERTURE_BLOQUE_MILIEU;
    }
#ifdef DEBUG
    Serial.println("fermeture de l'electrovanne OK");
#endif
  }
#ifdef DEBUG
  Serial.print("temps de fermeture EV: ");
  Serial.print(tempsOuvertureVanne);
  Serial.println(" ms");
  Serial.print("Electrovanne ");
  Serial.print(numero);
  Serial.println(" & capteur OK");
#endif
  return 0;
}
