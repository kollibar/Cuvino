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
#include <croutine.h>
#include <StackMacros.h>
#include <mpu_wrappers.h>
#include <task.h>
#include <portable.h>
/** !FreeRTOS **/

#include "main_DEBUG.h"
#include <DebugLogger.h>

#include <Arduino.h>
#include "CuvinoCore.h"
#include "CuvinoGUI.h"
#include "CuvinoWidgets.h"
#include "CuvinoErreurs.h"
#include "CuvinoProgramme.h"
#include "Sonde.h"
#include "SondeV2.h"
#include "EV2.h"
#include "CuveV2.h"
#include <Timer.h>
#include <Erreurs.h>
#include <IO_PCD8544_5T_FreeRTOS.h>
#include <TerminalWindow.h>
#include <avr/pgmspace.h>
#include "fct_diverses.h"
#include <avr/wdt.h>
#include "brochage.h"




#define DEBUG
#define CLAVIER_FREERTOS


const unsigned char ERREUR_OFF=0;
const unsigned char ERREUR_1=1;
const unsigned char ERREUR_2=1;

/************************************
                 datas
*************************************/

DebugLogger debug=DebugLogger(DEBUG_main);
DebugLogger debugGUI=DebugLogger(DEBUG_GUI);
DebugLogger debugTimer=DebugLogger(DEBUG_Timer);
DebugLogger debugProg=DebugLogger(DEBUG_Prog);
DebugLogger debugCore=DebugLogger(DEBUG_Core);


/* data de FreeRTOS */
bool FreeRTOSActif = false;

TaskHandle_t taskCore = NULL;
TaskHandle_t taskGUI = NULL;
//TaskTimer* taskTimer = NULL;
TaskHandle_t taskTimer = NULL;
TaskHandle_t taskKeyboard = NULL;
TaskHandle_t taskProgramme = NULL;


QueueHandle_t queueCmdeToCore = NULL;
QueueHandle_t queueCmdeToProgramme = NULL;
QueueHandle_t queueCmdeToTimer = NULL;


SemaphoreHandle_t semaphoreAccesDataCore = NULL;
const unsigned char TEMPS_MAX_ACCES_DATA_CORE_TICK = 2000 / portTICK_PERIOD_MS;

/* data du Core */
OneWire ds(BROCHE_ONEWIRE); // objet de contrôle de la connection au bus 1-Wire

RtcDS3231 Rtc; // objet de connection à l'horloge temps réel DS3231
Timer timer;  // timer pour programmer des actions dans le futur
PileErreur pileErreur=PileErreur(CuvinoErreurs::startLEDClignotante);
uint8_t codeErreurCore=pileErreur.getNumModuleSuiv();

CuveV2 listeCuve[NB_LIGNE_MAX + 1]; // toutes les infos sur les cuves

bool horlogeOK;
unsigned long lastDate;

AdresseBloc adresseBloc;

/* data de la GUI */
#ifdef CLAVIER_FREERTOS
IO_PCD8544_5T_SPI_software_FreeRTOS display=IO_PCD8544_5T_SPI_software_FreeRTOS(BROCHE_ECRAN_SCLK, BROCHE_ECRAN_SDIN, BROCHE_ECRAN_DC, BROCHE_ECRAN_CS, BROCHE_ECRAN_RESET);
CuvinoWidgets<IO_PCD8544_5T_SPI_software_FreeRTOS> widgets=CuvinoWidgets<IO_PCD8544_5T_SPI_software_FreeRTOS>(display);
// TerminalWindow<IO_PCD8544_5T_SPI_software_FreeRTOS> terminal=TerminalWindow<IO_PCD8544_5T_SPI_software_FreeRTOS>(display);
#else
IO_PCD8544_5T_SPI_software display=IO_PCD8544_5T_SPI_software(BROCHE_ECRAN_SCLK, BROCHE_ECRAN_SDIN, BROCHE_ECRAN_DC, BROCHE_ECRAN_CS, BROCHE_ECRAN_RESET);
CuvinoWidgets<IO_PCD8544_5T_SPI_software> widgets=CuvinoWidgets<IO_PCD8544_5T_SPI_software>(display);
// TerminalWindow<IO_PCD8544_5T_SPI_software> terminal=TerminalWindow<IO_PCD8544_5T_SPI_software>(display);
#endif

unsigned char etatAffichage; // gère l'état de l'affichage (affichage des lignes, affichage de l'heure...)
unsigned char ligne0=0;      // contient le numéro de la ligne active à la position 0 (pour pouvoir déplacer si plus de ligne que affiché à l'écran)
const unsigned char nbLigneEcran=4; // nb de ligne affichée à l'écran en même temps
const unsigned char largeurLigne=42;
const unsigned char hauteurLigne=24;

LigneMenu addLignes[NB_MAX_FCT_MENU];
FonctionModifActualiseAffTemp fctModifActualiseAffTemp[NB_MAX_FCT_ACT_AFF_TEMP];


/***********************
  fonction du core
**********************/

void CuvinoCore::reset(){
  // DDR_RESET |= (1 << BIT_RESET); // on met la broche reset en sortie
  // PORT_RESET &= ~(1 << BIT_RESET); // on met son état à 0
  wdt_enable(WDTO_15MS);
  while(1){}
}

void CuvinoCore::sleep(){
  vTaskDelay(1);
}

bool CuvinoCore::modifieSonde(uint8_t cuve,SondeV2& s){
  if( cuve > NB_LIGNE_MAX ) return false;
  if( ! xSemaphoreTake(semaphoreAccesDataCore, TEMPS_MAX_ACCES_DATA_CORE_TICK )){
    // AJOUTER GESTION ERREUR
    return false;
  } else {
    listeCuve[cuve].sonde=&s;
    xSemaphoreGive(semaphoreAccesDataCore);
    return true;
  }
}
bool CuvinoCore::modifieCuve(uint8_t cuve,CuveV2& c){
  if( cuve > NB_LIGNE_MAX ) return false;
  if( ! xSemaphoreTake(semaphoreAccesDataCore, TEMPS_MAX_ACCES_DATA_CORE_TICK )){
    // AJOUTER GESTION ERREUR
    return false;
  } else {
    listeCuve[cuve]=c;
    xSemaphoreGive(semaphoreAccesDataCore);
    return true;
  }
}

void CuvinoCore::initBroches() {
#ifdef DEBUG
  debugCore.printlnPGM(DEBUG_initBroches);
#endif
  // initialisation des broches des relais

#ifdef BIT_LED // si une broche de led intégré à la carte est définie
  DDR_LED |= (1 << BIT_LED); // met la broche connecté à la LED de la carte en sortie
#endif

}

void CuvinoCore::init(){
  horlogeOK=false; // par defaut l'horloge n'est pas active
  CuvinoCore::initBroches();
  CuvinoCore::initDS3231();
}

void CuvinoCore::demandeMesureTemperature() {
#ifdef DEBUG
  debugCore.printlnPGM(DEBUG_demandeMesureTemperature);
#endif

  if( ! xSemaphoreTake(semaphoreAccesDataCore, TEMPS_MAX_ACCES_DATA_CORE_TICK )){ // prend l'acces aux data du core
    // AJOUTER GESTION ERREUR
    return;
  } else {
    for (unsigned char i = 0; i <= NB_LIGNE_MAX; ++i) {// demande à toutes les sondes de temperature de mesurer la temperature
      listeCuve[i].demandeMesureTemp();
      //if ( listeCuve[i].tempConsigneCuve != TEMP_ARRET && listeCuve[i].sonde->isSondeTemp()) listeCuve[i].sonde->demandeMesureTemp();
    }
    xSemaphoreGive(semaphoreAccesDataCore);
  }
}

void CuvinoCore::controlTemp(void) {
#ifdef DEBUG
  debugCore.printlnPGM(DEBUG_controlTemp);
#endif
  // signed int temp, tempObj;

  if( ! xSemaphoreTake(semaphoreAccesDataCore, TEMPS_MAX_ACCES_DATA_CORE_TICK )){ // prend l'acces aux data du core
    // AJOUTER GESTION ERREUR
    return;
  } else {
    for(unsigned char i=0; i <NB_LIGNE_MAX; ++i){
      listeCuve[i].demandeMesureTemp();
    }
    for (unsigned char i = 0; i < NB_LIGNE_MAX; ++i) {
      listeCuve[i].controlTemp();
      /*if (listeCuve[i].sonde.isSondeTemp()) {
        listeCuve[i].sonde.demandeMesureTemp();
        vTaskDelay(800 / portTICK_PERIOD_MS);
        temp = listeCuve[i].sonde.getTemperature();
      }
      else if (listeCuve[i].sonde.addr[0] == 0) temp = TEMP_ERREUR_ABS_SONDE;
      else temp = TEMP_ERREUR_PAS_SONDE;

      tempObj = listeCuve[i].tempConsigneCuve;

      if ( listeCuve[i].tempConsigneCuve  == TEMP_ARRET) { //si arrêt de la régulation
        if ( listeCuve[i].EV_F.mode != EV_NON_CONFIGURE && listeCuve[i].EV_F.position != FERME) { // EV froid
          listeCuve[i].EV_F.bougeEV(FERME);
        }
        if ( listeCuve[i].EV_C.mode != EV_NON_CONFIGURE && listeCuve[i].EV_C.position != FERME) { // EV chaud
          listeCuve[i].EV_C.bougeEV(FERME);
        }
      } else if (listeCuve[i].tempConsigneCuve <= 2000 && listeCuve[i].tempConsigneCuve >= -880) { // si la température de consigne est comprise dans la plage du capteur
        if (listeCuve[i].sonde.addr[0] == DS18B20) { //contrôle de la ligne en interronpant le froid

  #ifdef DEBUG
          debug.print("Ligne ");
          debug.println(i);
          debug.print("Temperature Objectif:");
          debug.println(tempObj >> 4, DEC);
          debug.print("Temperature Mesure:");
          debug.println(temp >> 4, DEC);
          debug.print("EV_F mode:");
          debug.println(listeCuve[i].EV_F.mode);
          debug.print("EV_F position actuelle:");
          debug.print(listeCuve[i].EV_F.position);
          if ( listeCuve[i].EV_F.position == OUVERT) debug.println("(Ouverte)");
          else debug.println("(Fermée)");
  #endif


          if ( listeCuve[i].EV_F.mode != EV_NON_CONFIGURE) { // EV froid
            if ( listeCuve[i].EV_C.mode != EV_NON_CONFIGURE) tempObj += _1_2_ECART_CHAUD_FROID; // si présence EV froid -> on compare par rapport à une température obj legèrement plus haut (d'1/2 ecart chaud froid)

            if ( listeCuve[i].EV_F.position == OUVERT && temp <= (tempObj - HISTERESIS_SIMPLE) ) {
              listeCuve[i].EV_F.bougeEV(FERME);
              #ifdef DEBUG
                debug.println("Fermeture EV_F");
              #endif
            } else if ( listeCuve[i].EV_F.position == FERME && temp >= (tempObj + HISTERESIS_SIMPLE) ) {
              listeCuve[i].EV_F.bougeEV(OUVERT);
              #ifdef DEBUG
                debug.println("Ouverture EV_F");
              #endif
            }
          }

          if ( listeCuve[i].EV_C.mode != EV_NON_CONFIGURE) { // EV chaud
            if ( listeCuve[i].EV_F.mode != EV_NON_CONFIGURE) tempObj -= (_1_2_ECART_CHAUD_FROID << 1); // si présence EV chaud -> on compare par rapport à une température obj legèrement plus bas (d'1/2 ecart chaud froid)

            if ( listeCuve[i].EV_C.position == OUVERT && temp >= (tempObj + HISTERESIS_SIMPLE) ) {
              listeCuve[i].EV_F.bougeEV(FERME);
              #ifdef DEBUG
                debug.println("Fermeture EV_C");
              #endif
            }else if ( listeCuve[i].EV_C.position == 0 && temp <= (tempObj - HISTERESIS_SIMPLE) ) {
              listeCuve[i].EV_F.bougeEV(OUVERT);
              #ifdef DEBUG
                debug.println("Ouverture EV_C");
              #endif
            }
          }
        }
      } */
    }
    xSemaphoreGive(semaphoreAccesDataCore);
  }
}


void CuvinoCore::initCuves(void){
  #ifdef DEBUG
    debugCore.printlnPGM(DEBUG_initCuves);
  #endif
  for(unsigned char i=0;i<NB_LIGNE_MAX;++i){
    listeCuve[i].initEV();
  }
}

void CuvinoCore::initDS3231(void) {
  Rtc.Begin();

  // never assume the Rtc was last configured by you, so
  // just clear them to your needed state
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);

  if (!Rtc.IsDateTimeValid())
  {
    // Common Cuases:
    //    1) first time you ran and the device wasn't running yet
    //    2) the battery on the device is low or even missing
#ifdef DEBUG
    debugCore.printlnPGM(DEBUG_rtc_perd_temps);
#endif
    // following line sets the RTC to the date & time this sketch was compiled
    // it will also reset the valid flag internally unless the Rtc device is
    // having an issue

  }

  if (!Rtc.GetIsRunning())
  {
#ifdef DEBUG
    debugCore.printlnPGM(DEBUG_rtc_non_actif);
#endif
    Rtc.SetIsRunning(true);
    delay(500);
  }

  if( Rtc.GetIsRunning()) horlogeOK=true;
}


/******************************************************
           Gestions des paramètres
******************************************************/

void CuvinoCore::reglageDefaut(void) {
  // lastDate minimum = 1/8/16 à 00:00
  if( FreeRTOSActif && ! xSemaphoreTake(semaphoreAccesDataCore, TEMPS_MAX_ACCES_DATA_CORE_TICK )){ // prend l'acces aux data du core
    // AJOUTER GESTION ERREUR
    return;
  } else {
    lastDate = 0x1F314D80;
    for (uint8_t i = 0; i <= NB_LIGNE_MAX; ++i) {
      listeCuve[i].defaut();

      listeCuve[i].nom = i + 48;
      if ( i == NB_LIGNE_MAX) listeCuve[i].nom = 'L';
    }
    listeCuve[NB_LIGNE_MAX].tempConsigneCuve = TEMP_SONDE_LOCAL;
    listeCuve[NB_LIGNE_MAX].EV_F.mode = EV_SONDE_LOCAL;

    adresseBloc.cuves = 63;
    adresseBloc.programmes = 350;
    adresseBloc.principal = 1;

    if( FreeRTOSActif ) xSemaphoreGive(semaphoreAccesDataCore);
  }
}


