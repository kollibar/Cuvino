
#ifndef __LANG_FR_H__
#define __LANG_FR_H__


#include <avr/io.h>
#include <avr/pgmspace.h>


namespace FR{
  const char PROGMEM TXT_ERR_COMM_SONDE[] = "Erreur com.   avec sondes detemperature.";
  const char PROGMEM TXT_DEFAUT_BRANCH_SONDE[] = "Defaut sonde  ou branchement";
  const char PROGMEM TXT_CONFIGURATION[] = "configuration";
  const char PROGMEM TXT_CUVE[] = "cuve";
  const char PROGMEM TXT_NO_CONF[] = "No conf";
  const char PROGMEM TXT_CONF_OK[] = "Conf OK";
  const char PROGMEM TXT_ERR_AUCUNE_CUVE_CONF[] = "Erreur:       Aucune cuve   configuree";
  const char PROGMEM TXT_STOP[] = "stop";
  const char PROGMEM TXT_NON[] = "non";
  const char PROGMEM TXT_NOM[] = "nom";
  const char PROGMEM TXT_CONF[] = "conf";
  const char PROGMEM TXT_SONDE[] = "sonde";
  const char PROGMEM TXT_EXTERNE[] = "externe";
  const char PROGMEM TXT_CONFIGURE[] = "configure";
  const char PROGMEM TXT_ACTUELLE[] = "actuelle";
  const char PROGMEM TXT_DU_LOCAL[] = "du local";
  const char PROGMEM TXT_AUCUNE[] = "aucune";
  const char PROGMEM TXT_MODIFIER[] = "modifier";
  const char PROGMEM TXT_RETOUR[] = "retour";
  const char PROGMEM TXT_ANNULER[] = "annuler";
  const char PROGMEM TXT_RESET[] = "reset";
  const char PROGMEM TXT_ARRET[] = "arret";
  const char PROGMEM TXT_VANNE[] = "vanne";
  const char PROGMEM TXT_CALIBRAGE[] = "calibrage";
  const char PROGMEM TXT_GESTION[] = "gestion";

  const char PROGMEM TXT_OK[]="ok";
  const char PROGMEM TXT_SELECT[]="select";
  const char PROGMEM TXT_CHAUD[]="chaud";
  const char PROGMEM TXT_FROID[]="froid";
  const char PROGMEM TXT_SELECTION_MODE[]="selection mode";

  const char PROGMEM TXT_EV[]="EV";
  const char PROGMEM TXT_MODE[]="mode";
  const char PROGMEM TXT_ETAT[]="etat";
  const char PROGMEM TXT_FERME[]="fermee";
  const char PROGMEM TXT_OUVERT[]="ouvert";
  const char PROGMEM TXT_PARTIEL[]="partiel";
  const char PROGMEM TXT_OUVRIR[]="ouvrir";
  const char PROGMEM TXT_FERMER[]="fermer";


  const char PROGMEM TXT_CR05[]="CR-05-N";
  const char PROGMEM TXT_CR05_A[]="CR-05-A";
  const char PROGMEM TXT_CR01[]="CR-01";
  const char PROGMEM TXT_CR01_MO[]="CR-01 M. Ouv.";
  const char PROGMEM TXT_CR01_MF[]="CR-01 M. Fer.";
  const char PROGMEM TXT_CR05_MO[]="CR-05 M. Ouv.";
  const char PROGMEM TXT_CR05_MF[]="CR-05 M. Fer.";
  const char PROGMEM TXT_CR01_M[]="CR-01 Maint.";
  const char PROGMEM TXT_CR05_M[]="CR-05 Maint.";
  const char PROGMEM TXT_CR01modif[]="CR-01 modifiee";
  const char PROGMEM TXT_CR07[]="CR-07";
  const char PROGMEM TXT_EV_RELAI[]="Relai unique";
  const char PROGMEM TXT_NON_CONFIGURE[]="non configure";

  const char PROGMEM TXT_SELECT_SONDE[]="Selct Sonde To";

  const char PROGMEM TXT_N_CF[]="n-cf";
  const char PROGMEM TXT_TIME_INVALIDE[]="--:--";
  const char PROGMEM TXT_REGLAGE[]="reglage";
  const char PROGMEM TXT_HEURE[]="heure";
  const char PROGMEM TXT_JOUR[]="jour";
  const char PROGMEM TXT_MOIS[]="mois";
  const char PROGMEM TXT_SEMAINE[]="semaine";
  const char PROGMEM TXT_MINUTE[]="minute";
  const char PROGMEM TXT_SECONDE[]="seconde";
  const char PROGMEM TXT_HC_HP[]="HC/HP";
  const char PROGMEM TXT_NOW[]="now";

