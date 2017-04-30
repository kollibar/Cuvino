#ifndef __EV_H__
#define __EV_H__

#include <Arduino.h>
#include <broches.h>
#include <BlocMem.h>
#include <Timer.h>
#include <Erreurs.h>
#include <OneWire.h>
#include "lang_fr.h"
#include "brochage.h"

// mode électrovannes supportés
// code electrovanne:
//    bit 0: etat déterminé sur les fils de commande (CR01 modif)
//    bit 1: etat electrovanne donnée par 2 fils binaire (ouvert ou fermé)
//    bit 2: etat en lecture analogique
//    bit 3:
//    bit 4:
//    bit 5: commande sur 2 fils (donc 2 bits)
//    bit 6: maintient de la tension à l'ouverture
//    bit 7: maintien de la tension à la fermeture

#define EV_ETAT_SUR_CMD       0b1
#define EV_ETAT_2_FILS        0b10
#define EV_ANALOGIQUE         0b100
#define EV_CMD_2_FILS         0b100000
#define MAINTIENT_OUVETURE    0b1000000
#define MAINTIENT_FERMETURE   0b10000000

#define EV_CR     0b100000
#define EV_NON_CONFIGURE  0
#define CR05      0b100010
#define CR01modif 0b100001
#define CR01      0b100000
#define CR05_A    0b100110
#define CR01_MO   0b1100000
#define EV_RELAI  0b10000000

#define EV_SONDE_LOCAL  255
#define EV_MODE_UNIQUE  CR05

#define OUVERT  10
#define FERME   -10
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

/*
unsigned char EVinListe(unsigned char num);
const char* TXT_EV(unsigned char num);
unsigned char sizeListeEV();
*/

class ElectroVanne {
  public:
    uint8_t numero;
    int8_t mode;

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
    bool loadPort();

    void print(HardwareSerial& serial);

    void initEV();
    void initBroches();
    void initBrochesCR05();
    void initBrochesCR05_A();
    void initBrochesCR01();
    void initBrochesCR01modif();


    int8_t etatEV();
    int8_t etatEVCR05_A();
    int8_t etatEVCR05();
    int8_t etatEVCR01modif();

    uint8_t testEV();
    uint8_t testEVCR01();
    uint8_t testEVCR05_CR01modif();

    bool bougeEV(int8_t pos);

    void begin(PileErreur& _pileErreur,Timer& _timer,OneWire& _ds);

    uint8_t getNumModuleErreur() const {
      return numModuleErr;
    }

    ElectroVanne &operator=(const ElectroVanne &source);

    static void finMvt(void* arg);
  protected:
    static uint8_t numModuleErr;
    static PileErreur* pileErreur;
    static Timer* timer;
    static OneWire* ds;
};

#endif