void CuvinoCore::saveParams(void) {
  BlocMem bloc;
#ifdef DEBUG
  debugCore.printlnPGM(DEBUG_saveParams);
#endif

  if( FreeRTOSActif && ! xSemaphoreTake(semaphoreAccesDataCore, TEMPS_MAX_ACCES_DATA_CORE_TICK )){ // prend l'acces aux data du core
    // AJOUTER GESTION ERREUR
    return;
  } else {
    EEPROM.write(0, 1); // addresse version memoire : 0 , version memoire : 1


    adresseBloc.cuves = 63;
    adresseBloc.programmes = 350;

    // sauvegarde du bloc principal
    bloc = BlocMem(adresseBloc.principal, 1, modeBlocMem_Ecriture);

    bloc.ecrit((uint8_t)VERSION_LOGICIEL);
    if( horlogeOK) bloc.ecrit((uint8_t*)&lastDate, 4);
    else bloc.ecrit((uint32_t)0x1F314D80);
    bloc.ecrit((uint8_t)NB_LIGNE_MAX);// nb de cuve TOTAL dans le bloc cuve (y-compris non configurées éventuellement)
    bloc.ecrit((uint8_t)NB_PROGRAMME_MAX);// nb de programme TOTAL dans le bloc programme (y-compris non configurés éventuellement)
    listeCuve[NB_LIGNE_MAX].save(&bloc);
    bloc.reserve(7);
    bloc.ecrit((uint8_t*)&adresseBloc.cuves, 2);
    bloc.ecrit((uint8_t*)&adresseBloc.programmes, 2);
    bloc.reserve(20);
    bloc.actualiseEntete();

  #ifdef DEBUG
    debugCore.setWriteMode(modeDebugComplet);
    debugCore.printlnPGM(DEBUG_blocPrincipal);
    debugCore.printPGM(DEBUG_CRC16_CALCULE);
    debugCore.println(bloc.CRC16());
    debugCore.printPGM(DEBUG_CRC16_LUE);
    debugCore.println(bloc.CRC16lue());
    debugCore.printPGM(DEBUG_adresseBloc);
    debugCore.println(bloc.addr());
    debugCore.printPGM(DEBUG_tailleBloc);
    debugCore.println(bloc.taille());
    debugCore.setWriteMode(modeDebug);
  #endif

    bloc.close();

    // sauvegarde du bloc Cuve
    bloc.init(adresseBloc.cuves, 1, modeBlocMem_Ecriture);
  #ifdef DEBUG
    debugCore.setWriteMode(modeDebugComplet);
    debugCore.printlnPGM(DEBUG_blocCuve_Reinit);
    debugCore.printPGM(DEBUG_CRC16_CALCULE);
    debugCore.println(bloc.CRC16());
    debugCore.printPGM(DEBUG_CRC16_LUE);
    debugCore.println(bloc.CRC16lue());
    debugCore.printPGM(DEBUG_adresseBloc);
    debugCore.println(bloc.addr());
    debugCore.printPGM(DEBUG_tailleBloc);
    debugCore.println(bloc.taille());
    debugCore.setWriteMode(modeDebug);
  #endif
    for (uint8_t i = 0; i < NB_LIGNE_MAX; ++i) {
  #ifdef DEBUG
      debugCore.printPGM(DEBUG_save);
      debugCore.write(' ');
      debugCore.printPGM(DEBUG_cuve);
      debugCore.print(i);
      debugCore.write(':');

      debugCore.println(listeCuve[i].save(&bloc));

      debugCore.printPGM(DEBUG_tailleBloc);
      debugCore.println(bloc.taille());
  #else
      listeCuve[i].save(&bloc);
  #endif
    }
    if( FreeRTOSActif ) xSemaphoreGive(semaphoreAccesDataCore);

    bloc.actualiseEntete();
  }

#ifdef DEBUG
  debugCore.setWriteMode(modeDebugComplet);
  debugCore.printlnPGM(DEBUG_blocCuve);
  debugCore.printPGM(DEBUG_CRC16_CALCULE);
  debugCore.println(bloc.CRC16());
  debugCore.printPGM(DEBUG_CRC16_LUE);
  debugCore.println(bloc.CRC16lue());
  debugCore.printPGM(DEBUG_adresseBloc);
  debugCore.println(bloc.addr());
  debugCore.printPGM(DEBUG_tailleBloc);
  debugCore.println(bloc.taille());
  debugCore.setWriteMode(modeDebug);
#endif
}

bool CuvinoCore::loadParams() {
#ifdef DEBUG
  debugCore.printlnPGM(DEBUG_loadParams);
#endif
  switch (EEPROM.read(0)) {
    case 1:
      return loadParamsV1();
    default:
#ifdef DEBUG
      debugCore.printPGM(DEBUG_versionMemNonSupporte);
      debugCore.print(EEPROM.read(0));
      debugCore.println(')');
      debugCore.printlnPGM(DEBUG_chargementImpossible);
#endif
      return false;
  }
}

bool CuvinoCore::loadParamsV1() {
#ifdef DEBUG
  debugCore.printPGM(DEBUG_loadParams);
  debugCore.println("V1.1");
#endif

  uint8_t nb_cuve = 0, nb_programme = 0, version_logiciel, version;

  BlocMem bloc = BlocMem(adresseBloc.principal, modeBlocMem_Lecture);
#ifdef DEBUG
  debugCore.setWriteMode(modeDebugComplet);
  debugCore.printlnPGM(DEBUG_blocPrincipal);
  debugCore.printPGM(DEBUG_CRC16_LUE);
  debugCore.println(bloc.CRC16lue());
  debugCore.printPGM(DEBUG_CRC16_CALCULE);
  debugCore.println(bloc.CRC16());
  debugCore.printPGM(DEBUG_tailleBloc);
  debugCore.println(bloc.taille());
  debugCore.printPGM(DEBUG_adresseBloc);
  debugCore.println(bloc.addr());
  debugCore.printPGM(DEBUG_modeBloc);
  debugCore.println((char)bloc.mode());
  debugCore.printPGM(DEBUG_adresseBloc);
  debugCore.println((uint8_t)bloc.version());
  debugCore.setWriteMode(modeDebug);
#endif

  if( FreeRTOSActif && ! xSemaphoreTake(semaphoreAccesDataCore, TEMPS_MAX_ACCES_DATA_CORE_TICK )){ // prend l'acces aux data du core
    // AJOUTER GESTION ERREUR
    return false;
  } else {
    if ( bloc.CRCValide() ) {
      switch (bloc.version()) {
        case 1:
          bloc.lit((uint8_t*)&version_logiciel, 1);
          bloc.lit((uint8_t*)&lastDate, 4);
          bloc.lit((uint8_t*)&nb_cuve, 1); // nb de cuve TOTAL dans le bloc cuve (y-compris non configurées éventuellement)
          bloc.lit((uint8_t*)&nb_programme, 1); // nb de programme TOTAL dans le bloc programme (y-compris non configurés éventuellement)
          listeCuve[NB_LIGNE_MAX].load(&bloc);
          bloc.reserve(7);
          bloc.lit((uint8_t*)&adresseBloc.cuves, 2);
          bloc.lit((uint8_t*)&adresseBloc.programmes, 2);
          bloc.reserve(20);
          break;
        default:
  #ifdef DEBUG
          debugCore.printPGM(DEBUG_erreur_lecture_bloc);
          debugCore.write(' ');
          debugCore.printPGM(DEBUG_principal);
          debugCore.print(": ");
          debugCore.printPGM(DEBUG_versionBlocNonSupporte);
          debugCore.write('(');
          debugCore.print(bloc.version());
          debugCore.println(')');
  #endif
          break;
      }
    } else {
  #ifdef DEBUG
      debugCore.printPGM(DEBUG_erreur_lecture_bloc);
      debugCore.write(' ');
      debugCore.printPGM(DEBUG_principal);
      debugCore.print(": ");
      debugCore.printPGM(DEBUG_blocIlisible_crc);
  #endif
    }

    // lecture du bloc Cuve

    bloc = BlocMem(adresseBloc.cuves, modeBlocMem_Lecture);
  #ifdef DEBUG
    debugCore.setWriteMode(modeDebugComplet);
    debugCore.printlnPGM(DEBUG_blocCuve);
    debugCore.printPGM(DEBUG_CRC16_LUE);
    debugCore.println(bloc.CRC16lue());
    debugCore.printPGM(DEBUG_CRC16_CALCULE);
    debugCore.println(bloc.CRC16());
    debugCore.printPGM(DEBUG_tailleBloc);
    debugCore.println(bloc.taille());
    debugCore.printPGM(DEBUG_adresseBloc);
    debugCore.println(bloc.addr());
    debugCore.printPGM(DEBUG_modeBloc);
    debugCore.println((char)bloc.mode());
    debugCore.printPGM(DEBUG_adresseBloc);
    debugCore.println((uint8_t)bloc.version());
    debugCore.setWriteMode(modeDebug);
  #endif

    if ( bloc.CRCValide() ) {
      switch (bloc.version()) {
        case 1:
  #ifdef DEBUG
          debugCore.printPGM(FR::TXT_NB);
          debugCore.write(' ');
          debugCore.printPGM(FR::TXT_CUVE);
          debugCore.write(':');
          debugCore.println(nb_cuve);
  #endif
          for (uint8_t i = 0; i < nb_cuve && i < NB_LIGNE_MAX; ++i) {
  #ifdef DEBUG
            debugCore.printPGM(FR::TXT_LOAD);
            debugCore.write(' ');
            debugCore.printPGM(FR::TXT_CUVE);
            debugCore.write(' ');
            debugCore.print(i);
            debugCore.write(':');
            debugCore.println(listeCuve[i].load(&bloc));
  #else
            listeCuve[i].load(&bloc);
  #endif
          }
          break;
        default:
  #ifdef DEBUG
          debugCore.printPGM(DEBUG_erreur_lecture_bloc);
          debugCore.write(' ');
          debugCore.printPGM(FR::TXT_CUVE);
          debugCore.print(": ");
          debugCore.printPGM(DEBUG_versionBlocNonSupporte);
          debugCore.write('(');
          debugCore.print(bloc.version());
          debugCore.println(')');
  #endif
          break;
      }
    } else {
  #ifdef DEBUG
      debugCore.printPGM(DEBUG_erreur_lecture_bloc);
      debugCore.write(' ');
      debugCore.printPGM(FR::TXT_CUVE);
      debugCore.print(": ");
      debugCore.printPGM(DEBUG_blocIlisible_crc);
  #endif
    }

    if( FreeRTOSActif ) xSemaphoreGive(semaphoreAccesDataCore);
  }

  return true;
}


bool CuvinoCore::modifTempConsigne(int16_t& temp, int8_t add) {
  if ( temp == TEMP_ARRET) temp = ((add > 0) ? TEMP_MINI_CONSIGNE : TEMP_MAXI_CONSIGNE);
  else if ( temp < -1360 || temp > 2000) { // erreur   // la température de consigne est hors de la plage du capteur => on est dans un cas d'erreur
    return false;
  } else {
    temp += add;
    if ( temp > TEMP_MAXI_CONSIGNE || temp < TEMP_MINI_CONSIGNE) temp = TEMP_ARRET;
  }
  return true;
}

/******************************************
           gestion des Erreurs
*******************************************/

void CuvinoErreurs::startLEDClignotante(){
  if ( etatAlert < 1 && timerAlerte == TIMER_INVALIDE) timerAlerte = timer.every(250, CuvinoErreurs::LEDClignotante); // si il n'y en a pas déjà un, lance un clignotement de la led de la carte
}

void CuvinoErreurs::stopLEDClignotante(){
  if ( timerAlerte != TIMER_INVALIDE) timer.stop(timerAlerte);
  etatAlert=0;
}

void CuvinoErreurs::LEDClignotante(void) {
  // fonction faisant clignoter la led de la carte
  // a chaque appel, change l'état de la led de la carte
#ifdef BROCHE_LED
  if ( (PORT_LED & (1 << BIT_LED)) != 0) { // si led allumé
    PORT_LED &= ~(1 << BIT_LED);
  } else { // si led éteinte
    PORT_LED |= (1 << BIT_LED);
  }
#endif
}

