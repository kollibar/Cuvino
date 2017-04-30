#include "EV2.h"

DebugLogger debugEV=DebugLogger(EV2_DEBUG_nom);

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


uint8_t ElectroVanne2::numModuleErr=0;
PileErreur* ElectroVanne2::pileErreur=NULL;
Timer* ElectroVanne2::timer=NULL;
OneWire* ElectroVanne2::ds=NULL;
QueueHandle_t ElectroVanne2::qCmdeToCore=NULL;
QueueHandle_t ElectroVanne2::qCmdeToTimer=NULL;

void ElectroVanne2::begin(PileErreur& _pileErreur,Timer& _timer,OneWire& _ds,QueueHandle_t queueCmdeToCore,QueueHandle_t queueCmdeToTimer){
  pileErreur=&_pileErreur;
  timer=&_timer;
  ds=&_ds;
  qCmdeToCore=queueCmdeToCore;
  qCmdeToTimer=queueCmdeToTimer;
  if( numModuleErr==0) numModuleErr=_pileErreur.getNumModuleSuiv();
}

void ElectroVanne2::print(HardwareSerial& serial){
  serial.print("ElectroVanne2(num:");

  serial.print(numero);
  serial.print(", mode:");
  serial.print(mode);

  serial.print(')');
}

void ElectroVanne2::print(DebugLogger& debug){
  debug.printPGM(FR::TXT_ELECTROVANNE,true);
  debug.print("2(num:");

  debug.print(numero);
  debug.print(", ");
  debug.printPGM(FR::TXT_MODE);
  debug.print(':');
  debug.print(mode);

  debug.print(')');
}

