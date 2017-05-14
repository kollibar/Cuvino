#ifndef __EV2_H__
#define __EV2_H__

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

#include <Arduino.h>
#include <broches.h>
#include <BlocMem.h>
#include <Timer.h>
#include <Erreurs.h>
#include <OneWire.h>
#include "lang_fr.h"
#include "const.h"
#include <DebugLogger.h>
#include "EV2_DEBUG.h"
#include "brochage.h"

#define DEBUG



// mode électrovannes supportés
// code electrovanne:
//    bit 0: etat déterminé sur les fils de commande (CR01 modif)
//    bit 1: etat electrovanne donnée par 2 fils binaire (ouvert ou fermé)
//    bit 2: etat en lecture analogique
//    bit 3: ouverture partielle
//    bit 4:
//    bit 5: commande sur 2 fils (donc 2 bits)
//    bit 6: maintient de la tension à l'ouverture
//    bit 7: maintien de la tension à la fermeture

#ifndef EV_ETAT_SUR_CMD
  #define EV_ETAT_SUR_CMD         0b1
  #define EV_ETAT_2_FILS          0b10
  #define EV_ETAT_ANALOGIQUE      0b100
  #define EV_OUVERTURE_PARTIELLE  0b1000
  #define EV_CMD_2_FILS           0b100000
  #define MAINTIENT_OUVETURE      0b1000000
  #define MAINTIENT_FERMETURE     0b10000000

  #define EV_CR     0b0100000
  #define EV_NON_CONFIGURE  0
  #define CR05      0b0100010
  #define CR01modif 0b0100001
  #define CR01      0b0100000
  #define CR05_A    0b0100110
  #define CR01_MO   0b1100000
  #define EV_RELAI_OUVRANT  0b01000000
  #define EV_RELAI_FERMANT  0b10000000

  #define EV_ERREUR       254
  #define EV_SONDE_LOCAL  255
  #define EV_MODE_UNIQUE  CR05
#endif

#ifdef OUVERT
  #undef OUVERT
  #undef FERME
#endif

#define OUVERT  10
#define FERME   -10

#ifndef ERREUR_EV_INCONNUE
  // temps de mouvement de l'electrovanne beaucoup trop long
  #define ERREUR_EV_TEMPS_TROP_LONG 10

  #define ERREUR_EV_INCONNUE                0
  // Erreur lors de l'ouverture partielle de l'electrovanne. Aucun changement n'est observé. Electrovanne bloquée ou mal branchée ou capteur mal branché
  #define ERREUR_EV_AUCUN_MVT_OBS           1
  // l'état de l'electrovanne s'est fermée alors qu'on a demandé une ouverture... inversion soit des fils de test, soit des branchement electrovanne
  #define ERREUR_EV_EV_OU_CAPTEUR_INVERSE   2
  // l'état de l'electrovanne est encore à FERME --> l'electrovanne est bloqué ou mal branchée
  #define ERREUR_EV_OUVRE_PAS               3
  // l'état de l'electrovanne est entre ouvert et ferme. soit capteur ouvert mal branché, soit electrovanne lente
  #define ERREUR_EV_OUVERTURE_BLOQUE_MILIEU 4
  // l'état de l'electrovanne est encore à FERME --> l'electrovanne est bloqué ou mal branchée
  #define ERREUR_EV_FERME_PAS               5
  // l'état de l'electrovanne est entre ouvert et ferme. soit capteur ouvert mal branché, soit electrovanne lente
  #define ERREUR_EV_FERMETURE_BLOQUE_MILIEU 6
  // electrovanne de type inconnu (pas dans la liste des type supportés)
  #define ERREUR_TYPE_EV_INCONNU            7
#endif


unsigned char EVinListe(unsigned char num); // renvoi le mode de l'electrovanne à la postion num de la liste (fictive)
const char* TXT_EV(unsigned char num);  /// renvoi le teste affecté à un mode d'EV
unsigned char sizeListeEV();  // renvoi la taille de la liste d'EV
bool EV_OK(unsigned char num);  // return true si le code de l'EV est bien existant, false sinon


class ElectroVanne2 {
  public:
    uint8_t numero;
    int8_t mode;

    uint8_t brocheEV;
    uint8_t brocheEtatEV;

    int8_t position;
    uint16_t tempsOuvertureVanne;

    uint32_t mvt_date;
    int8_t mvt_pos_deb;
    int8_t mvt_pos_fin;
    uint8_t mvt_timerID;

    int8_t mvt_futur;

    uint16_t save(uint16_t addr);
    uint16_t load(uint16_t addr);
    bool save(BlocMem* bloc);
    bool load(BlocMem* bloc);
    void defaut(void);
    bool estConfigure() const;

    bool OK() const;

    bool loadPort();

    void print(HardwareSerial& serial);
    void print(DebugLogger& serial);

    void initEV();
    void initBroches();

    int8_t etatEV();
    //int8_t etatEVCR01modif();

    uint8_t testEV();

    bool bougeEV(int8_t mvt);
    //bool bougeEVpartiel(int8_t mvt);
    bool bougeEVtotal(int8_t mvt);
    bool ferme();
    bool ouvre();
    void repos();

    void begin(PileErreur& _pileErreur,Timer& _timer,OneWire& _ds,QueueHandle_t queueCmdeToCore,QueueHandle_t queueCmdeToTimer);

    uint8_t getNumModuleErreur() const {
      return numModuleErr;
    }

    ElectroVanne2 &operator=(const ElectroVanne2 &source);

    //static void finMvt(void* arg);
    static void finMvtTotal(void* arg);
  protected:
    static uint8_t numModuleErr;
    static PileErreur* pileErreur;
    static Timer* timer;
    static OneWire* ds;
    static QueueHandle_t qCmdeToCore;
    static QueueHandle_t qCmdeToTimer;
};

#endif