bool CuvinoErreurs::menuErreurs(){
  for(;;){ //boucle générale
    display.clearDisplay();
    display.printPGM(FR::TXT_LISTE,true);
    display.write(' ');
    display.printlnPGM(FR::TXT_ERREURS);
    if( pileErreur.getNb() == 0){ // aucune erreur
      display.setCursor(0,16);
      display.printPGM(FR::TXT_AUCUNE);
      display.write(' ');
      display.printPGM(FR::TXT_ERREURS);
      display.display();
      for(;;){
        display.aucuneTouche();
        unsigned char touche=display.getTouche(-1);
        if( touche==TIMEOUT_LED) return false;
        if( touche ==BT_SELECT || touche==(BT_HAUT|BT_BAS)) return true;
      }
    }else{ // il y a des erreurs
      unsigned char depart=0;
      retourSelect result;
      for(;;){ // boucle d'affichage de toutes les lignes
        result=widgets.menuListeV2(CuvinoErreurs::_creeLigneMenuErreurs, (void*)&pileErreur, pileErreur.getNb(), 15, 8,48,0,84,1,depart);
        if( result.touche==TIMEOUT_LED) return false;
        if( result.touche==(BT_BAS|BT_HAUT)){ // on quitte
          display.clearDisplay();
          display.printlnPGM(FR::TXT_SUPPRIMER,true);
          display.printlnPGM(FR::TXT_TOUTES_LES);
          display.printlnPGM(FR::TXT_ERREURS);
          for(;;){ // boucle de demande de suppression de toutes les erreurs
            result=widgets.menuListeOuiNon(32,48,12,28);
            if( result.touche == TIMEOUT_LED) return false;
            if( result.touche == BT_SELECT ){
              if( result.result==0){//oui
                pileErreur.videPile(); // on supprime toutes les erreurs
                CuvinoErreurs::stopLEDClignotante(); // et on arrete la LED d'alerte
              }
              return true;
            }
          } // fin boucle de suppression de toutes les erreurs
        }
        if( result.touche==BT_SELECT){
          display.fillRect(0, 8, 84, 40, WHITE);
          display.setCursor(0,8);
          display.printPGM(FR::TXT_ERREUR);
          display.write(':');
          display.setCursor(0,16);
          CuvinoErreurs::_creeLigneMenuErreurs(result.result,NULL,15,(void*)&pileErreur);
          display.setCursor(0,32);
          display.printPGM(FR::TXT_SUPPRIMER);
          display.write('?');
          for(unsigned char i=result.result;;){ // boucle de demande de suppression de la ligne
            result=widgets.menuListeOuiNon(32,48,60);
            if( result.touche==TIMEOUT_LED) return false;
            if( result.touche==BT_SELECT) {
              if( result.result==0){ // oui
                pileErreur.delErreur(i); // on supprime l'erreur en question
                if( pileErreur.getNb()==0) CuvinoErreurs::stopLEDClignotante();
                break;
              }
            }
          } // fin boucle de demande de suppression de la ligne
          if( pileErreur.getNb()==0) break; // si pas d'erreur on ressort dans la boucle générale
        }
      } // fin boucle d'affichage de toutes les lignes
    }
  } // fin boucle générale
  return true;
}

void CuvinoErreurs::_creeLigneMenuErreurs(uint8_t i, char* chaine, uint8_t taille, void* arg){
  PileErreur* pileErreur=(PileErreur*)arg;

  if( chaine == NULL){
    RtcDateTime dateErreur=RtcDateTime(Rtc.GetDateTime().TotalSeconds()- (millis() - pileErreur->getDateErreur(i)));
    // affichage de la date
    display.printDateTime(dateErreur.Day(),dateErreur.Month(),(uint8_t)(dateErreur.Year()-2000),dateErreur.Hour(),dateErreur.Minute());
    display.write('\n');

    // affichage du nom du module
    if( pileErreur->getModuleErreur(i) == codeErreurCore){
      display.printPGM(FR::TXT_CORE);
      display.write(' ');

      // affichage du code de l'erreur
      display.print(pileErreur->getCodeErreur(i));
      display.write('_');
    } else if( pileErreur->getModuleErreur(i) == listeCuve[0].EV_C.getNumModuleErreur() ){
      display.printPGM(FR::TXT_EV);
      display.write(' ');
      display.printPGM(FR::TXT_ERREUR_EV[pileErreur->getCodeErreur(i)]);
    }
    //affichage du param de l'erreur
    display.print(pileErreur->getParamErreur(i));
  } else{
    // affichage du nom du module
    if( pileErreur->getModuleErreur(i) == codeErreurCore){
      strcpy_P(chaine,FR::TXT_CORE);
      chaine[4]=' ';
      // affichage du code de l'erreur
      if( pileErreur->getCodeErreur(i) <10){
        chaine[5]=' ';
        chaine[6]=pileErreur->getCodeErreur(i)+48;
      } else{
        chaine[5]=(pileErreur->getCodeErreur(i)/10)+48;
        chaine[6]=(pileErreur->getCodeErreur(i)%10)+48;
      }
      chaine[7]=' ';
      chaine[8]=' ';
      chaine[9]=' ';
      chaine[10]=' ';
      chaine[11]=' ';
    } else if( pileErreur->getModuleErreur(i) == listeCuve[0].EV_C.getNumModuleErreur() ){
      strcpy_P(chaine,FR::TXT_EV);
      chaine[2]=' ';
      strcpy_P(&chaine[3],FR::TXT_ERREUR_EV[pileErreur->getCodeErreur(i)]);
      chaine[11]=' ';
    }

    //affichage du param de l'erreur
    if( pileErreur->getParamErreur(i) <10){
      chaine[12]=' ';
      chaine[13]=pileErreur->getParamErreur(i)+48;
    } else if( pileErreur->getParamErreur(i) <100){
      chaine[12]=(pileErreur->getCodeErreur(i)/10)+48;
      chaine[13]=(pileErreur->getCodeErreur(i)%10)+48;
    }
    chaine[14]=0;
  }
}

/**********************************
        fonctions de la GUI
**********************************/

void CuvinoGUI::initGUI() {
  #ifdef DEBUG
    debugGUI.printlnPGM(DEBUG_initGUI);
  #endif
  // initialisation du tableau de ligne supplémentaires
  for (unsigned char i = 0; i < NB_MAX_FCT_MENU; ++i) {
    addLignes[i].pos = POSITION_MENU_NON_CONF;
    addLignes[i].fctMenu = NULL;
  }

  // initialisation du tableau de fct de modifi affichage temps
  for(unsigned char i=0; i< NB_MAX_FCT_ACT_AFF_TEMP;++i) fctModifActualiseAffTemp[i]=NULL;

  CuvinoGUI::initEcran();
}


void CuvinoGUI::initEcran() {
#ifdef DEBUG
  debugGUI.printlnPGM(DEBUG_initEcran);
#endif
  display.onOffEcran(true);
  display.begin();
  display.setContrast(50);
  display.display();
  display.setTextColor(BLACK, WHITE);
  display.setTextSize(1);
}


void CuvinoGUI::splashScreen() { //affiche l'écran d'acceuil
#ifdef DEBUG
  debugGUI.printlnPGM(DEBUG_splashScreen);
#endif
  display.clearDisplay();
  display.printlnPGM(VERSION::TXT_LOGICIEL);
  display.setCursor(0, 24);
  display.printlnPGM(VERSION::TXT_VERSION);
  display.printlnPGM(VERSION::TXT_VERSION_MEM);

  display.setTextSize(0);
  display.printPGM(FR::TXT_DATE);
  display.write(':');
  RtcDateTime dateTime = RtcDateTime(__DATE__, __TIME__);
  display.printDateTime(dateTime.Day(), dateTime.Month(), (uint8_t)(dateTime.Year() - 2000), dateTime.Hour(), dateTime.Minute());
  display.setTextSize(1);

  display.display();
}


void CuvinoGUI::initAffStandard(void) {
#ifdef DEBUG
  debugGUI.printlnPGM(DEBUG_initAFFStandard);
#endif
  etatAffichage = AFF_TEMP;
  signed int i;
  // crée l'affichage standard
  display.clearDisplay();

  display.drawLine(0, hauteurLigne-1, LCDWIDTH, hauteurLigne-1, BLACK);
  display.drawLine(largeurLigne-1, 0, largeurLigne-1, LCDHEIGHT, BLACK);

  for (i = 0; i <= NB_LIGNE_MAX ; ++i) CuvinoGUI::afficheLigne(i, positionActuelLigne(i));

  CuvinoGUI::actualiseAffTemp();
  display.display();
}

Point8_t CuvinoGUI::CoordonneesLigne(signed int pos){
  /* renvoi les coordonnées du point en haut a gauche de l'affichage de la ligne */
  return Point8_t(
      (pos % 2) * largeurLigne,  // x
      (pos / 2) * hauteurLigne   // y
    );
}

signed char CuvinoGUI::positionActuelLigne(signed int i){
  /* renvoi la position dans l'état actuelle de l'affichage de la ligne i
  renvoi 0b10000000 si inaplicable actuellement */
  if ( (etatAffichage & AFF_TEMP ) == 0) return 0b10000000; // si pas d'affichage des temperatures -> quitte

  return (signed char)i - ligne0;
}

void CuvinoGUI::afficheLigne(signed int i, signed int pos, bool selectConsigne) {
  if ( ((etatAffichage & AFF_TEMP ) == 0) || pos < 0 || pos >= nbLigneEcran) return; // si pas d'affichage des temperatures -> quitte
#ifdef DEBUG
  debugGUI.printPGM(DEBUG_afficheLigne);
  debugGUI.print(i);
  debugGUI.print(',');
  debugGUI.print(pos);
  debugGUI.print(',');
  debugGUI.print(selectConsigne);
  debugGUI.println(')');
#endif
  if ( i > NB_LIGNE_MAX) return;
  if ( pos > 3) return;

  Point8_t p=CoordonneesLigne(pos);
  p.y+=3;

  display.setTextSize(2);
  display.setCursor(p.x, p.y);
  signed int tempConsigne = listeCuve[i].tempConsigneCuve;

  if ( i == NB_LIGNE_MAX || (i == 3 && tempConsigne == TEMP_LIGNE_NON_CONFIGURE && (listeCuve[NB_LIGNE_MAX].sonde->isSondeTemp()))) {
    display.print((char)listeCuve[NB_LIGNE_MAX].nom);
    tempConsigne = TEMP_SONDE_LOCAL;
  }
  else display.print((char)listeCuve[i].nom);

  display.setTextSize(1);
  display.fillRect(p.x + 12, p.y, 29, 8, WHITE); // on efface la zone
  if ( selectConsigne) display.setTextColor(WHITE, BLACK);
  display.setCursor(p.x + 12, p.y);
  switch (tempConsigne) {
    case TEMP_SONDE_LOCAL:
      display.setCursor(p.x + 12, p.y);
      if (!Rtc.IsDateTimeValid()) {
        display.printPGM(FR::TXT_TIME_INVALIDE);
      } else {
        RtcDateTime now = Rtc.GetDateTime();
        display.printTime(now.Hour(), now.Minute());
      }
      break;
    case TEMP_ARRET:
      display.printPGM(FR::TXT_STOP);
      break;
    case TEMP_SONDE_NON_CONFIGURE:
    case TEMP_LIGNE_NON_CONFIGURE:
      display.printPGM(FR::TXT_NON);
      display.setCursor(p.x + 12, p.y + 8);
      display.printPGM(FR::TXT_CONF);
      break;
    case TEMP_EV_NON_CONFIGURE:
      display.printPGM(FR::TXT_N_CF);
      break;
    default:
      widgets.printTemp(listeCuve[i].tempConsigneCuve, p.x + 12, p.y, true);
  }
  display.setTextColor(BLACK, WHITE);
  actualiseAffTemp(i, pos);
}

void CuvinoGUI::actualiseEtatEV(){
  #ifdef DEBUG
  debugGUI.printPGM(DEBUG_actualiseEtatEV);
  debugGUI.println(')');
  #endif
  #ifdef DEBUG
    debugGUI.print("etatAffichage:");
    debugGUI.println(etatAffichage,BIN);
    debugGUI.print("AFF_TEMP");
    debugGUI.println(AFF_TEMP,BIN);
  #endif
  if ( (etatAffichage & AFF_TEMP ) == 0) return; // si pas d'affichage des temperatures -> quitte

  for (uint8_t i = 0; i <= NB_LIGNE_MAX ; ++i) {
    CuvinoGUI::actualiseEtatEV(i,positionActuelLigne(i));
  }
}

void CuvinoGUI::actualiseEtatEV(signed int i, signed int pos){
  #ifdef DEBUG
  debugGUI.printPGM(DEBUG_actualiseEtatEV);
  debugGUI.print(i);
  debugGUI.write(',');
  debugGUI.print(pos);
  debugGUI.println(')');
  #endif

  #ifdef DEBUG
    debugGUI.print("etatAffichage:");
    debugGUI.println(etatAffichage,BIN);
    debugGUI.print("AFF_TEMP");
    debugGUI.println(AFF_TEMP,BIN);
  #endif
  if ( ((etatAffichage & AFF_TEMP ) == 0) || pos < 0 || pos >= nbLigneEcran) return; // si pas d'affichage des temperatures -> quitte



  Point8_t p=CuvinoGUI::CoordonneesLigne(pos);
  p.y+=18;

  listeCuve[i].EV_C.etatEV(); // appelle de l'état des EV pour actualisation de la position
  listeCuve[i].EV_F.etatEV();

  display.fillRect(p.x, p.y, 12, 5, WHITE);
  if( listeCuve[i].EV_F.mode != EV_NON_CONFIGURE){
    display.drawCharReduit(p.x, p.y, 'F', BLACK, WHITE);
    if( p.y < 24) display.drawLine(p.x, p.y+5, p.x+4, p.y+5, BLACK);
    switch (listeCuve[i].EV_F.position) {
      case OUVERT:
        display.drawLine(p.x+6, p.y+2, p.x+10, p.y+2, BLACK);
        break;
      case FERME:
        display.drawLine(p.x+8, p.y+4, p.x+8, p.y, BLACK);
        break;
      default:
        switch(listeCuve[i].EV_F.mvt_pos_fin){
          case OUVERT:
            display.drawLine(p.x+6, p.y+1, p.x+10, p.y+3, BLACK);
            break;
          case FERME:
            display.drawLine(p.x+7, p.y+4, p.x+9, p.y, BLACK);
            break;
          default:
            display.drawPixel(p.x+8, p.y+2, BLACK);
        }
    }
  }
  p.x+=15;
  display.fillRect(p.x, p.y, 12, 5, WHITE);
  if( listeCuve[i].EV_C.mode != EV_NON_CONFIGURE){
    display.drawCharReduit(p.x, p.y, 'C', BLACK, WHITE);
    if( p.y < 24) display.drawLine(p.x, p.y+5, p.x+4, p.y+5, BLACK);
    switch (listeCuve[i].EV_C.position) {
      case OUVERT:
        display.drawLine(p.x+6, p.y+2, p.x+10, p.y+2, BLACK);
        break;
      case FERME:
        display.drawLine(p.x+8, p.y+4, p.x+8, p.y, BLACK);
        break;
      default:
        switch(listeCuve[i].EV_C.mvt_pos_fin){
          case OUVERT:
            display.drawLine(p.x+6, p.y+1, p.x+10, p.y+3, BLACK);
            break;
          case FERME:
            display.drawLine(p.x+7, p.y+4, p.x+9, p.y, BLACK);
            break;
          default:
            display.drawPixel(p.x+8, p.y+2, BLACK);
        }
    }
  }
  display.display();
}

