#include "circuit.h"



uint16_t Circuit::save(uint16_t addr) {
  eeprom_update_byte((uint8_t*)addr, this->nom); addr+=1;
  eeprom_update_byte((uint8_t*)addr, this->type); addr+=1;
  eeprom_update_byte((uint8_t*)addr, this->pin1); addr+=1;
  eeprom_update_byte((uint8_t*)addr, this->pin2); addr+=1;
  eeprom_update_byte((uint8_t*)addr, this->tempsAvantMesure); addr+=1;
  eeprom_update_byte((uint8_t*)addr, this->tempsEntre_pin1_pin2); addr+=1;
  eeprom_update_word((uint16_t*)addr, this->tempsAvantReset); addr+=2;
  eeprom_update_byte((uint8_t*)addr, this->pinReset); addr+=1;
  eeprom_update_word((uint16_t*)addr, this->temperatureReset); addr+=2;
  addr+=20;

  // EEPROM.write(addr, this->nom); ++addr;
  // EEPROM.write(addr, this->type); ++addr;
  // EEPROM.write(addr, this->pin1); ++addr;
  // EEPROM.write(addr, this->pin2); ++addr;
  // EEPROM.write(addr, this->tempsAvantMesure); ++addr;
  // EEPROM.write(addr, this->pinReset); ++addr;
  // addr = ecritEEPROM((byte *)&this->temperatureReset, 2, addr);
  // addr=reserveEEPROM(7, addr);

/*
  addr=ecritEEPROM((unsigned char*)&this->sonde->num, 1, addr);
  addr=ecritEEPROM((unsigned char*)&this->sonde->a, 1, addr);
  
*/
  return addr;
}

bool Circuit::save(BlocMem* bloc) {
  if ( ! bloc->ecrit((byte*)&this->nom, 1)) return false;
  if ( ! bloc->ecrit((byte*)&this->type, 1)) return false;
  if ( ! bloc->ecrit((byte*)&this->pin1, 1)) return false;
  if ( ! bloc->ecrit((byte*)&this->pin2, 1)) return false;
  if ( ! bloc->ecrit((byte*)&this->tempsAvantMesure, 1)) return false;
  if ( ! bloc->ecrit((byte*)&this->tempsEntre_pin1_pin2, 1)) return false;
  if ( ! bloc->ecrit((byte*)&this->tempsAvantReset, 2)) return false;
  if ( ! bloc->ecrit((byte*)&this->pinReset, 1)) return false;
  if ( ! bloc->ecrit((byte*)&this->temperatureReset, 2)) return false;
  bloc->reserve(20);
    /*
  if ( ! bloc->ecrit((byte*)&this->sonde->num, 1)) return false;
  if ( ! bloc->ecrit((byte*)&this->sonde->a, 1)) return false;
  if ( ! bloc->ecrit((byte*)&this->tempConsigneCuve, 2)) return false;
  */
  return true;
}

uint16_t Circuit::load(uint16_t addr) {
  this->defaut();
  // this->nom = EEPROM.read(addr); ++addr;
  // this->type = EEPROM.read(addr); ++addr;
  // this->pin1 = EEPROM.read(addr); ++addr;
  // this->pin2 = EEPROM.read(addr); ++addr;
  // this->tempsAvantMesure = EEPROM.read(addr); ++addr;
  // this->tempsEntre_pin1_pin2 = EEPROM.read(addr); ++addr;
  // this->tempsA = EEPROM.read(addr); ++addr;
  // this->pinReset = EEPROM.read(addr); ++addr;
  // addr = litEEPROM((byte*)&this->temperatureReset, 2, addr);
  // addr=reserveEEPROM(20, addr);
  this->nom=eeprom_read_byte((uint8_t*)addr); addr+=1;
  this->type=eeprom_read_byte((uint8_t*)addr); addr+=1;
  this->pin1=eeprom_read_byte((uint8_t*)addr); addr+=1;
  this->pin2=eeprom_read_byte((uint8_t*)addr); addr+=1;
  this->tempsAvantMesure=eeprom_read_byte((uint8_t*)addr); addr+=1;
  this->tempsEntre_pin1_pin2=eeprom_read_byte((uint8_t*)addr); addr+=1;
  this->tempsAvantReset= eeprom_read_word((uint16_t*)addr); addr+=2;
  this->pinReset=eeprom_read_byte((uint8_t*)addr); addr+=1;
  this->temperatureReset=eeprom_read_word((uint16_t*)addr); addr+=2;
  addr+=20;
/*
  addr=litEEPROM((unsigned char*)&sondeNum, 1, addr);
  addr=litEEPROM((unsigned char*)&this->sonde->b, 1, addr);
  
  */
  return addr;
}

