#ifndef __EV3_H__
#define __EV3_H__

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

#include <avr/eeprom.h>
#include <canopenObj.h>

#include <SPI.h>
#include <mcp23s17.h>

#include <broches.h>
#include <BlocMem.h>
#include <Timer.h>
#include <Erreurs.h>
#include <OneWire.h>
#include "lang_fr.h"
#include "const.h"
#include <DebugLogger.h>
#include "EV3_DEBUG.h"
#include "brochage.h"

#include <MCP23S17.h>




#define DEBUG_EV3

#ifndef PORT_MCP23S17_CS
  #define PORT_MCP23S17_CS  PORTG
  #define DDR_MCP23S17_CS   DDRG
  #define BIT_MCP23S17_CS   4
#endif



// mode électrovannes supportés
// code electrovanne:
//    bit 0: etat déterminé sur les fils de commande (CR01 modif)
//    bit 1: etat electrovanne donnée par 2 fils binaire (ouvert ou fermé)
//    bit 2: etat en lecture analogique
//    bit 3: ouverture partielle
//    bit 4: etat sur un fil (uniquement avec cmd sur 1 fil)
//    bit 5: commande sur 2 fils (donc 2 bits)
//    bit 6: maintient de la tension à l'ouverture
//    bit 7: maintien de la tension à la fermeture

#ifndef EV_ETAT_SUR_CMD
  #define EV_ETAT_SUR_CMD         0b00000001
  #define EV_ETAT_2_FILS          0b00000010
  #define EV_ETAT_ANALOGIQUE      0b00000100
  #define EV_OUVERTURE_PARTIELLE  0b00001000
  #define EV_ETAT_1_FILS          0b00010000
  #define EV_CMD_2_FILS           0b00100000
  #define MAINTIENT_OUVETURE      0b01000000
  #define MAINTIENT_FERMETURE     0b10000000

  #define EV_CR     0b00100000
  #define EV_NON_CONFIGURE  0
  #define CR05      0b00100010
  #define CR01modif 0b00100001
  #define CR01      0b00100000
  #define CR05_A    0b00100110
  #define CR01_MO   0b01100000
  #define CR05_MO   0b01100010
  #define CR01_MF   0b10100000
  #define CR05_MF   0b10100010
  #define CR01_M    0b11100000
  #define CR05_M    0b11100010
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

#define POSITION_ERREUR   255

#define OUVERT  10
#define FERME   -10

#ifndef ERREUR_EV_INCONNUE
  // temps de mouvement de l'electrovanne beaucoup trop long
  #define ERREUR_EV_TEMPS_TROP_LONG         10

  #define ERREUR_EV_OK                      0

  #define ERREUR_EV_INCONNUE                8
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
  // electrovanne non supportée
  #define ERREUR_TYPE_EV_NON_SUPPORTEE      9

  #define ERREUR_EV_NUMERO_INCOMPATIBLE     11

  #define ERREUR_EV_NUMERO_NEGATIF_INCOMPATIBLE_MODE_CMD_2_FILS 12
  #define ERREUR_EV_DEJA_CONFIGURE_A_CE_NUMERO  13
#endif

typedef uint8_t erreurEV;


unsigned char EVinListe(unsigned char num); // renvoi le mode de l'electrovanne à la postion num de la liste (fictive)
const char* TXT_EV(unsigned char num);  /// renvoi le texte affecté à un mode d'EV
unsigned char sizeListeEV();  // renvoi la taille de la liste d'EV
bool EV_OK(unsigned char num);  // return true si le code de l'EV est bien existant, false sinon

//const co_descr_obj* CO_OBJ_EV=[{1}];

#define EV3_TYPE_MASQ_TOUT_PIN        0
#define EV3_TYPE_MASQ_PIN_OUVERTURE   OUVERT
#define EV3_TYPE_MASQ_PIN_FERMETURE   FERME

class ElectroVanne3 {
  public:
    ElectroVanne3(int8_t numero=127, uint8_t mode=EV_NON_CONFIGURE);
    ~ElectroVanne3();