void CuvinoGUI::actualiseAffTemp() {
  /* actualise les temperatures à l'écran */
  #ifdef DEBUG
  debugGUI.printPGM(DEBUG_actualiseAffTemp);
  debugGUI.println(')');
  #endif
  if ( (etatAffichage & AFF_TEMP ) == 0) return; // si pas d'affichage des temperatures -> quitte

  if( FreeRTOSActif && ! xSemaphoreTake(semaphoreAccesDataCore, TEMPS_MAX_ACCES_DATA_CORE_TICK )){ // prend l'acces aux data du core
    // AJOUTER GESTION ERREUR
    return;
  } else {
    for (uint8_t i = 0; i <= NB_LIGNE_MAX ; ++i) {
      CuvinoGUI::actualiseAffTemp(i,positionActuelLigne(i));
    }
    xSemaphoreGive(semaphoreAccesDataCore);
    for(unsigned char i=0;i<NB_MAX_FCT_ACT_AFF_TEMP;++i){
      if(fctModifActualiseAffTemp[i] != NULL) fctModifActualiseAffTemp[i]();
    }
  }
}

void CuvinoGUI::actualiseAffTemp(signed int i, signed int pos){
  #ifdef DEBUG
  debugGUI.printPGM(DEBUG_actualiseAffTemp);
  debugGUI.print(i);
  debugGUI.write(',');
  debugGUI.print(pos);
  debugGUI.println(')');
  debugGUI.printPGM(DEBUG_etatAffichage);
  debugGUI.write(':');
  debugGUI.println(etatAffichage);
  debugGUI.print("isSondeTemp:");
  debugGUI.println(listeCuve[i].sonde->isSondeTemp());
  #endif
  if ( ((etatAffichage & AFF_TEMP ) == 0) || pos < 0 || pos >= nbLigneEcran) return; // si pas d'affichage des temperatures -> quitte

  Point8_t p=CuvinoGUI::CoordonneesLigne(pos);
  p.x+=12;
  p.y+=11;


  if (i == NB_LIGNE_MAX || (i == 3 && listeCuve[3].tempConsigneCuve == TEMP_LIGNE_NON_CONFIGURE && listeCuve[NB_LIGNE_MAX].sonde->isSondeTemp() ) ) {
    signed int temp=listeCuve[NB_LIGNE_MAX].getTemperature();
    if( temp > TEMP_NON_LUE) widgets.printTemp(temp, p.x, p.y);
    if( horlogeOK ){
      display.setCursor(p.x, p.y-8);
      display.printTime(Rtc.GetDateTime().Hour(),Rtc.GetDateTime().Minute());
    }
  } else if ( listeCuve[i].tempConsigneCuve != TEMP_LIGNE_NON_CONFIGURE && listeCuve[i].tempConsigneCuve != TEMP_SONDE_NON_CONFIGURE && listeCuve[i].sonde->isSondeTemp() ) {
    signed int temp=listeCuve[i].getTemperature();
    if( temp <= TEMP_ERREUR) {
      display.setCursor(p.x,p.y);
      display.print(FR::TXT_ERR);
    } else {
        widgets.printTemp(temp, p.x, p.y);
    }

    // affichage de la position de l'electrovanne
    CuvinoGUI::actualiseEtatEV(i,i);
  }
  display.display();
}

bool CuvinoGUI::addLigneMenuConf(uint8_t pos, const char* nomPGM, FonctionMenu fct) {
  unsigned char i=0;
  while (addLignes[i].pos != POSITION_MENU_NON_CONF) {
    ++i;
    if ( i >= NB_MAX_FCT_MENU) return false;
  }
  addLignes[i].pos = pos;
  addLignes[i].fctMenu = fct;
  addLignes[i].nomPGM = nomPGM;
  return true;
}

bool CuvinoGUI::addModifActualiseAffTemp(FonctionModifActualiseAffTemp fct){
  for(unsigned char i=0;i<NB_MAX_FCT_ACT_AFF_TEMP;++i){
    if( fctModifActualiseAffTemp[i]==NULL ) {
      fctModifActualiseAffTemp[i]=fct;
      return true;
    }
  }
  return false;
}

bool CuvinoGUI::menuConfiguration(void) {
  etatAffichage=0;
#ifdef DEBUG
  debugGUI.printlnPGM(DEBUG_menuConfiguration);
#endif
  /* menu complet de configuration */
  etatAffichage &= ~AFF_TEMP; // désactive l'affichage de la température
  retourSelect result;
  uint8_t depart = 0;
  unsigned char nbAvt = 0;
  unsigned char nbApres = 0;
  CuveV2 cuveConfig;

  for (unsigned char i = 0; i < NB_MAX_FCT_MENU; ++i) {
    switch (addLignes[i].pos) {
      case POSITION_MENU_AVT:
        nbAvt++;
        break;
      case POSITION_MENU_APRES:
        nbApres++;
        break;
    }
  }
  #ifdef DEBUG
  debugGUI.printPGM(DEBUG_ligneAvt);
  debugGUI.print(nbAvt);
  for(unsigned char i=0;i<NB_MAX_FCT_MENU;++i) {
    char chaine[15]="";
    if( addLignes[i].pos==POSITION_MENU_AVT){
      strcpy_P(chaine,addLignes[i].nomPGM);
      debugGUI.print("|-> ");
      debugGUI.println(chaine);
    }
  }
  debugGUI.printPGM(DEBUG_ligneApres);
  debugGUI.println(nbApres);
  for(unsigned char i=0;i<NB_MAX_FCT_MENU;++i) {
    char chaine[15]="";
    if( addLignes[i].pos==POSITION_MENU_APRES){
      strcpy_P(chaine,addLignes[i].nomPGM);
      debugGUI.print("|-> ");
      debugGUI.println(chaine);
    }
  }
  #endif

  for (;;) {
    #ifdef DEBUG
    debugGUI.printlnPGM(DEBUG_ligneTrait);
    for(unsigned char i=0;i<NB_LIGNE_MAX;++i){
      listeCuve[i].print(debugGUI);
      debugGUI.println();
    }
    #endif
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(BLACK, WHITE);
    display.printlnPGM(FR::TXT_CONFIGURATION, true);
    display.display();

    result = widgets.menuListeV2(CuvinoGUI::_creeLigneMenuConfiguration, NULL, NB_LIGNE_MAX + nbAvt + nbApres + ((horlogeOK) ? 1 : 0) + 5, 15, 8, LCDHEIGHT, 0, LCDWIDTH, 1, depart);
    depart = result.result;

    if ( result.touche == 0 && result.result != NB_LIGNE_MAX + nbAvt + nbApres + ((horlogeOK) ? 1 : 0) + 4) { // touche selection
      if ( result.result < nbAvt ) { // cas des fonctions avant le menu(fonctions ajoutées avant)
        for (unsigned char j = 0, i = 0; i < NB_MAX_FCT_MENU; ++i) {
          if ( addLignes[i].pos == POSITION_MENU_AVT) {
            if (j == result.result) if( ! addLignes[i].fctMenu()) return false; // appel de la fonction ajoutée et si celle ci renvoi false -> false
            ++j;
          }
        }
      } else if ( result.result < (NB_LIGNE_MAX + nbAvt)) { // config ligne
        cuveConfig=listeCuve[result.result-nbAvt]; // cuve à configurer
        #ifdef DEBUG
        cuveConfig.print(Serial);
        #endif
        if( ! CuvinoGUI::menuConfigCuve(cuveConfig)) return false;
        #ifdef DEBUG
        cuveConfig.print(Serial);
        #endif
        CuvinoCore::modifieCuve(result.result-nbAvt,cuveConfig);
      } else if ( result.result == (NB_LIGNE_MAX + nbAvt)) { // config local

        cuveConfig=listeCuve[NB_LIGNE_MAX]; // cuve à configurer
        #ifdef DEBUG
        cuveConfig.print(Serial);
        #endif
        if( ! CuvinoGUI::menuConfigCuve(cuveConfig)) return false; // si false -> false
        #ifdef DEBUG
        cuveConfig.print(Serial);
        #endif
        CuvinoCore::modifieCuve(NB_LIGNE_MAX,cuveConfig);
      } else if (horlogeOK && result.result == NB_LIGNE_MAX + nbAvt + 1) { // config heure
        if( ! CuvinoGUI::menuConfigurationHeureV2()) return false;
      } else if (result.result == NB_LIGNE_MAX + nbAvt + nbApres + ((horlogeOK) ? 1 : 0) + 1) { // reinitialisation
        display.clearDisplay();
        display.printlnPGM(FR::TXT_REINITIALISER);
        display.setCursor(0, 8);
        display.printPGM(FR::TXT_LA_MEMOIRE);
        display.write('?');
        result = widgets.menuListeOuiNon(24);
        if( result.touche==TIMEOUT_LED) return false;
        if ( result.touche == 0 && result.result == 0) {
          CuvinoCore::reglageDefaut();
        }
      } else if (result.result <= NB_LIGNE_MAX + nbAvt + nbApres + ((horlogeOK) ? 1 : 0) ) { // cas des fonctions après le menu(fonctions ajoutées après, mais avant la réinitialisation)
        for (unsigned char j = (NB_LIGNE_MAX + nbAvt + ((horlogeOK) ? 1 : 0) + 1), i = 0; i < NB_MAX_FCT_MENU; ++i) {
          if ( addLignes[i].pos == POSITION_MENU_APRES) {
            if (j == result.result) if( ! addLignes[i].fctMenu()) return false; // appel de la fonction ajoutée et si celle ci renvoi false -> false
            ++j;
          }
        }
      } else if( result.result == NB_LIGNE_MAX + nbAvt + nbApres + ((horlogeOK) ? 1 : 0) + 2 ) { // arrêt EV
        CuvinoGUI::arretEV();
      } else if( result.result == NB_LIGNE_MAX + nbAvt + nbApres + ((horlogeOK) ? 1 : 0) + 3 ){ // reset
        display.clearDisplay();
        display.printlnPGM(FR::TXT_ETES_VOUS_SUR);
        display.printPGM(FR::TXT_DE_FAIRE);
        display.print(' ');
        display.printlnPGM(FR::TXT_RESET);
        result = widgets.menuListeOuiNon(32);
        if( result.result ==0 && result.touche==0){
          CuvinoCore::reset();
        }
      }
    } else {
      for ( uint8_t i = 0; i < NB_LIGNE_MAX; ++i) {
        if (listeCuve[i].tempConsigneCuve != TEMP_LIGNE_NON_CONFIGURE) { // si il y a au moins une ligne configurée, on peut quitter

          #ifdef DEBUG
          debugGUI.printlnPGM(DEBUG_ligneTrait);
          for(unsigned char i=0;i<NB_LIGNE_MAX;++i){
            listeCuve[i].print(debugGUI);
            debugGUI.println();
          }
          #endif

          CuvinoCore::saveParams(); // on enregistre les paramètres

          #ifdef DEBUG
          debugGUI.printlnPGM(DEBUG_ligneTrait);
          for(unsigned char i=0;i<NB_LIGNE_MAX;++i){
            listeCuve[i].print(debugGUI);
            debugGUI.println();
          }
          #endif

          return true; // et on quitte
        }
      }
      display.clearDisplay();
      display.printPGM(FR::TXT_ERR_AUCUNE_CUVE_CONF);
      display.display();
      display.aucuneTouche();
      display.getTouche(-1);
    }
  }
}


void CuvinoGUI::_creeLigneMenuConfiguration(uint8_t i, char* chaine, uint8_t taille, void* arg) {

  unsigned char nbAvt = 0;
  unsigned char nbApres = 0;
  for (unsigned char i = 0; i < NB_MAX_FCT_MENU; ++i) {
    switch (addLignes[i].pos) {
      case POSITION_MENU_AVT:
        nbAvt++;
        break;
      case POSITION_MENU_APRES:
        nbApres++;
        break;
    }
  }

  if ( i < nbAvt ) { // cas des fonctions avant le menu(fonctions ajoutées avant)
    for (unsigned char j = 0, k = 0; k < NB_MAX_FCT_MENU; ++k) {
      if ( addLignes[k].pos == POSITION_MENU_AVT) {
        if (j == i) strcpy_P(chaine, addLignes[k].nomPGM);
        ++j;
      }
    }
  } else if ( i < (NB_LIGNE_MAX + nbAvt)) { // config ligne
    strcpy_P(chaine, FR::TXT_CUVE, true);
    chaine[4] = ' ';
    //chaine[5] = i - 1 + 48;
    chaine[5]=listeCuve[i-nbAvt].nom;
    chaine[6] = ' ';
    if ( listeCuve[i - nbAvt].tempConsigneCuve == TEMP_LIGNE_NON_CONFIGURE) strcpy_P(&chaine[7], FR::TXT_NO_CONF);
    else strcpy_P(&chaine[7], FR::TXT_CONF_OK);

  } else if ( i == (NB_LIGNE_MAX + nbAvt)) { // config local
    strcpy_P(chaine, FR::TXT_SONDE, true);
    chaine[5] = ' ';
    strcpy_P(&chaine[6], FR::TXT_DU_LOCAL, false, 3);
  } else if (horlogeOK && i == NB_LIGNE_MAX + nbAvt + 1) { // config heure
    strcpy_P(chaine, FR::TXT_REGLAGE, true);
    chaine[7] = ' ';
    strcpy_P(&chaine[8], FR::TXT_HEURE);
  } else if (i == NB_LIGNE_MAX + nbAvt + nbApres + ((horlogeOK) ? 1 : 0) + 1) { // reinitialisation
    strcpy_P(chaine, FR::TXT_REINIT_MEM, true);
  } else if (i == NB_LIGNE_MAX + nbAvt + nbApres + ((horlogeOK) ? 1 : 0) + 2) { // arrêt EV
    strcpy_P(chaine, FR::TXT_ARRET, true);
    chaine[5]=' ';
    strcpy_P(&chaine[6],FR::TXT_EV);
  } else if (i == NB_LIGNE_MAX + nbAvt + nbApres + ((horlogeOK) ? 1 : 0) + 3) { // reset
    strcpy_P(chaine, FR::TXT_RESET, true);
  } else if (i == NB_LIGNE_MAX + nbAvt + nbApres + ((horlogeOK) ? 1 : 0) + 4) { // retour
    strcpy_P(chaine, FR::TXT_RETOUR, true);
  } else if (i <= NB_LIGNE_MAX + nbAvt + nbApres + ((horlogeOK) ? 1 : 0)) { // cas des fonctions après le menu(fonctions ajoutées après, mais avant la réinitialisation)
    for (unsigned char j = (NB_LIGNE_MAX + nbAvt + ((horlogeOK) ? 1 : 0) + 1), k = 0; k < NB_MAX_FCT_MENU; ++k) {
      if ( addLignes[k].pos == POSITION_MENU_APRES) {
        if (j == i) strcpy_P(chaine, addLignes[k].nomPGM);
        ++j;
      }
    }
  }
  chaine[21] = 0;
}