bool Circuit::load(BlocMem* bloc) {
  this->defaut();
  if ( ! bloc->lit((byte*)&this->nom, 1)) return false;
  if ( ! bloc->lit((byte*)&this->type, 1)) return false;
  if ( ! bloc->lit((byte*)&this->pin1, 1)) return false;
  if ( ! bloc->lit((byte*)&this->pin2, 1)) return false;
  if ( ! bloc->lit((byte*)&this->tempsAvantMesure, 1)) return false;
  if ( ! bloc->lit((byte*)&this->tempsEntre_pin1_pin2, 1)) return false;
  if ( ! bloc->lit((byte*)&this->tempsAvantReset, 2)) return false;
  if ( ! bloc->lit((byte*)&this->pinReset, 1)) return false;
  if ( ! bloc->lit((byte*)&this->temperatureReset, 2)) return false;
  bloc->reserve(20);
  /*
  if ( ! bloc->lit((byte*)&sondeNum, 1)) return false;
  if ( ! bloc->lit((byte*)&this->sonde->a, 1)) return false;
  
  */
  return true;
}

void Circuit::defaut(){
    this->sonde=NULL;

    this->nom=' ';
    this->type=CIRCUIT_TYPE_TJR_OUI;
    this->pin1=255;
    this->pin2=255;
    this->pinReset=255;
    this->tempsAvantMesure=0;
    this->tempsEntre_pin1_pin2=0;
    this->temperatureReset=0xFFFF;
}

void Circuit::ouvertureEV(){
    if( this->nbEVOuverte == 0) this->ouverturePremiereEV();
    this->nbEVOuverte++;
}


void Circuit::fermetureEV(){
    this->nbEVOuverte--;
    if( this->nbEVOuverte == 0) this->fermetureDerniereEV();
}


void Circuit::fermetureDerniereEV(){

}

void Circuit::ouverturePremiereEV(){

}

bool Circuit::estPret(uint16_t temperatureDemande){
    if( this->type == CIRCUIT_TYPE_TJR_OUI ) return true;   // circuit "toujours OUI"

    return false;
}


unsigned long Circuit::timeToWait(){
  if( this->sonde == NULL ) return 0;
  else return this->sonde->timeToWait();
}

bool Circuit::demandeMesureTemp(){
  if( this->sonde == NULL ) return TEMP_ERREUR;
  if( ! this->sonde->isSondeTemp() ) return TEMP_ERREUR;
  return this->sonde->demandeMesureTemp();
}

int16_t Circuit::getTemperature(){
    /*
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

  mesure=this->sonde->getTemperature();
  dateMesure=millis();

  return mesure;*/
}



void Circuit::setDureeMiniAvantMesure(uint8_t s){
    this->tempsAvantMesure=s;
}
void Circuit::setDureeEntreActivation1et2(uint8_t s){
    this->tempsEntre_pin1_pin2=s;
}
uint8_t Circuit::getDureeMiniAvantMesure(){
    return this->tempsAvantMesure;
}
uint8_t Circuit::getDureeEntreActivation1et2(){
    return this->tempsEntre_pin1_pin2;
}

void Circuit::setPin(uint8_t numPin, uint8_t typePin){
    switch(typePin){
        case 1:
            this->pin1=numPin;
            break;
        case 2:
            this->pin2=numPin;
            break;
        case 3:
            this->pinReset=numPin;
            break;
    }
}
uint8_t Circuit::getPin(uint8_t typePin){
    switch(typePin){
        case 1:
            return this->pin1;
        case 2:
            return this->pin2;
        case 3:
            return this->pinReset;
    }
    return 255;
}
uint8_t Circuit::getNom(){
    return this->nom;
}
void Circuit::setNom(uint8_t nom){
    this->nom=nom;
}
void Circuit::setType(uint8_t type){
    this->type=type;
}
uint8_t Circuit::getType(){
    return this->type;
}
void Circuit::setTemperatureReset(int16_t temp){
    this->temperatureReset=temp;
}
int16_t Circuit::getTemperatureReset(){
    return this->temperatureReset;
}

uint8_t Circuit::getNbEVOuvert(){
    return this->nbEVOuverte;
}
