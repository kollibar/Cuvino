#ifndef __BROCHAGE_H__
#define __BROCHAGE_H__

/** FreeRTOS **/
#include <Arduino_FreeRTOS.h>
#include <projdefs.h>
#include <portmacro.h>
#include <list.h>
#include <timers.h>
#include <event_groups.h>
#include <semphr.h>
#include <FreeRTOSVariant.h>
#include <queue.h>
#include <FreeRTOSConfig.h>
#include <mpu_wrappers.h>
#include <task.h>
#include <portable.h>
/** !FreeRTOS **/

#include <Arduino.h>
#include <OneWire.h>
#include "SondeV2.h"

namespace Brochage{
  SondeV2* sondeV2(signed char num);
  /*
  unsigned char brocheEVouv(unsigned char num);
  unsigned char brocheEtatEVouv(unsigned char num);
  unsigned char brocheSonde(unsigned char num);*/
  OneWire* oneWireSonde(unsigned char num);


  const unsigned nbEV = 9;
  const unsigned nbSonde=5;
  
/*
  // brochage des Electrovannes //
  const unsigned char brocheEV_0_O = 22;
  const unsigned char brocheEV_0_F = 23;
  const unsigned char brocheEV_1_O = 24;
  const unsigned char brocheEV_1_F = 25;
  const unsigned char brocheEV_2_O = 26;
  const unsigned char brocheEV_2_F = 27;
  const unsigned char brocheEV_3_O = 28;
  const unsigned char brocheEV_3_F = 29;

  // brochage des retours d'état des EV //
  const unsigned char brocheEtatEV_0_O = 30;
  const unsigned char brocheEtatEV_0_F = 31;
  const unsigned char brocheEtatEV_1_O = 32;
  const unsigned char brocheEtatEV_1_F = 33;
  const unsigned char brocheEtatEV_2_O = 34;
  const unsigned char brocheEtatEV_2_F = 35;
  const unsigned char brocheEtatEV_3_O = 36;
  const unsigned char brocheEtatEV_3_F = 37;*/

  // brochage des sondes de températures //
  const unsigned char brocheSonde_0 = A2; //PF2
  const unsigned char brocheSonde_1 = A3; //PF3
  const unsigned char brocheSonde_2 = A0; //PF0
  const unsigned char brocheSonde_3 = A1; //PF1
  const unsigned char brocheSonde_4 = A9; //PK1
  // const unsigned char brocheSonde_5 = A8; //PK0

  // objets 1-Wire correspondant au brochage des sondes de températures



  // brochage du clavier //
  //      attention le brochage du clavier DOIT se faire sur 1 seul port !!!

}

#endif
