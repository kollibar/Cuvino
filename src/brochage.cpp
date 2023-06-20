#include "brochage.h"



#define NB_PORT_ONE_WIRE  5


/*
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
}*/

OneWire ds_0(Brochage::brocheSonde_0);
OneWire ds_1(Brochage::brocheSonde_1);
#if NB_PORT_ONE_WIRE > 2
OneWire ds_2(Brochage::brocheSonde_2);
#endif
#if NB_PORT_ONE_WIRE > 3
OneWire ds_3(Brochage::brocheSonde_3);
#endif
#if NB_PORT_ONE_WIRE > 4
OneWire ds_4(Brochage::brocheSonde_4);
#endif
#if NB_PORT_ONE_WIRE > 5
OneWire ds_5(Brochage::brocheSonde_5);
#endif

OneWire* Brochage::oneWireSonde(unsigned char num){
  switch (num) {
    case 0:
      return &ds_0;
    case 1:
      return &ds_1;
#if NB_PORT_ONE_WIRE > 2
    case 2:
      return &ds_2;
#endif
#if NB_PORT_ONE_WIRE > 5
    case 3:
      return &ds_3;
#endif
#if NB_PORT_ONE_WIRE > 5
    case 4:
      return &ds_4;
#endif
#if NB_PORT_ONE_WIRE > 5
    case 5:
      return &ds_5;
#endif
    default:
      return NULL;
  }
}

SondeV2 sonde_0(&ds_0,0);
SondeV2 sonde_1(&ds_1,1);
#if NB_PORT_ONE_WIRE > 2
SondeV2 sonde_2(&ds_2,2);
#endif
#if NB_PORT_ONE_WIRE > 3
SondeV2 sonde_3(&ds_3,3);
#endif
#if NB_PORT_ONE_WIRE > 4
SondeV2 sonde_4(&ds_4,4);
#endif
#if NB_PORT_ONE_WIRE > 5
SondeV2 sonde_5(&ds_5,5);
#endif
SondeV2 sonde_interne(NULL,SONDE_DS3231);

SondeV2* Brochage::sondeV2(signed char num){
  switch (num) {
    case 0:
      return &sonde_0;
    case 1:
      return &sonde_1;
#if NB_PORT_ONE_WIRE > 2
    case 2:
      return &sonde_2;
#endif
#if NB_PORT_ONE_WIRE > 3
    case 3:
      return &sonde_3;
#endif
#if NB_PORT_ONE_WIRE > 4
    case 4:
      return &sonde_4;
#endif
#if NB_PORT_ONE_WIRE > 5
    case 5:
      return &sonde_5;
#endif
    case SONDE_DS3231:
      return &sonde_interne;
    default:
      return NULL;
  }
}
