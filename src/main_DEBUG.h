#ifndef __MAIN_DEBUG_H__
#define __MAIN_DEBUG_H__

#include <avr/pgmspace.h>

extern const char PROGMEM DEBUG_main[]="main" ;
extern const char PROGMEM DEBUG_GUI[]="GUI" ;
extern const char PROGMEM DEBUG_Timer[]="Timer" ;
extern const char PROGMEM DEBUG_Prog[]="Prog" ;
extern const char PROGMEM DEBUG_Core[]="Core" ;

extern const char PROGMEM DEBUG_ligneTrait[]= "----------------";
extern const char PROGMEM DEBUG_initBroches[] = "initBroches";
extern const char PROGMEM DEBUG_demandeMesureTemperature[]="demandeMesureTemperature()";
extern const char PROGMEM DEBUG_controlTemp[]="controlTemp";
extern const char PROGMEM DEBUG_initCuves[]="initCuves";
extern const char PROGMEM DEBUG_rtc_perd_temps[]="RTC lost confidence in the DateTime!";
extern const char PROGMEM DEBUG_rtc_non_actif[]="RTC was not actively running, starting now";
extern const char PROGMEM DEBUG_saveParams[]="saveParams()";
extern const char PROGMEM DEBUG_loadParams[]="loadParams()";
extern const char PROGMEM DEBUG_blocPrincipal[]=" ----------- Bloc principal -------------";
extern const char PROGMEM DEBUG_blocCuve_Reinit[]=" ----------- Bloc cuve - REINIT -------------";
extern const char PROGMEM DEBUG_blocCuve[]=" ----------- Bloc cuve -------------";
extern const char PROGMEM DEBUG_CRC16_CALCULE[]="CRC16 calculée:";
extern const char PROGMEM DEBUG_CRC16_LUE[]="CRC16 lue:";
extern const char PROGMEM DEBUG_adresseBloc[]="adresse bloc:";
extern const char PROGMEM DEBUG_tailleBloc[]="taille bloc:";
extern const char PROGMEM DEBUG_modeBloc[]="mode bloc:";
extern const char PROGMEM DEBUG_versionBloc[]="version bloc:";
extern const char PROGMEM DEBUG_versionMemNonSupporte[]="Version de mémoire non supportée (";
extern const char PROGMEM DEBUG_chargementImpossible[]="Chargement impossible";
extern const char PROGMEM DEBUG_initGUI[]="initGUI";
extern const char PROGMEM DEBUG_initEcran[]="initEcran";
extern const char PROGMEM DEBUG_splashScreen[]="splashScreen";
extern const char PROGMEM DEBUG_initAFFStandard[]="initAffStandard";
extern const char PROGMEM DEBUG_afficheLigne[]="afficheLigne(";
extern const char PROGMEM DEBUG_menuConfiguration[]="menuConfiguration()";
extern const char PROGMEM DEBUG_ligneAvt[]="ligne avt:";
extern const char PROGMEM DEBUG_ligneApres[]="ligne apres:";
extern const char PROGMEM DEBUG_sondeConfigure[]="sonde configuree!";
extern const char PROGMEM DEBUG_erreurRTC_horloge_non_valide[]="Erreur du module RTC. Heure non valide! Démarrage programme impossible";
extern const char PROGMEM DEBUG_CuvinoProgramme[]="CuvinoProgramme::";
extern const char PROGMEM DEBUG_verifProchain[]="verifProchain()";
extern const char PROGMEM DEBUG_programmeInterval[]="programmeInterval(";
extern const char PROGMEM DEBUG_regleProgramme[]="regleProgramme()";
extern const char PROGMEM DEBUG_now[]="now:";
extern const char PROGMEM DEBUG_dans[]="dans";
extern const char PROGMEM DEBUG_secondes[]="secondes";
extern const char PROGMEM DEBUG_prochain[]="prochain:";
extern const char PROGMEM DEBUG_programme[]="programme:";
extern const char PROGMEM DEBUG_datePro[]="date pro:";
extern const char PROGMEM DEBUG_date[]="date:";
extern const char PROGMEM DEBUG_exit[]="exit";
extern const char PROGMEM DEBUG_start[]="start";
extern const char PROGMEM DEBUG_end[]="end";
extern const char PROGMEM DEBUG_scheduler[]="scheduler";
extern const char PROGMEM DEBUG_touche_[]="touche => ";
extern const char PROGMEM DEBUG_CuvinoTimer[]="CuvinoTimer";
extern const char PROGMEM DEBUG_CuvinoCore[]="CuvinoCore";
extern const char PROGMEM DEBUG_CuvinoGUI[]="CuvinoGUI";
extern const char PROGMEM DEBUG_Erreur_[]="Erreur: ";
extern const char PROGMEM DEBUG_freeStackSize[]="Free stack size: ";
extern const char PROGMEM DEBUG_stackOverFlow[]="Stack overflow!";
extern const char PROGMEM DEBUG_taskAffected[]="Task affected: ";
extern const char PROGMEM DEBUG_save[]="save";
extern const char PROGMEM DEBUG_cuve[]="cuve";
extern const char PROGMEM DEBUG_erreur_lecture_bloc[]="Erreur lors de la lecture du bloc";
extern const char PROGMEM DEBUG_principal[]="principal";
extern const char PROGMEM DEBUG_versionBlocNonSupporte[]="version de bloc non supporté";
extern const char PROGMEM DEBUG_blocIlisible_crc[]="bloc ilisible (CRC éronée)";
extern const char PROGMEM DEBUG_actualisationDateProProg[]="actualisation date prochain programme:";
extern const char PROGMEM DEBUG_delai_ecoule[]="délai écoulé!";
extern const char PROGMEM DEBUG_cmdeRecu[]="commande recu:";
extern const char PROGMEM DEBUG_s_var_delai[]="s (var delai=";
extern const char PROGMEM DEBUG_actualiseEtatEV[]="actualiseEtatEV(";
extern const char PROGMEM DEBUG_actualiseAffTemp[]="actualiseAffTemp(";
extern const char PROGMEM DEBUG_etatAffichage[]="etatAffichage(";




#endif
