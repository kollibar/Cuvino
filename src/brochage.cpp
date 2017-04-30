#include "brochage.h"

unsigned char Brochage::brocheEtatEVouv(unsigned char num){
  switch (num) {
    case 0:
      return brocheEtatEV_0_O;
    case 1:
      return brocheEtatEV_1_O;
    case 2:
      return brocheEtatEV_2_O;
    case 3:
      return brocheEtatEV_3_O;
    default:
      return 0;
  }
}

unsigned char Brochage::brocheEVouv(unsigned char num){
  switch (num) {
    case 0:
      return brocheEV_0_O;
    case 1:
      return brocheEV_1_O;
    case 2:
      return brocheEV_2_O;
    case 3:
      return brocheEV_3_O;
    default:
      return 0;
  }
}

unsigned char Brochage::brocheSonde(unsigned char num){
  switch (num) {
    case 0:
      return brocheSonde_0;
    case 1:
      return brocheSonde_1;
    case 2:
      return brocheSonde_2;
    case 3:
      return brocheSonde_3;
    default:
      return 0;
  }
}

OneWire ds_0(Brochage::brocheSonde_0);
OneWire ds_1(Brochage::brocheSonde_1);
OneWire ds_2(Brochage::brocheSonde_2);
OneWire ds_3(Brochage::brocheSonde_3);

OneWire* Brochage::oneWireSonde(unsigned char num){
  switch (num) {
    case 0:
      return &ds_0;
    case 1:
      return &ds_1;
    case 2:
      return &ds_2;
    case 3:
      return &ds_3;
    default:
      return NULL;
  }
}

SondeV2 sonde_0(&ds_0,0);
SondeV2 sonde_1(&ds_1,1);
SondeV2 sonde_2(&ds_2,2);
SondeV2 sonde_3(&ds_3,3);
SondeV2 sonde_interne(NULL,SONDE_DS3231);

SondeV2* Brochage::sondeV2(signed char num){
  switch (num) {
    case 0:
      return &sonde_0;
    case 1:
      return &sonde_1;
    case 2:
      return &sonde_2;
    case 3:
      return &sonde_3;
    case SONDE_DS3231:
      return &sonde_interne;
    default:
      return NULL;
  }
}