bool CuvinoGUI::arretEV(){
  signed int data[NB_LIGNE_MAX+1];

  if( xSemaphoreTake(semaphoreAccesDataCore, TEMPS_MAX_ACCES_DATA_CORE_TICK )){ // prend l'acces aux data du core
    for(unsigned char i=0;i<=NB_LIGNE_MAX;++i){
      data[i]=listeCuve[i].tempConsigneCuve;
      listeCuve[i].tempConsigneCuve=TEMP_ARRET;
    }

    xSemaphoreGive(semaphoreAccesDataCore); // rend l'acces au data du core

    // atter de toutes les EV
    unsigned char depart=0;
    for(;;){
      display.clearDisplay();

      display.printPGM(FR::TXT_GESTION);
      display.print(' ');
      display.printPGM(FR::TXT_EV);

      retourSelect result=widgets.menuListeV2(CuvinoGUI::_creeLigneMenuArretEV, NULL, 3, 15, 8, LCDHEIGHT, 0, LCDWIDTH, 1, depart);

      if( result.touche==0){
        if( result.result == 2) break;
        else if( result.result==0){ // ouverture EV
          for(unsigned char i=0;i<=NB_LIGNE_MAX;++i){
            if(listeCuve[i].EV_C.estConfigure()){
              listeCuve[i].EV_C.bougeEV(OUVERT);
            }
            if(listeCuve[i].EV_F.estConfigure()){
              listeCuve[i].EV_F.bougeEV(OUVERT);
            }
          }
        } else if( result.result == 1 ){ // fermeture EV
          for(unsigned char i=0;i<=NB_LIGNE_MAX;++i){
            if(listeCuve[i].EV_C.estConfigure()){
              listeCuve[i].EV_C.bougeEV(FERME);
            }
            if(listeCuve[i].EV_F.estConfigure()){
              listeCuve[i].EV_F.bougeEV(FERME);
            }
          }
        }
      }
    }

    // redémarrage de toutes les EV
    if( xSemaphoreTake(semaphoreAccesDataCore, TEMPS_MAX_ACCES_DATA_CORE_TICK )){ // prend l'acces aux data du core
      for(unsigned char i=0;i<=NB_LIGNE_MAX;++i){
        listeCuve[i].tempConsigneCuve=data[i];
      }

      xSemaphoreGive(semaphoreAccesDataCore); // rend l'acces au data du core
    }
  }
  return true;
}

void CuvinoGUI::_creeLigneMenuArretEV(uint8_t i, char* chaine, uint8_t taille, void* arg){
  switch(i){
    case 0:
      strcpy_P(chaine,FR::TXT_OUVERTURE);
      chaine[9]=0;
      strcpy_P(&chaine[10],FR::TXT_EV);
      break;
    case 1:
      strcpy_P(chaine,FR::TXT_FERMETURE);
      chaine[9]=0;
      strcpy_P(&chaine[10],FR::TXT_EV);
      break;
    case 2:
      strcpy_P(chaine,FR::TXT_RETOUR);
      break;
  }
  return;
}

bool CuvinoGUI::menuConfigurationHeureV2(void) {
  RtcDateTime dateTime;
  if (!Rtc.IsDateTimeValid()) {
    dateTime = RtcDateTime(lastDate);
  } else {
    dateTime = Rtc.GetDateTime();
  }

  display.clearDisplay();
  display.printPGM(FR::TXT_REGLAGE, true);
  display.write(' ');
  display.printPGM(FR::TXT_HEURE);

  if ( widgets.regleHeure(dateTime) == 1) {
    Rtc.SetDateTime(dateTime);
    lastDate = dateTime.TotalSeconds();
    return true;
  } else return false;
}


void CuvinoGUI::_actualiseTemp_menuConfigCuve(void* arg) {
  /* fonction appellé toutes les secondes pour actualiser l'affichage de la température dans le cadre des fonctions menuConfigCuve et menuConfigLocal */
  structDataMenuConfigCuve* data = (structDataMenuConfigCuve*)arg;

  data->temperature = data->cuve->getTemperature();
  widgets.affLigneSelectSonde( data->y, data->cuve->sonde->addr, data->temperature);
  display.display();
  data->cuve->demandeMesureTemp();
}


bool CuvinoGUI::menuConfigCuve(CuveV2& cuve) {
  /* fonction de configuration d'une cuve */
  retourSelect result;
  structDataMenuConfigCuve data;
  data.cuve = &cuve;
  data.temperature = TEMP_NON_LUE;
  data.y = 16;
  #ifdef DEBUG
  debugGUI.println("-------------");
  cuve.print(debugGUI);
  debugGUI.println();
  #endif
  for (;;) {
    display.clearDisplay();
    display.setCursor(24, 0);
    display.printPGM(FR::TXT_CUVE, true);
    display.write(' ');
    display.println((char)cuve.nom);
    display.display();
    if ( cuve.sonde->isDS18B20()) {
      display.printPGM(FR::TXT_SONDE, true);
      display.write(' ');
      display.printlnPGM(FR::TXT_ACTUELLE);
      widgets.affLigneSelectSonde(16, cuve.sonde->addr, TEMP_NON_LUE);
      data.timerID =  timer.every(1000, CuvinoGUI::_actualiseTemp_menuConfigCuve, (void*)&data);
    } else if( cuve.sonde->isDS3231() ){
      display.printPGM(FR::TXT_SONDE, true);
      display.write(' ');
      display.printlnPGM(FR::TXT_ACTUELLE);
      display.printPGM(FR::TXT_INTERNE);
      data.timerID =  timer.every(1000, CuvinoGUI::_actualiseTemp_menuConfigCuve, (void*)&data);
    } else {
      display.printPGM(FR::TXT_AUCUNE, true);
      display.write(' ');
      display.printlnPGM(FR::TXT_SONDE);
      display.printlnPGM(FR::TXT_CONFIGURE);
    }

    result = widgets.menuListeV2(CuvinoGUI::_creeLigneMenuConfigCuve, (void*)&data, ((cuve.tempConsigneCuve==TEMP_SONDE_LOCAL)?4:6), 15, 24);
    if( result.touche==TIMEOUT_LED) return false;

    if ( result.touche == (BT_BAS | BT_HAUT) || result.touche == BT_ESC
      || result.result == 5 || ((result.touche&TIMEOUT) != 0)
      || (cuve.tempConsigneCuve==TEMP_SONDE_LOCAL && result.result==2)) break;


    if ( result.touche == 0 && ((result.result == 3 && cuve.tempConsigneCuve!=TEMP_SONDE_LOCAL)||(result.result == 1 && cuve.tempConsigneCuve==TEMP_SONDE_LOCAL) )) { // modification du nom de la cuve
      timer.stop(data.timerID);
      char n=widgets.menuModifNom(cuve.nom);
      if( n==0) return false;
      else cuve.nom=n;
    } else if( result.touche == 0 && result.result == 4 ){ // calibrage sonde
      if( !cuve.hasSondeTemp() ){ // aucune sonde configurée

      } else{ // sonde configurée
        timer.stop(data.timerID);
        widgets.menuCalibrageSonde(*cuve.sonde,cuve.nom);
      }
    } else if ( result.touche == 0 && result.result == 0) { // modification de la sonde
      timer.stop(data.timerID);
      if( widgets.menuSelectSondeForV2(cuve, listeCuve, NB_LIGNE_MAX+1)==false) return false;
      data.temperature = TEMP_NON_LUE;

      #ifdef DEBUG
      debugGUI.printPGM(DEBUG_sondeConfigure);
      #endif
    } else if ( result.touche == 0 && (result.result == 1 || result.result == 2)) { // modification de l'électrovanne
      timer.stop(data.timerID);

      // texte d'entête du menuConfigEV
      display.clearDisplay();
      display.setCursor(0, 0);
      display.printPGM(FR::TXT_CUVE, true);
      display.write(' ');
      display.print((char)cuve.nom);
      display.printPGM(FR::TXT_EV);
      display.write(' ');
      if ( result.result == 2) { //EV chaud
        display.printPGM(FR::TXT_CHAUD);
      } else { // EV Froid
        display.printPGM(FR::TXT_FROID);
      }

      if( xSemaphoreTake(semaphoreAccesDataCore, TEMPS_MAX_ACCES_DATA_CORE_TICK )){ // prend l'acces aux data du core
        signed int sauveTemp = cuve.tempConsigneCuve; // enregistre la température de consigne de la cuve
        cuve.tempConsigneCuve = TEMP_ARRET; // et arrête sa surveillance

        xSemaphoreGive(semaphoreAccesDataCore); // rend l'acces au data du core

        if( ! widgets.menuConfigEV(((result.result == 2) ? cuve.EV_C : cuve.EV_F)) ) return false; // configuration de l'EV

        if( xSemaphoreTake(semaphoreAccesDataCore, TEMPS_MAX_ACCES_DATA_CORE_TICK )){ // prend l'acces aux data du core
          cuve.tempConsigneCuve = sauveTemp;// remet la température de consigne de la cuve
        }
        xSemaphoreGive(semaphoreAccesDataCore); // rend l'acces au data du core
      }

    }
  }
  timer.stop(data.timerID); //avant de quitter arrête le timer!

  if ( ! cuve.sonde->isSondeTemp() && cuve.EV_F.mode == EV_NON_CONFIGURE) { // si sonde et electrovanne non configuré
    cuve.tempConsigneCuve = TEMP_LIGNE_NON_CONFIGURE;
  } else if ( ! cuve.sonde->isSondeTemp() ) { // Sonde non configurée
    cuve.tempConsigneCuve = TEMP_SONDE_NON_CONFIGURE;
  } else if (cuve.EV_F.mode == EV_NON_CONFIGURE) { // EV froid non configurée
    cuve.tempConsigneCuve = TEMP_EV_NON_CONFIGURE;
  } else { // config OK !!
    cuve.tempConsigneCuve = TEMP_ARRET;
  }

  #ifdef DEBUG
  debugGUI.println("-------------");
  cuve.print(debugGUI);
  debugGUI.println();
  #endif

  return true;
}


void CuvinoGUI::_creeLigneMenuConfigCuve(uint8_t i, char* chaine, uint8_t taille, void* arg) {
  structDataMenuConfigCuve* data = (structDataMenuConfigCuve*)arg;
  if ( i != 0 && data->cuve->tempConsigneCuve == TEMP_SONDE_LOCAL) i += 2; // pour adaptation avec le menu de config du local (pas d'EV)
  switch (i) {
    case 0:
      strcpy_P(chaine, FR::TXT_MODIFIER);
      chaine[8] = ' ';
      strcpy_P(&chaine[9], FR::TXT_SONDE);
      break;
    case 1: // EV Froid
    case 2: // EV Chaud
      ElectroVanne2 *EV;
      if ( i == 1) EV = &data->cuve->EV_F;
      else EV = &data->cuve->EV_C;

      strcpy_P(chaine, FR::TXT_EV);
      chaine[2] = ((i == 1) ? 'F' : 'C');
      chaine[3] = ' ';

      if ( EV->numero < 36) chaine[4] = EV->numero + (( EV->numero < 10) ? 48 : 55);
      else chaine[4] = ' ';
      chaine[5] = ' ';
      strcpy_P(&chaine[6],TXT_EV(EV->mode));
      chaine[14] = 0;
      break;
    case 3:// modif nom
      strcpy_P(chaine, FR::TXT_MODIFIER);
      chaine[8] = ' ';
      strcpy_P(&chaine[9], FR::TXT_NOM);
      chaine[12] = ':';
      chaine[13] = data->cuve->nom;
      chaine[14] = 0;
      break;
    case 4: // calibrage sonde
      strcpy_P(chaine, FR::TXT_CALIBRAGE,true);
      chaine[8] = ' ';
      strcpy_P(&chaine[9], FR::TXT_SONDE, false);
      break;
    case 5: // retour
      strcpy_P(chaine, FR::TXT_RETOUR);
      break;
    default:
      break;
  }
}

/***************************************
       programmateur
***************************************/

BlocMemProgramme blocProg;
uint32_t prochainCuvinoProg=0;
bool startCuvinoProg=false;
uint32_t dateLancementCuvino=0;

void CuvinoProgramme::loadProgramme(void){
  blocProg = BlocMemProgramme(adresseBloc.programmes);
  if( blocProg.alloc() < NB_PROGRAMME_MAX ) blocProg.alloc(NB_PROGRAMME_MAX);
}
void CuvinoProgramme::initProgramme(bool loadCuvino){
  if (! horlogeOK) {
    #ifdef DEBUG
      debugProg.printPGM(DEBUG_erreurRTC_horloge_non_valide);
    #endif
    return;
  } else {
    RtcDateTime now = Rtc.GetDateTime();
    dateLancementCuvino=now.TotalSeconds()-(millis()/1000); // enregistre la date du lancement de cuvino
    /* loadCuvino = true dans la séquence de démarrage.
      si on est dans la séquence de démarrage, on peut éxécuter les programmes en attente,
      sinon on ne peut pas (car celà veux dire que l'heure a été reréglé et on ne sait pas où on en est!)
    */
    if( loadCuvino && now.TotalSeconds() > blocProg.getDateProchain()){ // la date du prochain programme en mémoire est déjà passé ! on a raté au moins 1 évènement
      programmeInterval(blocProg.getDateDernierAcces(),now.TotalSeconds(),false);
    }

    // la date du prochain programme est dans le futur
    CuvinoProgramme::checkProchain(now);

    startCuvinoProg=true;
  }
}