ElectroVanne2 &ElectroVanne2::operator=(const ElectroVanne2 &source){
  if (&source != this){
    this->numero=source.numero;
    this->mode=source.mode;
    this->brocheEtatEV=source.brocheEtatEV;
    this->brocheEV=source.brocheEV;

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

uint16_t ElectroVanne2::save(uint16_t addr) {
  addr = ecritEEPROM((uint8_t *)&this->tempsOuvertureVanne, 2, addr);
  EEPROM.write(addr, this->mode); ++addr;
  EEPROM.write(addr, this->numero); ++addr;
  return addr;
}

bool ElectroVanne2::save(BlocMem* bloc) {
  if ( ! bloc->ecrit((uint8_t*)&this->tempsOuvertureVanne, 2)) return false;
  if ( ! bloc->ecrit((uint8_t*)&this->mode, 1)) return false;
  if ( ! bloc->ecrit((uint8_t*)&this->numero, 1)) return false;
  return true;
}

bool ElectroVanne2::load(BlocMem* bloc) {
  if ( ! bloc->lit((uint8_t*)&this->tempsOuvertureVanne, 2)) return false;
  if ( ! bloc->lit((uint8_t*)&this->mode, 1)) return false;
  if ( ! bloc->lit((uint8_t*)&this->numero, 1)) return false;
  this->loadPort();
  return true;
}

uint16_t ElectroVanne2::load(uint16_t addr) {
  this->defaut();
  addr = litEEPROM((uint8_t *)&this->tempsOuvertureVanne, 2, addr);
  this->mode = EEPROM.read(addr); ++addr;
  this->numero = EEPROM.read(addr); ++addr;
  this->loadPort();
  return addr;
}

void ElectroVanne2::defaut(void) {
  this->tempsOuvertureVanne = 0;
  this->position = 100;
  this->mode = EV_NON_CONFIGURE;
  this->numero = 255;

  this->brocheEtatEV=0;
  this->brocheEV=0;

  this->mvt_timerID = 0;
  this->mvt_futur = 0;
}

bool ElectroVanne2::loadPort(){
  switch(this->numero){
    case 0:
      this->brocheEV=BROCHE_EV_0_O;
      this->brocheEtatEV=BROCHE_E_EV_0_O;
      break;
    case 1:
      this->brocheEV=BROCHE_EV_1_O;
      this->brocheEtatEV=BROCHE_E_EV_1_O;
      break;
    case 2:
      this->brocheEV=BROCHE_EV_2_O;
      this->brocheEtatEV=BROCHE_E_EV_2_O;
      break;
    case 3:
      this->brocheEV=BROCHE_EV_3_O;
      this->brocheEtatEV=BROCHE_E_EV_3_O;
      break;
    default:
      return false;
  }
  return true;
}

bool ElectroVanne2::estConfigure() const {
  return (mode != EV_NON_CONFIGURE);
}


void ElectroVanne2::initBroches() {
  if( mode==EV_NON_CONFIGURE) return;

  if( (mode & EV_CMD_2_FILS)!=0) { // si commande sur 2 fils
    // mise des 2 broches en sorties
    pinMode(brocheEV, OUTPUT);
    pinMode(brocheEV + 1, OUTPUT);

    // mise à 0 des broches
    digitalWrite(brocheEV, LOW);
    digitalWrite(brocheEV + 1, LOW);
  } else  { // commande sur 1 fils
    pinMode(brocheEV, OUTPUT);

    // mise à 0 de la broche
    digitalWrite(brocheEV, LOW);
  }

  if( (mode & EV_ETAT_2_FILS)!=0) { // si etat EV sur 2 fils
    // mise des 2 broches de l'état EV en entré
    pinMode(brocheEtatEV, INPUT);
    digitalWrite(brocheEtatEV, HIGH); // ajout pull-up

    pinMode(brocheEtatEV + 1, INPUT);
    digitalWrite(brocheEtatEV + 1, HIGH); // ajout pull-up
  } else if ( (mode & EV_ETAT_ANALOGIQUE)!=0){
    // mise de la broche en entré
    pinMode(brocheEtatEV, INPUT);
    digitalWrite(brocheEtatEV, LOW); // sans pull-up
  }

}

void ElectroVanne2::initEV() {
  if ( mode == EV_NON_CONFIGURE) return;

  loadPort();

  initBroches();
  if ( mode != EV_NON_CONFIGURE) {
#ifdef DEBUG
    debugEV.printPGM(EV2_DEBUG_FERMETURE);
    debugEV.write(' ');
    debugEV.printPGM(EV2_DEBUG_electrovanne);
    debugEV.write(' ');
    debugEV.println(numero);
#endif
    bougeEV(FERME);
  }
}

int8_t ElectroVanne2::etatEV() {
  #ifdef DEBUG
    debugEV.printlnPGM(EV2_DEBUG_etatEV);
  #endif
  if( (mode & EV_ETAT_2_FILS)!=0){
    #ifdef DEBUG
      debugEV.setWriteMode(modeDebugComplet);
      debugEV.printlnPGM(EV2_DEBUG_etat2fils);
      debugEV.printPGM(EV2_DEBUG_digitalRead);
      debugEV.print(brocheEtatEV);
      debugEV.write("):");
      if(digitalRead(brocheEtatEV) ) debugEV.printPGM(EV2_DEBUG_HIGH);
      else debugEV.printPGM(EV2_DEBUG_LOW);

      debugEV.write(' ');
      debugEV.printPGM(EV2_DEBUG_digitalRead);
      debugEV.print(brocheEtatEV);
      debugEV.printPGM(EV2_DEBUG_PLUS1);
      debugEV.write("):");
      if(digitalRead(brocheEtatEV+1) ) debugEV.printPGM(EV2_DEBUG_HIGH);
      else debugEV.printPGM(EV2_DEBUG_LOW);

      debugEV.printPGM(EV2_DEBUG_FLECHE);
      if(digitalRead(brocheEtatEV)==0 ) debugEV.printlnPGM(EV2_DEBUG_OUVERT);
      else if(digitalRead(brocheEtatEV+1)==0 ) debugEV.printlnPGM(EV2_DEBUG_FERME);
      else debugEV.printlnPGM(EV2_DEBUG_PARTIEL);
      debugEV.setWriteMode(modeDebug);
    #endif
    if(digitalRead(brocheEtatEV) == LOW) position=OUVERT;
    else if(digitalRead(brocheEtatEV + 1 ) == LOW) position=FERME;
    else position=0;

    return position;
  }
  if( (mode & EV_ETAT_ANALOGIQUE)!=0){
    if(digitalRead(brocheEtatEV) == LOW) return 0;
    uint16_t val = analogRead(brocheEtatEV);
    if ( val < 500) position=0;
    else if ( val < 850 ) position=FERME;
    else position=OUVERT;

    return position;
  }
  if( (mode & EV_ETAT_SUR_CMD)!=0){

    // A FAIRE
  }
  return 0;
}

/*
int8_t ElectroVanne2::etatEVCR01modif() {
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
*/

bool ElectroVanne2::ouvre(){
  return bougeEV(OUVERT);
}

bool ElectroVanne2::ferme(){
  return bougeEV(FERME);
}

bool ElectroVanne2::bougeEV(int8_t mvt) {
  return bougeEVtotal(mvt);
#ifdef DEBUG
  debugEV.printPGM(EV2_DEBUG_bougeEV);
  debugEV.print(numero);
  debugEV.print(',');
  debugEV.print(mvt);
  debugEV.print('-');
  if( mvt == FERME ) debugEV.printPGM(EV2_DEBUG_FERME);
  else if( mvt == OUVERT) debugEV.printPGM(EV2_DEBUG_OUVERT);
  else debugEV.printPGM(EV2_DEBUG_AUTRE);
  debugEV.println(')');
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

void ElectroVanne2::repos(){ // position de repos de l'EV
#ifdef DEBUG
  debugEV.printPGM(EV2_DEBUG_repos);
  debugEV.print(numero);
  debugEV.println(')');

  debugEV.printPGM(FR::TXT_MODE);
  debugEV.write(':');
  debugEV.print(mode,BIN);

  debugEV.write(' ');
  debugEV.print("EV_CMD_2_FILS:");
  debugEV.print(EV_CMD_2_FILS,BIN);
  debugEV.write(' ');
  debugEV.print("mode & EV_CMD_2_FILS:");
  debugEV.println(mode & EV_CMD_2_FILS,BIN);

#endif
  if( (mode & EV_CMD_2_FILS )!=0 ){ // commande sur 2 fils
    digitalWrite(brocheEV, LOW);
    digitalWrite(brocheEV + 1, LOW);
    #ifdef DEBUG
    debugEV.printPGM(EV2_DEBUG_digitalWrite);
    debugEV.print(brocheEV);
    debugEV.write(',');
    debugEV.printPGM(EV2_DEBUG_LOW);
    debugEV.print(')');
    debugEV.print(' ');
    debugEV.printPGM(EV2_DEBUG_digitalWrite);
    debugEV.print(brocheEV);
    debugEV.printPGM(EV2_DEBUG_PLUS1);
    debugEV.write(',');
    debugEV.printPGM(EV2_DEBUG_LOW);
    debugEV.println(')');
    #endif
  } else{
    digitalWrite(brocheEV, LOW);
    #ifdef DEBUG
    debugEV.printPGM(EV2_DEBUG_digitalWrite);
    debugEV.print(brocheEV);
    debugEV.write(',');
    debugEV.printPGM(EV2_DEBUG_HIGH);
    debugEV.println(')');
    #endif
  }
}

bool ElectroVanne2::bougeEVtotal(int8_t mvt){
  const cmdeInterne cmde=cmdeActEtatTouteEV;
  #ifdef DEBUG
    debugEV.printPGM(EV2_DEBUG_bougEVtotal);
    debugEV.print(numero);
    debugEV.print(',');
    debugEV.print(mvt);
    debugEV.print('-');
    if( mvt == FERME ) debugEV.printPGM(EV2_DEBUG_FERME);
    else if( mvt == OUVERT) debugEV.printPGM(EV2_DEBUG_OUVERT);
    else debugEV.printPGM(EV2_DEBUG_AUTRE);
    debugEV.println(')');
  #endif

  if( mvt == 0 ) return true; // pas de mvt
  if( mvt != OUVERT && mvt != FERME) return false; // mvt incompatible

  if( mvt == etatEV() ) return true; // si l'EV est déjà dans la position demandée

  if( mvt_timerID != 0 ){ // mvt en cours
    timer->stop(mvt_timerID); // on stop l'attente de fin du mvt
    mvt_timerID = 0;
  }

  repos(); // mise au repos de l'EV

  if( (mode & EV_CMD_2_FILS )!=0 ){ // commande sur 2 fils

    if( mvt == FERME) digitalWrite(brocheEV + 1, HIGH);
    else digitalWrite(brocheEV, HIGH);

    mvt_date=millis();

    #ifdef DEBUG
      if( mvt == FERME) {
        debugEV.printPGM(EV2_DEBUG_FERMETURE);
        debugEV.write(' ');
        debugEV.printPGM(EV2_DEBUG_digitalWrite);
        debugEV.print(brocheEV);
        debugEV.printPGM(EV2_DEBUG_PLUS1);
        debugEV.write(',');
        debugEV.printPGM(EV2_DEBUG_HIGH);
        debugEV.println(')');
      }else{
        debugEV.printPGM(EV2_DEBUG_OUVERTURE);
        debugEV.write(' ');
        debugEV.printPGM(EV2_DEBUG_digitalWrite);
        debugEV.print(brocheEV);
        debugEV.write(',');
        debugEV.printPGM(EV2_DEBUG_HIGH);
        debugEV.println(')');
      }
    #endif

    if( mvt == OUVERT) mvt_pos_fin=OUVERT;
    else mvt_pos_fin=FERME;

    // déclenchement du timer pour arrêt ou controle
    if ( (mode & ( EV_ETAT_SUR_CMD | EV_ETAT_2_FILS | EV_ETAT_ANALOGIQUE)) == 0) { // pas de capteur de fin de course -> ouverture ou fermeture complète programmée pour 10s
      mvt_timerID = timer->after(10000, ElectroVanne2::finMvtTotal, this);
    } else { // sinon verification toutes les 0,5s de l'état de l'EV
      mvt_timerID = timer->every(500, ElectroVanne2::finMvtTotal, this);
    }
    this->position=0;

  } else {
    if( (mode& MAINTIENT_OUVETURE)!=0 && mvt == OUVERT) digitalWrite(brocheEV,HIGH);
    else if( (mode& MAINTIENT_FERMETURE)!=0 && mvt == FERME) digitalWrite(brocheEV,HIGH);
    this->position=mvt;
  }
  xQueueSend(qCmdeToTimer,&cmde,0); // actualisation de la position des EV !
  return true;
}

void ElectroVanne2::finMvtTotal(void* data) {
  ElectroVanne2* EV = (ElectroVanne2*) data;
  const cmdeInterne cmde=cmdeActEtatTouteEV;

#ifdef DEBUG
  debugEV.printPGM(EV2_DEBUG_finMvtTotal);
  debugEV.print(EV->numero);
  debugEV.println(')');
#endif

  if ( (EV->mode & ( EV_ETAT_SUR_CMD | EV_ETAT_2_FILS | EV_ETAT_ANALOGIQUE)) == 0) { // pas de capteur de fin de course -> aucun moyen de savoir où on est!!
    if (EV->mvt_pos_fin == FERME ) EV->position = FERME; // enregistrement de la nouvelle position
    else EV->position = OUVERT;

    if(EV->mvt_timerID!=0) timer->stop(EV->mvt_timerID);
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
#ifdef DEBUG
      debugEV.printlnPGM(EV2_DEBUG_TMP_OUV_BCP_TROP_LONG);
      debugEV.printPGM(FR::TXT_TEMP);
      debugEV.write(' ');
      debugEV.printPGM(EV2_DEBUG_OUVERTURE);
      debugEV.write(':');
      debugEV.println((millis() - EV->mvt_date));
      debugEV.printPGM(EV2_DEBUG_tempsOuvertureVanne);
      debugEV.println(EV->tempsOuvertureVanne);
#endif
      pileErreur->addErreur(numModuleErr,ERREUR_EV_TEMPS_TROP_LONG,EV->numero);
      EV->position = ERREUR_EV_TEMPS_TROP_LONG; // met une erreur dans la position
      timer->stop(EV->mvt_timerID); // fin de l'appel régulier du suivi du mvt
      EV->mvt_timerID = 0;

      xQueueSend(qCmdeToTimer,&cmde,0); // actualisation de la position des EV !
    }
  }
}

/*
bool ElectroVanne2::bougeEVpartiel(int8_t mvt) {
#ifdef DEBUG
  debugEV.print("bougeEV(");
  debugEV.print(numero);
  debugEV.print(',');
  debugEV.print(mvt);
  debugEV.println(')');
#endif
  if (mvt == 0) return true; // aucun mvt demandé
  uint8_t sens_mvt;
  if ( mvt < 0) sens_mvt = -10;
  else sens_mvt = 10;

  mvt_date = millis();
  mvt_pos_deb = position;

  if ( tempsOuvertureVanne == 0) { // si la mesure de durée d'ouverture n'a pas été faite
    if ( mode == CR01 || mode == CR01modif || mode==CR01_MO) {
      mvt_pos_fin = (sens_mvt == OUVERT) ? OUVERT : 0; // si on ne connais pas la durée, on ne peut faire qu'une ouverture ou fermeture complète
      mvt_timerID = timer->after(10000, ElectroVanne2::finMvt, this);
    } else {
      mvt_pos_fin = (sens_mvt == OUVERT) ? OUVERT : 0; // si on ne connais pas la durée, on ne peut faire qu'une ouverture ou fermeture complète
      mvt_timerID = timer->every(500, ElectroVanne2::finMvt, this);
    }
  } else {
    if ( mvt == OUVERT || mvt == FERME) { // si ouverture ou fermeture complète
      if ( mode == CR01|| mode == CR01modif || mode==CR01_MO) { // pas de capteur de fin de course -> ouverture ou fermeture complète programmée pour 10s
        mvt_pos_fin = (mvt == OUVERT) ? OUVERT : 0;
        mvt_timerID = timer->after(10000, ElectroVanne2::finMvt, this);
      } else { // sinon verification toutes les 0,5s de l'état de l'EV
        mvt_pos_fin = (mvt == OUVERT) ? OUVERT : 0;
        mvt_timerID = timer->every(500, ElectroVanne2::finMvt, this);
      }
    } else {
      if ( position + mvt  >= 10 || position - mvt <= 0) { // le déplacement aboutis à une fermeture complète
        if ( mode == CR01|| mode == CR01modif || mode==CR01_MO) { // pas de capteur de fin de course -> ouverture ou fermeture complète programmée pour 10s
          mvt_pos_fin = (mvt == OUVERT) ? OUVERT : 0;
          mvt_timerID = timer->after(10000, ElectroVanne2::finMvt, this);
        } else { // sinon verification toutes les 0,5s de l'état de l'EV
          mvt_pos_fin = (sens_mvt == OUVERT) ? OUVERT : 0;
          mvt_timerID = timer->every(500, ElectroVanne2::finMvt, this);
        }
      } else { // ouverture/fermeture partielle
        mvt_pos_fin = position + mvt;
        mvt_timerID = timer->after(tempsOuvertureVanne / 10 * abs((unsigned int)mvt), ElectroVanne2::finMvt, this);
      }
    }
  }

  switch (numero) {
    case 0:
#ifdef DEBUG
      debugEV.print("init -- PORT_EV_0:");
      debugEV.println(PORT_EV_0 & MASQUE_EV_0, BIN);
#endif
      PORT_EV_0 &= ~MASQUE_EV_0;       // mise des 2 broches à l'état bas (arrêt des 2 lignes)
#ifdef DEBUG
      debugEV.print("mise à 0 -- PORT_EV_0:");
      debugEV.println(PORT_EV_0 & MASQUE_EV_0, BIN);
#endif
      PORT_EV_0 |= (1 << (( sens_mvt == OUVERT) ? BIT_EV_0_O : BIT_EV_0_F)); // mise à l'état haut de la broche OUVERT ou mise à l'état haut de la broche FERME
#ifdef DEBUG
      debugEV.print("activation -- PORT_EV_0:");
      debugEV.println(PORT_EV_0 & MASQUE_EV_0, BIN);
#endif
      break;
#if NB_EV_LIGNE > 1
    case 1:
#ifdef DEBUG
      debugEV.print("init -- PORT_EV_0:");
      debugEV.println(PORT_EV_0 & MASQUE_EV_0, BIN);
#endif
      PORT_EV_1 &= ~MASQUE_EV_1;       // mise des 2 broches à l'état bas (arrêt des 2 lignes)
#ifdef DEBUG
      debugEV.print("mise à 0 -- PORT_EV_0:");
      debugEV.println(PORT_EV_0 & MASQUE_EV_0, BIN);
#endif
      PORT_EV_1 |= (1 << (( sens_mvt == OUVERT) ? BIT_EV_1_O : BIT_EV_1_F)); // mise à l'état haut de la broche OUVERT ou mise à l'état haut de la broche FERME
#ifdef DEBUG
      debugEV.print("activation -- PORT_EV_0:");
      debugEV.println(PORT_EV_0 & MASQUE_EV_0, BIN);
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


void ElectroVanne2::finMvt(void* data) {
#ifdef DEBUG
  debugEV.println("finMvt()");
#endif
  ElectroVanne2* EV = (ElectroVanne2*) data;

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
        debugEV.println("temps de mouvement de l'electrovanne beaucoup trop long ! ERREUR !");
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
        debugEV.print("ElectroVanne2::finMvt-avant -- PORT_EV_0:");
        debugEV.println(PORT_EV_0 & MASQUE_EV_0, BIN);
  #endif
        PORT_EV_0 &= ~MASQUE_EV_0;       // mise des 2 broches à l'état bas (arrêt des 2 lignes)
  #ifdef DEBUG
        debugEV.print("ElectroVanne2::finMvt-apres -- PORT_EV_0:");
        debugEV.println(PORT_EV_0 & MASQUE_EV_0, BIN);
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
*/



uint8_t ElectroVanne2::testEV() {
  if ( (mode & ( EV_ETAT_SUR_CMD | EV_ETAT_2_FILS | EV_ETAT_ANALOGIQUE)) == 0) { // pas de capteur de fin de courses -> test impossible
    tempsOuvertureVanne = 0;
    return 0;
  }

  /* test de l'électrovanne */

#ifdef DEBUG
  debugEV.printPGM(EV2_DEBUG_TEST_EV);
  debugEV.print(numero);
  debugEV.printlnPGM(EV2_DEBUG_ET_CAPTEUR);
#endif

  int8_t etatVanne = etatEV();
  int8_t etatInitVanne = etatVanne;
  unsigned long date;
  tempsOuvertureVanne = 0;

  if (etatVanne != FERME && etatVanne != OUVERT) { // position initiale ni fermé, ni ouvert -> ouverture de la vanne
    bougeEV(OUVERT);
    date = millis();
    do {
      timer->update();

      if( xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED ){
        delay(100); // si le noyau FreeRTOS est inactif
      } else {
        vTaskDelay( 100 / portTICK_PERIOD_MS ); // si le noyau FreeRTOS est actif
      }

      etatVanne = etatEV();
    } while (millis() - date < 11000 && etatVanne != FERME && etatVanne != OUVERT); // on boucle pendant 11s ou jusqu'à que l'état de l'electrovanne change
    if (etatVanne == 0) { // l'état de l'electrovanne est encore à 0 --> l'electrovanne est bloqué ou mal branchée
#ifdef DEBUG
      debugEV.printlnPGM(EV2_DEBUG_ERR_AUCUN_MVT);
#endif
      return ERREUR_EV_AUCUN_MVT_OBS;
    }
    if ( etatVanne == FERME) { // l'état de l'electrovanne s'est fermée alors qu'on a demandé une ouverture... inversion soit des fils de test, soit des branchement electrovanne
#ifdef DEBUG
      debugEV.printlnPGM(EV2_DEBUG_ERR_EV_CAPT_INV);
#endif
      return ERREUR_EV_EV_OU_CAPTEUR_INVERSE;
    }
#ifdef DEBUG
    debugEV.printlnPGM(EV2_DEBUG_OUV_PART_OK);
#endif
  }
  if (etatVanne == FERME) { // si la vanne est fermé, test de l'ouverture
    bougeEV(OUVERT);
    date = millis();
    do {
      timer->update();

      if( xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED ){
        delay(100); // si le noyau FreeRTOS est inactif
      } else {
        vTaskDelay( 100 / portTICK_PERIOD_MS ); // si le noyau FreeRTOS est actif
      }

      etatVanne = etatEV();
    } while (millis() - date < 11000 && etatVanne != OUVERT); // on boucle pendant 11s ou jusqu'à que l'état de l'electrovanne change

    tempsOuvertureVanne = (unsigned int)(millis() - date); // calcul du temps d'ouverture de l'electrovanne


    if (etatVanne == FERME) { // l'état de l'electrovanne est encore à FERME --> l'electrovanne est bloqué ou mal branchée
#ifdef DEBUG
      debugEV.printlnPGM(EV2_DEBUG_ERR_EV_OUVRE_PAS);
#endif
      return ERREUR_EV_OUVRE_PAS;
    }
    if ( etatVanne == 0) { // l'état de l'electrovanne est entre ouvert et ferme. soit capteur ouvert mal branché, soit electrovanne lente
#ifdef DEBUG
      debugEV.printlnPGM(EV2_DEBUG_ERR_EV_OUVERTURE_BLOQUE_MILIEU);
#endif
      return ERREUR_EV_OUVERTURE_BLOQUE_MILIEU;
    }

#ifdef DEBUG
    debugEV.printlnPGM(EV2_DEBUG_FERME_EV_OK);
#endif
  }

  // test de la fermeture de la vanne
  bougeEV(FERME);
  date = millis();
  do {
    timer->update();

    if( xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED ){
      delay(100); // si le noyau FreeRTOS est inactif
    } else {
      vTaskDelay( 100 / portTICK_PERIOD_MS ); // si le noyau FreeRTOS est actif
    }

    etatVanne = etatEV();
  } while (millis() - date < 11000 && etatVanne != FERME); // on boucle pendant 11s ou jusqu'à que l'état de l'electrovanne change

  if ( tempsOuvertureVanne == 0) { // encore aucune mesure de temps d'electrovanne
    tempsOuvertureVanne = (unsigned int)(millis() - date);
  } else {
    tempsOuvertureVanne = (tempsOuvertureVanne + (unsigned int)(millis() - date)) / 2; // moyenne des temps d'ouverture/fermeture
  }


  if (etatVanne == OUVERT) { // l'état de l'electrovanne est encore à FERME --> l'electrovanne est bloqué ou mal branchée
#ifdef DEBUG
    debugEV.printlnPGM(EV2_DEBUG_ERR_EV_FERME_PAS);
#endif
    return ERREUR_EV_FERME_PAS;
  }
  if ( etatVanne == 0) { // l'état de l'electrovanne est entre ouvert et ferme. soit capteur ouvert mal branché, soit electrovanne lente
#ifdef DEBUG
    debugEV.printlnPGM(EV2_DEBUG_ERR_EV_FERMETURE_BLOQUE_MILIEU);
#endif
    return ERREUR_EV_FERMETURE_BLOQUE_MILIEU;
  }
#ifdef DEBUG
  debugEV.printlnPGM(EV2_DEBUG_FERME_EV_OK);
#endif


  if (etatInitVanne != FERME) { // si l'on a pas déjà fait le test de l'ouverture
    bougeEV( OUVERT);
    date = millis();
    do {
      timer->update();

      if( xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED ){
        delay(100); // si le noyau FreeRTOS est inactif
      } else {
        vTaskDelay( 100 / portTICK_PERIOD_MS ); // si le noyau FreeRTOS est actif
      }

      etatVanne = etatEV();
    } while (millis() - date < 11000 && etatVanne != OUVERT); // on boucle pendant 11s ou jusqu'à que l'état de l'electrovanne change

    tempsOuvertureVanne = (tempsOuvertureVanne + (unsigned int)(millis() - date)) / 2; // moyenne des temps d'ouverture/fermeture


    if (etatVanne == FERME) { // l'état de l'electrovanne est encore à FERME --> l'electrovanne est bloqué ou mal branchée
#ifdef DEBUG
      debugEV.printlnPGM(EV2_DEBUG_ERR_EV_OUVRE_PAS);
#endif
      return ERREUR_EV_OUVRE_PAS;
    }
    if ( etatVanne == 0) { // l'état de l'electrovanne est entre ouvert et ferme. soit capteur ouvert mal branché, soit electrovanne lente
#ifdef DEBUG
      debugEV.printlnPGM(EV2_DEBUG_ERR_EV_OUVERTURE_BLOQUE_MILIEU);
#endif
      return ERREUR_EV_OUVERTURE_BLOQUE_MILIEU;
    }
#ifdef DEBUG
    debugEV.printlnPGM(EV2_DEBUG_FERME_EV_OK);
#endif
  }
#ifdef DEBUG
  debugEV.printPGM(EV2_DEBUG_tempsOuvertureVanne);
  debugEV.print(tempsOuvertureVanne);
  debugEV.write(' ');
  debugEV.printlnPGM(EV2_DEBUG_MS);
  debugEV.printPGM(EV2_DEBUG_electrovanne);
  debugEV.write(' ');
  debugEV.print(numero);
  debugEV.printPGM(EV2_DEBUG_ET_CAPTEUR);
  debugEV.printlnPGM(EV2_DEBUG_OK);
#endif
  return 0;
}