  const char PROGMEM JOURS[][4]={"Dim","Lun","Mar","Mer","Jeu","Ven","Sam"};

  const char PROGMEM TXT_PLAGE[]="plage";
  const char PROGMEM TXT_NB_DE_PLAGE[]="nb de plage";
  const char PROGMEM TXT_INTERNE[]="interne";

  const char PROGMEM TXT_ERR[]="err";
  const char PROGMEM TXT_ERR_COM[]="ErrC";
  const char PROGMEM TXT_REINIT_MEM[]="reinit. mem.";
  const char PROGMEM TXT_REINITIALISER[]="reinitialiser";
  const char PROGMEM TXT_LA_MEMOIRE[]="la memoire";

  const char PROGMEM TXT_OUI[]="oui";

  const char PROGMEM TXT_DONNEES_INCORRECTES[]="donnees incorrectes";

  const char PROGMEM TXT_PROGRAMMATEUR[]="programmateur";
  const char PROGMEM TXT_CHOIX_[]="choix ";
  const char PROGMEM TXT_ECHOUE[]="echoue";
  const char PROGMEM TXT_AJOUTER_PROGRAMME[]="ajouter programme";
  const char PROGMEM TXT_NOUVEAU[]="nouveau";
  const char PROGMEM TXT_DATE[]="date";

  const char PROGMEM TXT_PASSAGE_HC_HP[]="passage HC>HP";
  const char PROGMEM TXT_PASSAGE_HP_HC[]="passage HP>HC";

  const char PROGMEM TXT_REPETITION[]="repetition";
  const char PROGMEM TXT_LE[]="le";
  const char PROGMEM TXT_TEMP[]="temp";


  const char PROGMEM TXT_ELECTROVANNE[]="electrovanne";
  const char PROGMEM TXT_TEST_DE_L[]="test de l'";
  const char PROGMEM TXT_DEFAUT[]="defaut";
  const char PROGMEM TXT_OU[]="ou";
  const char PROGMEM TXT_CAPTEUR[]="capteur";
  const char PROGMEM TXT_ERREUR[]="erreur";
  const char PROGMEM TXT_ERREURS[]="erreurs";
  const char PROGMEM TXT_PEUT_DURER_30S[]="peut durer 30s";
  const char PROGMEM TXT_EN_COURS[]="en cours";
  const char PROGMEM TXT_LISTE[]="liste";
  const char PROGMEM TXT_SUPPRIMER[]="supprimer";
  const char PROGMEM TXT_TOUTES_LES[]="toutes les";

  const char PROGMEM TXT_CORE[]="core";
  const char PROGMEM TXT_NB[]="nb";
  const char PROGMEM TXT_LOAD[]="load";
  const char PROGMEM TXT_FLECHE[]=" => ";
  const char PROGMEM TXT_NOUVELLE[]="nouvelle";


  const char PROGMEM TXT_MESUREE[]="mesuree";
  const char PROGMEM TXT_MESURE[]="mesure";
  const char PROGMEM TXT_REELLE[]="reelle";
  const char PROGMEM TXT_CORRIGEE[]="corrigee";
  const char PROGMEM TXT_FAIRE_SECONDE[]="faire seconde";

  const char PROGMEM TXT_ERREUR_EV[][9]={"Inconnue","AucunMvt","Inversio","Ouvre Pa","OBloqMil","Ferme Pa","FBloqMil","Type EV "};

  const char PROGMEM TXT_ETES_VOUS_SUR[]="etes vous sur";
  const char PROGMEM TXT_DE_FAIRE[]="de faire";
  const char PROGMEM TXT_VALIDER[]="valider";

  const char PROGMEM TXT_TEMPERATURE[]="temperature";
  const char PROGMEM TXT_OBJECTIF[]="objectif";
  const char PROGMEM TXT_POSITION_ACTUELLE[]="position actuelle";

  const char PROGMEM TXT_FERMETURE[]="fermeture";
  const char PROGMEM TXT_OUVERTURE[]="ouverture";
  const char PROGMEM TXT_SUPPRESSION[]="suppression";
  const char PROGMEM TXT_AJOUTE[]="ajoute";
  const char PROGMEM TXT_ACTUALISE[]="actualise";
  const char PROGMEM TXT_ENREGISTREMENT[]="enregistrement";
  const char PROGMEM TXT_PROGRAMME[]="programme";
  const char PROGMEM TXT_PROCHAIN[]="prochain";
  const char PROGMEM TXT_APPEL[]="appel";
  const char PROGMEM TXT_COMMANDE[]="commande";



}

#endif