void CuvinoProgramme::modifActualiseAffTemp(){
  if(  NB_LIGNE_MAX == 3 || (listeCuve[3].tempConsigneCuve == TEMP_LIGNE_NON_CONFIGURE && listeCuve[NB_LIGNE_MAX].sonde->isSondeTemp())){
    int8_t x,y;
    #if NB_LIGNE_MAX >= 2
        x = (3 * 42) % 84 + 12;
        y = (3 >= 2) ? 35 : 11;
    #elif NB_LIGNE_MAX == 1
        x = (3 * 42) % 84 + 12;
        y = 26;
    #endif
    display.setTextSize(0);
    display.setCursor(x-12,y+7);
    RtcDateTime dateTime=RtcDateTime(prochainCuvinoProg);
    display.printTime(dateTime.Hour(),dateTime.Minute());
    display.setTextSize(1);
  }
}

void CuvinoProgramme::verifProchain(void){
  #ifdef DEBUG
  debugProg.printPGM(DEBUG_CuvinoProgramme);
  debugProg.printlnPGM(DEBUG_verifProchain);
  #endif
  if( horlogeOK ){ // heure valide
    if( ! startCuvinoProg ) { // si le module programme n'est pas déjà démarré
      CuvinoProgramme::initProgramme(false); // lancement du module, on précise que l'on est pas ds la séquence de démarrage
    }


    RtcDateTime now=Rtc.GetDateTime();
    #ifdef DEBUG
    debugProg.setWriteMode(modeDebugComplet);
    debugProg.printPGM(DEBUG_now);
    debugProg.print(now.TotalSeconds(),BIN);
    debugProg.write(' ');
    debugProg.printPGM(DEBUG_prochain);
    debugProg.print(prochainCuvinoProg,BIN);
    debugProg.write(' ');
    debugProg.printPGM(DEBUG_dans);
    debugProg.write(' ');
    debugProg.print(prochainCuvinoProg - now.TotalSeconds());
    debugProg.printlnPGM(DEBUG_secondes);
    debugProg.setWriteMode(modeDebug);
    #endif
    if( now.TotalSeconds() > prochainCuvinoProg){
      /* éxécution de tous les programmes passés depuis prochain ! */
      CuvinoProgramme::programmeInterval(prochainCuvinoProg,now.TotalSeconds(),true);
    }
  } else { // heure invalide
    if( dateLancementCuvino != 0
      && dateLancementCuvino+(millis()/1000) > prochainCuvinoProg  ){ // si on connais la date de lancement de cuvino
        CuvinoProgramme::programmeInterval(prochainCuvinoProg, dateLancementCuvino+(millis()/1000));
    }
  }
}

void CuvinoProgramme::programmeInterval(uint32_t debut,uint32_t fin,bool write){
  #ifdef DEBUG
  debugProg.printPGM(DEBUG_CuvinoProgramme);
  debugProg.printPGM(DEBUG_programmeInterval);
  debugProg.print(debut);
  debugProg.print(',');
  debugProg.print(fin);
  debugProg.println(')');
  #endif
  Programme prog;
  RtcDateTime dateTime=RtcDateTime(debut);
  uint8_t pro=blocProg.prochain(dateTime); // récupère le numéro du prochain programme
  #ifdef DEBUG
  debugProg.printPGM(DEBUG_programme);
  debugProg.println(pro);

  #endif
  while( pro != 0xFF){ // tant qu'il y a un prochain programme

    blocProg.get(pro,prog); // récupère ce programme
    dateTime=RtcDateTime(prog.prochain(dateTime,blocProg._HC_HP));
    #ifdef DEBUG
    debugProg.printPGM(DEBUG_programme);
    debugProg.print(pro);
    debugProg.print(' ');
    debugProg.printPGM(DEBUG_datePro);
    debugProg.println(dateTime.TotalSeconds());
    #endif
    if( dateTime.TotalSeconds() > fin) break; // si programme après la date de fin de l'interval

    /* activation du programme */
    listeCuve[prog.cuve].tempConsigneCuve=prog.temp;
    #ifdef DEBUG
    debugProg.printPGM(DEBUG_programme);
    debugProg.print(pro);
    debugProg.print(' ');
    debugProg.printPGM(DEBUG_date);
    debugProg.print(dateTime.TotalSeconds());
    debugProg.printPGM(FR::TXT_FLECHE);
    debugProg.printPGM(FR::TXT_CUVE);
    debugProg.print(listeCuve[prog.cuve].nom);
    debugProg.print('(');
    debugProg.print(prog.cuve);
    debugProg.print(')');
    debugProg.printPGM(FR::TXT_NOUVELLE);
    debugProg.write(' ');
    debugProg.printPGM(FR::TXT_TEMPERATURE);
    debugProg.write(':');
    debugProg.print(prog.temp/16);
    debugProg.print('.');
    if( (prog.temp%16)==1) debugProg.print('0');
    debugProg.print((prog.temp%16)*625);
    debugProg.println((char)247);
    #endif


    /* programme suivant */
    pro=blocProg.prochain(dateTime,pro);
  }

  if( dateTime.TotalSeconds() <= fin) dateTime=RtcDateTime(fin+1);

  if( write ){
    if( pro != 0xFF){
      blocProg.setDateProchain(dateTime.TotalSeconds());
      prochainCuvinoProg=dateTime.TotalSeconds();
    } else {
      prochainCuvinoProg=0xFFFFFFFF;
      blocProg.setDateProchain(0xFFFFFFFF);
    }
    blocProg.setDateDernierAcces(fin);
  }
}

void CuvinoProgramme::checkProchain(RtcDateTime& now){
  Programme prog;

  uint8_t pro=blocProg.prochain(now);

  if( pro != 0xFF){
    Programme prog;
    blocProg.get(pro,prog);

    unsigned long dateTime = prog.prochain(now, blocProg._HC_HP);
    blocProg.setDateProchain(dateTime);
    prochainCuvinoProg=dateTime;
  } else {
    prochainCuvinoProg=0xFFFFFFFF;
    blocProg.setDateProchain(0xFFFFFFFF);
  }
  blocProg.setDateDernierAcces(now.TotalSeconds());

  #ifdef DEBUG
    debugProg.printPGM(DEBUG_actualisationDateProProg);
    debugProg.println(prochainCuvinoProg);
  #endif
}

bool CuvinoProgramme::menuConfigProgrammateur(void) {
  Programme programme;

  for (;;) {
    cmdeInterne cmde;
    #ifdef DEBUG
    debugProg.print("alloc:");
    debugProg.print(blocProg.alloc());
    debugProg.print("  count:");
    debugProg.println(blocProg.count());
    #endif

    display.clearDisplay();
    display.printPGM(FR::TXT_PROGRAMMATEUR);
    display.setTextSize(0);
    display.setCursor(0, 16);
    display.printPGM(FR::TXT_CUVE);
    display.write(' ');
    display.printPGM(FR::TXT_DATE);
    display.printEspace(2);
    display.printPGM(FR::TXT_HEURE);
    display.write(' ');
    display.printPGM(FR::TXT_TEMP);
    display.display();

    retourSelect result = widgets.menuListeV2(CuvinoProgramme::_creeLigneMenuProgrammateur, (void*)&programme, min(21, blocProg.count() + 2), 22, 22, LCDHEIGHT, 0, LCDWIDTH, 1, 0, 0);
    display.setTextSize(1);

    if( result.touche==TIMEOUT_LED) return false;
    if ( result.touche == (BT_BAS | BT_HAUT) || result.touche==BT_ESC) return true;

    if ( result.touche == 0) {

      if ( result.result == 0 && blocProg.alloc() > blocProg.count()) { // ajout d'un nouveau programme
        programme = Programme();
        programme.defaut();
      } else if( (result.result - ((blocProg.alloc() > blocProg.count())?1:0)) >= blocProg.count()){ // retour
        return true;
      } else { // modification d'un programme existant
        blocProg.get(result.result - (blocProg.alloc() > blocProg.count())?1:0, programme);
      }

      if ( regleProgramme(programme)) {
        if( programme.cuve == 0xFF){ // suppression
          if ( result.result != 0 || blocProg.alloc() == blocProg.count()) { // on est en mode modification d'un programme et non en création (en création il n'y a pas besoin de supprimer)
            if( blocProg.alloc() > blocProg.count() ) result.result--; // cas où tout les emplacement ne sont pas plein, la ligne nouveau programme apparait en 1er -> on doit décrémenter result.result de 1
            #ifdef DEBUG
            debugProg.printPGM(FR::TXT_SUPPRESSION);
            debugProg.write(' ');
            debugProg.printPGM(FR::TXT_PROGRAMME);
            debugProg.write(' ');
            debugProg.println(result.result);
            #endif
            blocProg.del(result.result);
            cmde=cmdeNewProg;
            xQueueSend(queueCmdeToProgramme,&cmde,0); // envoi au gestionnaire de programme l'info qu'un programme est créé/modifié/supprimmé
          }
        } else { // enregistrement
          #ifdef DEBUG
          debugProg.printPGM(FR::TXT_ENREGISTREMENT);
          debugProg.write(' ');
          debugProg.printPGM(FR::TXT_PROGRAMME);

          plageHC_HP HC_HP;
          blocProg.getHC_HP(HC_HP);
          RtcDateTime dateTimeProchain=Rtc.GetDateTime();
          dateTimeProchain=RtcDateTime(programme.prochain(dateTimeProchain,HC_HP));
          #endif
          if ( result.result == 0 && blocProg.alloc() > blocProg.count()) { // ajout d'un programme
            #ifdef DEBUG
            debugProg.printPGM(FR::TXT_AJOUTE);
            debugCore.write(' ');
            debugCore.printlnPGM(FR::TXT_PROGRAMME);
            #endif
            blocProg.add(programme);
          } else { // modification d'un programme
            #ifdef DEBUG
            debugProg.printPGM(FR::TXT_ACTUALISE);
            debugProg.write(' ');
            debugProg.printPGM(FR::TXT_PROGRAMME);
            debugProg.write(' ');
            debugProg.println(result.result);
            #endif
            if( blocProg.alloc() > blocProg.count() ) result.result-=1; // cas où tout les emplacement ne sont pas plein, la ligne nouveau programme apparait en 1er -> on doit décrémenter result.result de 1
            blocProg.set(result.result, programme);
          }

          cmde=cmdeNewProg;
          xQueueSend(queueCmdeToProgramme,&cmde,0); // envoi au gestionnaire de programme l'info qu'un programme est créé/modifié/supprimmé

  #ifdef DEBUG
          display.clearDisplay();
          display.printPGM(FR::TXT_PROCHAIN);
          display.print(' ');
          display.printlnPGM(FR::TXT_APPEL);
          display.printDateTime(dateTimeProchain.Day(),dateTimeProchain.Month(),(uint8_t)(dateTimeProchain.Year()-2000),dateTimeProchain.Hour(),dateTimeProchain.Minute());
          display.display();
          display.aucuneTouche();
          if( display.getTouche(-1) == TIMEOUT_LED ) return false;
  #endif
        }

        /* actualise la date du prochain programme */
        if( horlogeOK){
          RtcDateTime now=Rtc.GetDateTime();
          CuvinoProgramme::checkProchain(now);
        } else if( startCuvinoProg){
          RtcDateTime now=RtcDateTime(dateLancementCuvino+(millis()/1000));
          CuvinoProgramme::checkProchain(now);
        }

      }
    }
  }
}

