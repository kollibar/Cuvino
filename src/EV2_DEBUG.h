#ifndef __EV2_DEBUG_H__
#define __EV2_DEBUG_H__

#include <avr/pgmspace.h>

const char PROGMEM EV2_DEBUG_nom[]="EV2";
const char PROGMEM EV2_DEBUG_TEST_EV[]="Test de l'electrovanne ";
const char PROGMEM EV2_DEBUG_ET_CAPTEUR[]=" & capteur";
const char PROGMEM EV2_DEBUG_ERR_AUCUN_MVT[]="Erreur lors de l'ouverture partielle de l'electrovanne. Aucun changement n'est observe. Electrovanne bloquee ou mal branchee ou capteur mal branche";
const char PROGMEM EV2_DEBUG_ERR_EV_CAPT_INV[]="Fermeture lors de l'ouverture de l'electrovanne. Electrovanne ou capteur inversés";
const char PROGMEM EV2_DEBUG_OUV_PART_OK[]="ouverture partielle de l'electrovanne OK";
const char PROGMEM EV2_DEBUG_FERME_EV_OK[]="fermeture de l'electrovanne OK";
const char PROGMEM EV2_DEBUG_ERR_EV_OUVRE_PAS[]="L’electrovanne est encore fermé alors qu’une ouverture a été demandé. Electrovanne bloquée ou inversée.";
const char PROGMEM EV2_DEBUG_ERR_EV_OUVERTURE_BLOQUE_MILIEU[]="Lors de l'ouverture, l'electrovanne semble s'être arrété en cours. Soit l'electroanne est lente (>10s), soit elle est bloqué, soit le capteur de l'état ouvert a un defaut.";
const char PROGMEM EV2_DEBUG_ERR_EV_FERME_PAS[]="L’electrovanne est encore ouverte alors qu’une fermeture a été demandée. Electrovanne bloquée ou inversée.";
const char PROGMEM EV2_DEBUG_ERR_EV_FERMETURE_BLOQUE_MILIEU[]="Lors de la fermeture, l'electrovanne semble s'être arrété en cours. Soit l'electroanne est lente (>10s), soit elle est bloqué, soit le capteur de l'état ouvert a un defaut.";
const char PROGMEM EV2_DEBUG_TMP_OUV_BCP_TROP_LONG[]="temps de mouvement de l'electrovanne beaucoup trop long ! ERREUR !";
const char PROGMEM EV2_DEBUG_finMvtTotal[]="#finMvtTotal(";
const char PROGMEM EV2_DEBUG_bougEVtotal[]="#bougeEVtotal(";
const char PROGMEM EV2_DEBUG_digitalWrite[]="digitalWrite(";
const char PROGMEM EV2_DEBUG_digitalRead[]="digitalRead(";
const char PROGMEM EV2_DEBUG_repos[]="#repos(";
const char PROGMEM EV2_DEBUG_bougeEV[]="#bougeEV(";
const char PROGMEM EV2_DEBUG_etatEV[]="#etatEV()";
const char PROGMEM EV2_DEBUG_etat2fils[]="etat sur 2 fils";
const char PROGMEM EV2_DEBUG_FERMETURE[]="fermeture";
const char PROGMEM EV2_DEBUG_OUVERTURE[]="ouverture";
const char PROGMEM EV2_DEBUG_HIGH[]="HIGH";
const char PROGMEM EV2_DEBUG_LOW[]="LOW";
const char PROGMEM EV2_DEBUG_FLECHE[]="=>";
const char PROGMEM EV2_DEBUG_FERME[]="FERME";
const char PROGMEM EV2_DEBUG_OUVERT[]="OUVERT";
const char PROGMEM EV2_DEBUG_AUTRE[]="AUTRE";
const char PROGMEM EV2_DEBUG_PARTIEL[]="PARTIEL";
const char PROGMEM EV2_DEBUG_PLUS1[]=" + 1";


const char PROGMEM EV2_DEBUG_MS[]="ms";
const char PROGMEM EV2_DEBUG_tempsOuvertureVanne[]="temps de fermeture EV: ";
const char PROGMEM EV2_DEBUG_electrovanne[]="Electrovanne";
const char PROGMEM EV2_DEBUG_OK[]="ok";




#endif