    /* initialisation "module" */
    static void begin(PileErreur& _pileErreur,Timer& _timer, QueueHandle_t queueCmdeToCore,QueueHandle_t queueCmdeToTimer, MCP23S17* gpio);

    /* gestion "module" */

    static ElectroVanne3* getEVAt(int8_t numero);
    static bool setEVAt(ElectroVanne3* EV);
    static void deleteEVAt(ElectroVanne3* EV);
    static bool emplacementEVlibre(int8_t numero, bool cmde_2fils);

    static void initBroches(int8_t numero, uint8_t mode);
    static void relacheBroches(int8_t numero, uint8_t mode);
    static uint8_t getMasqCmde(int8_t numero, uint8_t mode, int8_t type=EV3_TYPE_MASQ_TOUT_PIN);
    static uint16_t getMasqEtat(int8_t numero, uint8_t mode);

    static int8_t etatEV(int8_t numero, uint8_t mode);
    static void repos(int8_t numero, uint8_t mode);
    static bool demarreMvt(int8_t numero, uint8_t mode, int8_t mvt);
    static int16_t testEV(int8_t numero, uint8_t mode);


    /* initialisation */
    void initEV();
    void defaut();
    bool estConfigure() const;
    bool OK() const;

    /* paramétrage */
    erreurEV setNumero(int8_t numero, bool force=false);
    erreurEV setMode(uint8_t mode);
    erreurEV setTempsOuverture(uint16_t tps);
    
    /* mouvements */
    int8_t etatEV();

    bool bougeEV(int8_t mvt);
    //bool bougeEVpartiel(int8_t mvt);
    bool bougeEVtotal(int8_t mvt);
    bool ferme();
    bool ouvre();
    void repos();

    uint16_t getTempsOuverture();
    int8_t getPosition();
    uint8_t getNumero();
    uint8_t getMode();
    int8_t getMvt_pos_fin();
    int8_t getMvt_pos_deb();

    /* autre */
    uint8_t testEV();


    /* CANOPEN */
    static size_t getSizeData(subindex_t subindex);
    void* getAddrData(subindex_t subindex);
    co_erreur_t getData(subindex_t subindex, void* data);
    co_erreur_t setData(subindex_t subindex, const void* data);

    static co_erreur_t getDataEEPROM(subindex_t subindex, uint16_t addrEV, void* data);
    static co_erreur_t setDataEEPROM(subindex_t subindex, uint16_t addrEV, const void* data);

    static co_erreur_t getDataEEPROM(subindex_t subindex, BlocMem* bloc, void* data);
    static co_erreur_t setDataEEPROM(subindex_t subindex, BlocMem* bloc, const void* data);

    static const co_descr_obj* getDescrObj_PGM();

    /* sauvegarde */
    uint16_t save(uint16_t addr);
    uint16_t load(uint16_t addr);
    bool save(BlocMem* bloc);
    bool load(BlocMem* bloc);


  
    uint8_t brocheEV;
    uint8_t brocheEtatEV;


    void print(HardwareSerial& serial);
    void print(DebugLogger& serial);

    

    

    

    uint8_t getNumModuleErreur() const {
      return numModuleErr;
    }

    ElectroVanne3 &operator=(const ElectroVanne3 &source);

    //static void finMvt(void* arg);
    static void finMvtTotal(void* arg);
  protected:
    /* paramétrage */
    void initBroches();
    void relacheBroches();

    /* variables de paramétrage */
    int8_t numero;
    uint8_t mode;
    uint16_t tempsOuvertureVanne;


    /* variable de fonctionnement */
    int8_t position;

    uint32_t mvt_arduidate;
    int8_t mvt_pos_deb;
    int8_t mvt_pos_fin;
    uint8_t mvt_timerID;

    int8_t mvt_futur;

    uint16_t mvt_date;

    /* variables static */
    static uint8_t numModuleErr;
    static PileErreur* pileErreur;
    static Timer* timer;
    static QueueHandle_t qCmdeToCore;
    static QueueHandle_t qCmdeToTimer;

    static ElectroVanne3* listeEV[16];
    static inline uint8_t getIndex(int8_t num);

    public:
      static MCP23S17* gpio;
};

#endif