void CuvinoProgramme::_creeLigneMenuProgrammateur(uint8_t i, char* chaine, uint8_t taille, void* arg) {
  Programme* programme = (Programme*)arg;
  RtcDateTime dateTimeProg;
  if (  blocProg.alloc() > blocProg.count() ) {
    if ( i == 0) {
      strcpy_P(chaine, FR::TXT_AJOUTER_PROGRAMME);
      return;
    }
    --i;
  }
  if ( i >= blocProg.count()) {
    strcpy_P(chaine, FR::TXT_RETOUR);
    return;
  }

  blocProg.get(i, *programme);
  #ifdef DEBUG
  debugProg.print("i:");
  debugProg.print(i);
  programme->println(Serial);
  #endif

  chaine[0] = listeCuve[programme->cuve].nom;
  for(i=1;i<22;++i) chaine[i]=' ';

  if ( ! programme->estRepetition()) { // programme unique
    dateTimeProg = RtcDateTime(programme->date);
    chaine[2] = dateTimeProg.Day() / 10 + 48;
    chaine[3] = dateTimeProg.Day() % 10 + 48;
    chaine[4] = '/';
    chaine[5] = dateTimeProg.Month() / 10 + 48;
    chaine[6] = dateTimeProg.Month() % 10 + 48;
    chaine[7] = '/';
    chaine[8] = (dateTimeProg.Year()-2000) / 10 + 48;
    chaine[9] = (dateTimeProg.Year()-2000) % 10 + 48;

    chaine[11] = dateTimeProg.Hour() / 10 + 48;
    chaine[12] = dateTimeProg.Hour() % 10 + 48;
    chaine[13] = ':';
    chaine[14] = dateTimeProg.Minute() / 10 + 48;
    chaine[15] = dateTimeProg.Minute() % 10 + 48;
  } else {
    dateTimeProg = programme->getRtcDateTime();

    strcpy_P(&chaine[2], CHAINE_INIT_MENU_PROGRMMATEUR);
    chaine[4] = ' ';

    switch (programme->mode()) {
      case MODE_PROG_M:
        if( dateTimeProg.Day()>=10) chaine[2]=dateTimeProg.Day()/10 + 48;
        chaine[3]=(dateTimeProg.Day() % 10) + 48;
        chaine[5]='D';
        chaine[5]='U';
        chaine[7]='M';
        break;
      case MODE_PROG_S:
        strcpy_P(&chaine[5], FR::JOURS[(dateTimeProg.Day()) % 7]);
        chaine[8]=' ';
        break;
      case MODE_PROG_J:
        if( programme->freqRepetition()>9) chaine[5]=programme->freqRepetition()/10+48;
        chaine[6]=(programme->freqRepetition() % 10) + 48;
        chaine[7]=' ';
        chaine[8]='J';
        break;
      case MODE_PROG_H:
        if( programme->freqRepetition()>9) chaine[5]=programme->freqRepetition()/10+48;
        chaine[6]=(programme->freqRepetition() % 10) + 48;
        chaine[8]='H';
        break;
      case MODE_PROG_10M:
        chaine[5]='1';
        chaine[6]='0';
        chaine[7]='m';
        chaine[8]='i';
        chaine[9]='n';
        break;
      case MODE_PROG_HC_HP:
        strcpy(&chaine[2], FR::TXT_PASSAGE_HC_HP);
        chaine[15]=' ';
        break;
      case MODE_PROG_HP_HC:
        strcpy(&chaine[2], FR::TXT_PASSAGE_HP_HC);
        chaine[15]=' ';
        break;
    }
    if ( programme->mode() < 0b100) {
      chaine[11] = dateTimeProg.Hour() / 10 + 48;
      chaine[12] = dateTimeProg.Hour() % 10 + 48;
      chaine[13] = ':';
      chaine[14] = dateTimeProg.Minute() / 10 + 48;
      chaine[15] = dateTimeProg.Minute() % 10 + 48;
    }
  }


  if( programme->temp == TEMP_ARRET){
    strcpy_P(&chaine[17], FR::TXT_STOP);
  } else if(programme->temp < TEMP_NON_LUE){
    strcpy_P(&chaine[17], FR::TXT_ERR);
  } else{
    if( programme->temp > -160 ) {
      if( programme->temp <0 )chaine[17]='-';
      else if( programme->temp >= 160) chaine[17]=programme->temp/16/10+48;
      chaine[18]=((programme->temp<0)?-programme->temp:programme->temp)/16+48;
      if(programme->temp%16!=0){
        chaine[19]='.';
        chaine[20]=(programme->temp*10)/16+48;
      }
    } else{
      chaine[17]='-';
      chaine[18]=(-programme->temp)/16/10+48;
      chaine[19]=(-programme->temp)/16+48;
      if(programme->temp%16!=0) chaine[20]='.';
    }
  }
  chaine[21]=0;

}

bool CuvinoProgramme::regleProgramme(Programme& programme) {
  bool nouveau=(programme.cuve==0xFF);

  #ifdef DEBUG
  debugProg.printPGM(DEBUG_CuvinoProgramme);
  debugProg.printlnPGM(DEBUG_regleProgramme);
  programme.println(Serial);
  #endif

  display.clearDisplay();
  display.printPGM(FR::TXT_AJOUTER_PROGRAMME, false, 8);

  display.setCursor(0, 16);
  display.printPGM(FR::TXT_CUVE, true);
  display.write(':');
  display.setCursor(0, 24);
  display.printPGM(FR::TXT_TEMP);
  display.write(':');

  for (int8_t pos = 0, tailleTemp=0;;) { // boucle infinie affichage
    uint8_t touche;
    // bouton cuve
    if ( pos == 0) display.setTextColor(WHITE, BLACK);
    display.setCursor(30, 16);
    if ( programme.cuve == 0xFF) display.write('-');
    else display.write((char)listeCuve[programme.cuve].nom);
    display.setTextColor(BLACK, WHITE);

    // boutton temp
    if (pos == 1) display.setTextColor(WHITE, BLACK);
    display.setCursor(30, 24);
    display.fillRect(30,24,6*tailleTemp,8,WHITE);
    if( programme.temp == TEMP_ARRET) tailleTemp=display.printPGM(FR::TXT_STOP);
    else tailleTemp=display.printTemp16e(programme.temp,0);
    display.setTextColor(BLACK, WHITE);

    // boutton supprimer
    if( ! nouveau){
      if (pos == 2) display.setTextColor(WHITE, BLACK);
      display.setCursor(0, 32);
      display.printPGM(FR::TXT_SUPPRIMER,true);
      display.setTextColor(BLACK, WHITE);
    }

    // boutton OK
    if (pos == 3) display.setTextColor(WHITE, BLACK);
    display.setCursor(6, 40);
    display.printPGM(FR::TXT_OK);
    display.setTextColor(BLACK, WHITE);

    //bouton retour
    if (pos == 4) display.setTextColor(WHITE, BLACK);
    display.setCursor(42, 40);
    display.printPGM(FR::TXT_RETOUR);
    display.setTextColor(BLACK, WHITE);

    display.display();

    for (;;) { // boucle infinie touche
      display.aucuneTouche();
      touche = display.getTouche(-1);
      if( touche==TIMEOUT_LED ) return false;
      if ( touche == (BT_HAUT | BT_BAS) || touche == BT_MENU || (pos == 4 && touche == BT_SELECT)) { // annuler ou appuis sur Haut & Bas
        if( nouveau ) programme.date=0;
        return true;
      }

      if ( touche == BT_BAS || touche == BT_HAUT) { // Haut ou Bas
        if (touche == BT_HAUT) --pos;
        else ++pos;
        if( nouveau && pos==2){
          pos=(touche==BT_HAUT)?1:3;
        }
        if( pos<0 ) pos=4;
        if( pos==5 ) pos=0;
        break;  // sortie boucle touche
      }
      if ( touche == BT_SELECT || touche==BT_ESC) break; // sortie boucle touche
    }// fin boucle touche
    if ( touche == BT_SELECT || touche==BT_ESC) {
      if( pos == 2 ) { // supprimer
        programme.defaut();
        return true;
      }
      if ( pos == 3 && programme.cuve < NB_LIGNE_MAX) break; // OK   -> sortie boucle affichage
      if ( pos == 1) { // temperature
        CuvinoCore::modifTempConsigne(programme.temp, (touche==BT_SELECT)?8:-8);
      } else { // cuve
        do {
          programme.cuve = (programme.cuve + ((touche==BT_SELECT)?1:-1)) % NB_LIGNE_MAX;
        } while ( !listeCuve[programme.cuve].estConfigure() );
      }
    }
  } // fin boucle infinie affichage
  #ifdef DEBUG
  RtcDateTime dateTimeProg;
  #endif
  if ( widgets.regleProgramme(programme) != 0) {
    #ifdef DEBUG
    plageHC_HP HC_HP;
    blocProg.getHC_HP(HC_HP);
    dateTimeProg=Rtc.GetDateTime();
    dateTimeProg=RtcDateTime(programme.prochain(dateTimeProg,HC_HP));
    debugProg.printPGM(FR::TXT_DATE);
    debugProg.write('=');
    debugProg.print(programme.date, HEX);
    debugProg.print('(');
    debugProg.print(dateTimeProg.Day());
    debugProg.print('/');
    debugProg.print(dateTimeProg.Month());
    debugProg.print('/');
    debugProg.print(dateTimeProg.Year());
    debugProg.print(' ');
    debugProg.print(dateTimeProg.Hour());
    debugProg.print(':');
    debugProg.print(dateTimeProg.Minute());
    debugProg.print(':');
    debugProg.print(dateTimeProg.Second());
    debugProg.print(") ");
    debugProg.printPGM(FR::TXT_MODE);
    debugProg.print(programme.mode(), HEX);
    debugProg.write(' ');
    debugProg.printPGM(FR::TXT_REPETITION);
    debugProg.write('=');
    debugProg.println(programme.freqRepetition());
#endif
    return true;
  }
#ifdef DEBUG
  debugProg.printPGM(FR::TXT_DATE);
  debugProg.write('=');
  debugProg.print(programme.date, HEX);
  debugProg.print('(');
  debugProg.print(dateTimeProg.Day());
  debugProg.print('/');
  debugProg.print(dateTimeProg.Month());
  debugProg.print('/');
  debugProg.print(dateTimeProg.Year());
  debugProg.print(' ');
  debugProg.print(dateTimeProg.Hour());
  debugProg.print(':');
  debugProg.print(dateTimeProg.Minute());
  debugProg.print(':');
  debugProg.print(dateTimeProg.Second());
  debugProg.print(") ");
  debugProg.printPGM(FR::TXT_MODE);
  debugProg.print(programme.mode(), HEX);
  debugProg.write(' ');
  debugProg.printPGM(FR::TXT_REPETITION);
  debugProg.write('=');
  debugProg.println(programme.freqRepetition());
#endif
  return false;
}

bool CuvinoProgramme::menuConfigurationHC_HP(void) {
  int8_t pos = -1;
  uint8_t touche;
  plageHC_HP plages;
  blocProg.getHC_HP(plages);

  for (;;) {
    display.clearDisplay();
    display.printPGM(FR::TXT_REGLAGE, true);
    display.write(' ');
    display.printPGM(FR::TXT_HC_HP);


    display.setCursor(0, 8);
    display.setTextSize(0);
    display.printPGM(FR::TXT_NB_DE_PLAGE);
    display.write(' ');
    if ( pos == -1) display.setTextColor(WHITE, BLACK);
    display.print(plages.nb_plages);
    if ( pos == -1) display.setTextColor(BLACK, WHITE);

    for (byte i = 0; i < 3; ++i) {
      if ( i < plages.nb_plages ) afficheLigneHC_HP(plages, i, 16 + i * 7, ((pos / 2) == i) ? (pos % 2 + 1) : 0);
    }

    if ( pos == 6) display.setTextColor(WHITE, BLACK);
    display.setCursor(6, 40);
    display.printPGM(FR::TXT_OK);
    if ( pos == 6) display.setTextColor(BLACK, WHITE);
    if ( pos == 7) display.setTextColor(WHITE, BLACK);
    display.setCursor(48, 40);
    display.printPGM(FR::TXT_RETOUR);
    if ( pos == 7) display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.display();

    display.aucuneTouche();
    touche = display.getTouche(-1);
    if( touche==TIMEOUT_LED) return false;

    if ( touche == BT_SELECT && pos == 6) {
      bool valid = true;
      for (byte i = 0; i < plages.nb_plages; ++i) {
        if ( plages.horaires[i * 2 + 1] <= plages.horaires[i * 2] || (i != 0 && plages.horaires[i * 2] <= plages.horaires[i * 2 - 1])) {
          valid = false;
          break;
        }
      }
      if ( valid) {
        blocProg.setHC_HP(plages);
        break;
      } else {
        display.fillRect(3, 21, 7, 8, WHITE);
        display.setCursor(4, 21);
        display.printPGM(FR::TXT_DONNEES_INCORRECTES);
        display.display();
        display.aucuneTouche();
        display.getTouche(-1);
      }
    } else if ((touche == BT_SELECT && pos == 7 ) || (touche == (BT_HAUT | BT_BAS)) ) {
      break;
    } else if ( touche == BT_SELECT) {
      if ( pos == -1) plages.nb_plages = (plages.nb_plages + 1) % 4;
      else plages.horaires[pos] = (plages.horaires[pos] + 1) % 48;
    } else if ( touche == BT_HAUT) {
      --pos; if ( pos == -2) pos = 7;
      while ( pos >= plages.nb_plages * 2 && pos < 6) --pos;
    } else if ( touche == BT_BAS) {
      ++pos; if ( pos == 8) pos = -1;
      while ( pos >= plages.nb_plages * 2 && pos < 6) ++pos;
    }
  }
  return true;
}

void CuvinoProgramme::afficheLigneHC_HP(plageHC_HP plages, uint8_t n, int8_t y, uint8_t sel) {
  display.setCursor(0, y);
  display.printPGM(FR::TXT_NB_DE_PLAGE, true, 6);
  display.write(' ');
  display.print(n + 1);
  display.write(':');
  if ( sel == 1) display.setTextColor(WHITE, BLACK);
  display.print(plages.horaires[n * 2] / 2);
  display.write('H');
  if ( plages.horaires[n * 2] % 2 == 0) display.write('0');
  display.print((plages.horaires[n * 2] % 2 ) * 30);
  if ( sel == 1) display.setTextColor(BLACK, WHITE);
  display.print(" a ");
  if ( sel == 2) display.setTextColor(WHITE, BLACK);
  display.print(plages.horaires[n * 2 + 1] / 2);
  display.write('H');
  if ( plages.horaires[n * 2 + 1] % 2 == 0) display.write('0');
  display.print((plages.horaires[n * 2 + 1] % 2) * 30);
  if ( sel == 2) display.setTextColor(BLACK, WHITE);
}


/****************************************
      Fonctions du systeme multi-task
****************************************/

void printDataRTOS(){
  /* affiche les infos sur le multi-task */

  return;
}

#ifdef DEBUG
void vApplicationStackOverflowHook( TaskHandle_t xTask, signed char *pcTaskName ){

  debug.printlnPGM(DEBUG_stackOverFlow);
  debug.printPGM(DEBUG_taskAffected);
  debug.println((char*)pcTaskName);


  debug.printlnPGM(DEBUG_freeStackSize);

  printDataRTOS();

}
#endif

void cuvinoProgramme(void* arg){
  #ifdef DEBUG
  debug.printPGM(DEBUG_CuvinoProgramme);
  debug.write(' ');
  debug.printlnPGM(DEBUG_start);
  #endif

  if( horlogeOK ){ // heure valide
    if( ! startCuvinoProg ) { // si le module programme n'est pas déjà démarré
      CuvinoProgramme::initProgramme(); // lancement du module
    }
  }

  for(;;){
    if( ! startCuvinoProg ){
      if( horlogeOK ) { // si module programme non démarré et horloge OK
        CuvinoProgramme::initProgramme(false); // on lance le module de programme en précisant que l'on est pas à la séquence de démarrage
      } else {
        vTaskDelay(60000 / portTICK_PERIOD_MS); // pause de 60 s avant nouvelle vérification de l'état de l'horloge
      }
    } else {

      RtcDateTime now=Rtc.GetDateTime();


      TickType_t delai= (1000*(prochainCuvinoProg - now.TotalSeconds())) / portTICK_PERIOD_MS;
      #ifdef DEBUG
      debugProg.printPGM(FR::TXT_NOW);
      debugProg.write(':');
      debugProg.print(now.TotalSeconds(),BIN);
      debugProg.write(' ');
      debugProg.print(FR::TXT_PROCHAIN);
      debugProg.write(':');
      debugProg.print(prochainCuvinoProg,BIN);
      debugProg.write(' ');
      debugProg.printPGM(DEBUG_dans);
      debugProg.write(' ');
      debugProg.print(prochainCuvinoProg - now.TotalSeconds());
      debugProg.write(' ');
      debugProg.printPGM(FR::TXT_SECONDE);
      debugProg.printPGM(DEBUG_s_var_delai);
      debugProg.print((delai*portTICK_PERIOD_MS)/1000);
      debugProg.println(')');

      #endif
      cmdeInterne cmde;

      if( xQueueReceive(queueCmdeToProgramme,&cmde,delai)){ // attends le délai avant le prochain évènement ou l'arrivé d'une commande
        // si reception de commande...
        #ifdef DEBUG
        debugProg.printPGM(DEBUG_cmdeRecu);
        debugProg.println((char)cmde);
        #endif
        now=Rtc.GetDateTime();
        switch((char)cmde){
          case cmdeNewProg: // un nouveau programme a été ajouté -> il faut recalculer prochainCuvinoProg

            CuvinoProgramme::checkProchain(now);

            /*uint8_t pro=blocProg.prochain(now);

            if( pro != 0xFF){
              Programme prog;
              blocProg.get(pro,prog);

              unsigned long dateTime = prog.prochain(now, blocProg._HC_HP);
              blocProg.setDateProchain(dateTime);
              prochainCuvinoProg=dateTime;
            } else {
              prochainCuvinoProg=0xFFFFFFFF;
              blocProg.setDateProchain(0xFFFFFFFF);
            }
            blocProg.setDateDernierAcces(now.TotalSeconds());*/
            break;
          default:
            break;
        }
      } else {
        // si pas de nouvelles commande alors ça veux dire que l'on a atteint le délai avant le prochain évènement
        #ifdef DEBUG
        debugProg.printPGM(DEBUG_delai_ecoule);
        #endif
        now=Rtc.GetDateTime();
        if( now > prochainCuvinoProg) CuvinoProgramme::programmeInterval(prochainCuvinoProg,now.TotalSeconds(),false);

        /*uint8_t pro=blocProg.prochain(now);
        if( pro != 0xFF){
          Programme prog;
          blocProg.get(pro,prog);

          unsigned long dateTime = prog.prochain(now, blocProg._HC_HP);
          blocProg.setDateProchain(dateTime);
          prochainCuvinoProg=dateTime;
        } else {
          prochainCuvinoProg=0xFFFFFFFF;
          blocProg.setDateProchain(0xFFFFFFFF);
        }
        blocProg.setDateDernierAcces(now.TotalSeconds());*/

        now+=1;
        CuvinoProgramme::checkProchain(now);

        cmde=cmdeActConsigne;
        xQueueSend(queueCmdeToTimer,&cmde,0); // on envoi la demande d'actualisation de l'écran

        cmde=cmdeSave;
        xQueueSend(queueCmdeToCore,&cmde,0); // on envoi une commande d'enregistrement des paramètres
      }

    }

  }
  #ifdef DEBUG
  debug.printPGM(DEBUG_Erreur_);
  debug.printPGM(DEBUG_exit);
  debug.print(' ');
  debug.printlnPGM(DEBUG_CuvinoProgramme);
  #endif
  vTaskDelete(NULL);
}

void cuvinoGUI(void* arg){
  #ifdef DEBUG
  debug.printPGM(DEBUG_CuvinoGUI);
  debug.write(' ');
  debug.printlnPGM(DEBUG_start);
  #endif
  unsigned char touche, i;
  unsigned char pos;
  CuvinoGUI::initAffStandard();

  for(;;){
    for(unsigned char i=0;i<NB_LIGNE_MAX;++i) CuvinoGUI::afficheLigne(i,i,false);
    for (;;) { // boucle d'attente d'une touche pour "réveiller"
      display.aucuneTouche();
      touche = display.getTouche(-1);
      if ( ((touche&TIMEOUT) == 0) && touche != 0) break;
    }
    pos = 0;
    CuvinoGUI::afficheLigne(pos, pos, true);
    for (;;) {
      display.aucuneTouche();
      touche = display.getTouche(-1);
      if ( touche== BT_MENU || touche == (BT_HAUT | BT_BAS) || touche==(BT_BAS|BT_HAUT|BT_SELECT)) {
        CuvinoGUI::menuConfiguration();
        CuvinoGUI::initAffStandard();
        CuvinoGUI::afficheLigne(pos, pos, true);
        display.display();
      } else if ( touche == BT_SELECT || touche == BT_ESC) {

        CuvinoGUI::afficheLigne(pos, pos);
        i = 0;
        do {
          pos = (pos + ((touche==BT_ESC)?(-1):1)) % NB_LIGNE_MAX;
          if( pos > 200) pos+=NB_LIGNE_MAX;
          ++i;
        } while ( i != 5 && (listeCuve[pos].tempConsigneCuve == TEMP_LIGNE_NON_CONFIGURE || listeCuve[pos].tempConsigneCuve == TEMP_EV_NON_CONFIGURE || listeCuve[pos].tempConsigneCuve == TEMP_SONDE_NON_CONFIGURE));
        if ( i != 5) CuvinoGUI::afficheLigne(pos, pos, true);
        display.display();
      } else if ( touche == BT_HAUT || touche == BT_BAS ) {
        CuvinoCore::modifTempConsigne(listeCuve[pos].tempConsigneCuve, ((touche == BT_HAUT) ? 16 : -16));

        cmdeInterne cmde=cmdeSave;
        xQueueSend(queueCmdeToCore,&cmde,10);

        CuvinoGUI::afficheLigne(pos, pos, true);
        display.display();
      } else if( touche == TIMEOUT_LED) break;
    }
  }
  #ifdef DEBUG
  debug.printPGM(DEBUG_Erreur_);
  debug.printPGM(DEBUG_exit);
  debug.print(' ');
  debug.printlnPGM(DEBUG_CuvinoGUI);
  #endif
  vTaskDelete(NULL);
}

void cuvinoCore(void* arg){
  #ifdef DEBUG
  debug.printPGM(DEBUG_CuvinoCore);
  debug.write(' ');
  debug.printlnPGM(DEBUG_start);
  #endif
  for(unsigned char i=0;i<NB_LIGNE_MAX;++i){
    if( listeCuve[i].estConfigure() ) listeCuve[i].demandeMesureTemp();
  }
  for(;;){
    cmdeInterne cmde;

    /*
    CuvinoCore::demandeMesureTemperature(); // fonction de demande de mesure des températures
    #ifdef DEBUG
    debug.print("ok");
    #endif
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    */

    //CuvinoProgramme::verifProchain(); // vérifie si appelle d'un nouveau programme
    CuvinoCore::controlTemp(); // algorithme d'ouverture/fermeture des vannes en fct de la température

    // envoi la commande d'actualisation de l'écran
    cmde=cmdeActTouteTemp;
    xQueueSend(queueCmdeToTimer,&cmde,0);

    //scheduler->sendMessage(taskTimer,new Message(code_cmde, cmdeActTouteTemp));
    #ifdef DEBUG
    for(unsigned char i=0;i<6;++i){ // cas debug => 6 boucle de 2,5s càd 15s
    #else
    for(unsigned char i=0;i<24;++i){ // cas normal => 12 boucle de 2,5s càd 60s
    #endif
      vTaskDelay(2500 / portTICK_PERIOD_MS);
    }

    cmde=cmdeNull;
    bool save=false;
    while( xQueueReceive(queueCmdeToCore,&cmde,0)){
      switch((char)cmde){
        case cmdeSave:
          save=true;
          break;
          default:
            break;
      }
    }

    if( save ){
      CuvinoCore::saveParams();
    }

    #ifdef DEBUG
      printDataRTOS();
    #endif
  }
  #ifdef DEBUG
  debug.printPGM(DEBUG_Erreur_);
  debug.printPGM(DEBUG_exit);
  debug.print(' ');
  debug.printlnPGM(DEBUG_CuvinoCore);
  #endif
  vTaskDelete(NULL);
}

void cuvinoTimer(void* arg){
  #ifdef DEBUG
  debug.printPGM(DEBUG_CuvinoTimer);
  debug.write(' ');
  debug.printlnPGM(DEBUG_start);
  #endif
  for(;;){
    unsigned long suivant;
    cmdeInterne cmde;
    signed char i;
  // fonction appellé toutes les secondes pour vérifier qu'il n'y a pas de taches programmé dans le timer

    timer.update();
    suivant=timer.next();
    if( suivant > 800 ) suivant=800;

    if( xQueueReceive(queueCmdeToTimer, &cmde, suivant / portTICK_PERIOD_MS) ){
      #ifdef DEBUG
      debugTimer.printPGM(FR::TXT_COMMANDE);
      debugTimer.write(':');
      debugTimer.write((char)cmde);
      #endif
      switch((char)cmde){
        case cmdeActTouteTemp:
          CuvinoGUI::actualiseAffTemp();
          break;
        case cmdeActConsigne:
          for(unsigned char i=0;i<NB_LIGNE_MAX;++i) CuvinoGUI::afficheLigne(i,i,false);
          break;
        case cmdeActEtatTouteEV:
          CuvinoGUI::actualiseEtatEV();
          break;
        case cmdeActEtatEV:
        case cmdeActTemp:
          if( xQueueReceive(queueCmdeToTimer, &i, 10 / portTICK_PERIOD_MS) ){
            if( i>=0 && i < NB_LIGNE_MAX + 1){
              switch ((char)cmde) {
                case cmdeActEtatEV:
                  CuvinoGUI::actualiseAffTemp(i, CuvinoGUI::positionActuelLigne(i));
                  break;
                case cmdeActTemp:
                  CuvinoGUI::actualiseAffTemp(i, CuvinoGUI::positionActuelLigne(i));
                  break;
                default:
                  break;
              }
            }
          }
          break;
        default:
          break;
      }
    }
  }
  #ifdef DEBUG
  debug.printPGM(DEBUG_Erreur_);
  debug.printPGM(DEBUG_exit);
  debug.print(' ');
  debug.printlnPGM(DEBUG_CuvinoTimer);
  #endif
  vTaskDelete(NULL);
}

void setup() {
  wdt_disable(); // désactivation du chien de garde (en cas d'activation par le précédent code)
  #ifdef DEBUG
  //Serial.begin(115200);
  //Serial.begin(250000);
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Debug.setOutput(Serial);
  debug.println("-------------------------------");
  #endif

  listeCuve[0].begin(pileErreur,timer,ds,queueCmdeToCore,queueCmdeToTimer);

  CuvinoCore::init();
  CuvinoGUI::initGUI();
  CuvinoGUI::splashScreen();
  CuvinoCore::reglageDefaut();
  CuvinoCore::loadParams();
  CuvinoCore::initCuves();
  CuvinoProgramme::loadProgramme();


  CuvinoGUI::addLigneMenuConf(POSITION_MENU_APRES, FR::TXT_HC_HP,CuvinoProgramme::menuConfigurationHC_HP);
  CuvinoGUI::addLigneMenuConf(POSITION_MENU_APRES, FR::TXT_ERREURS, CuvinoErreurs::menuErreurs);
  CuvinoGUI::addLigneMenuConf(POSITION_MENU_AVT, FR::TXT_PROGRAMMATEUR,CuvinoProgramme::menuConfigProgrammateur);
  CuvinoGUI::addModifActualiseAffTemp(CuvinoProgramme::modifActualiseAffTemp);




  // attente durant l'écran d'acceuil
  display.aucuneTouche();
  for(;;){
    unsigned char touche = display.getTouche(1);
    #ifdef DEBUG
    debug.printPGM(DEBUG_touche_);
    debug.println(touche,BIN);
    #endif
    if ( touche == (BT_BAS | BT_HAUT)) {
      CuvinoGUI::menuConfiguration();
      break;
    }
    if ( touche == BT_SELECT) break;
    if( (touche&TIMEOUT)!=0) break;
    //if ( millis() - date > 5000) break;
  }

  //timer.every(60000, CuvinoGUI::actualiseAffTemp);

  queueCmdeToCore = xQueueCreate(50,sizeof(char));
  queueCmdeToTimer = xQueueCreate(10,sizeof(char));
  queueCmdeToProgramme = xQueueCreate(10,sizeof(char));
  semaphoreAccesDataCore = xSemaphoreCreateBinary();
  xTaskCreate(&cuvinoCore, (const char*)"CuvinoCore", 800, 0, 1, &taskCore);
  xTaskCreate(&cuvinoGUI, (const char*)"CuvinoGUI", 1200, 0, 1, &taskGUI);
  xTaskCreate(&cuvinoTimer, (const char*)"CuvinoTimer", 600, 0, 1, &taskTimer);
  xTaskCreate(&cuvinoProgramme, (const char*)"CuvinoProgramme", 600, 0, 1, &taskProgramme);

  #ifdef CLAVIER_FREERTOS
  display.startTask(taskGUI,200,configUSE_PORT_OPTIMISED_TASK_SELECTION,&taskKeyboard);
  #else
  display.setUpdateFct(CuvinoCore::sleep);
  #endif

  #ifdef DEBUG
  debug.printPGM(DEBUG_start);
  debug.write(' ');
  debug.printlnPGM(DEBUG_scheduler);
  #endif
  //FreeRTOSActif = true;
  vTaskStartScheduler();
  #ifdef DEBUG
  debug.printPGM(DEBUG_end);
  debug.write(' ');
  debug.printlnPGM(DEBUG_scheduler);
  #endif
}


void loop() {}
