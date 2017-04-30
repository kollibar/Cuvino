#ifndef __CUVINO_GUI_H__
#define __CUVINO_GUI_H__

#include <Arduino.h>
#include "CuveV2.h"
#include "lang_fr.h"
#include <Geometrie.h>


#define AFF_TEMP  0b00000001
#define AFF_HEURE 0b00000010

#define NB_MAX_GUI 1
#define NB_MAX_FCT_MENU 4
#define NB_MAX_FCT_ACT_AFF_TEMP 2

#define POSITION_MENU_AVT 1
#define POSITION_MENU_APRES 2
#define POSITION_MENU_NON_CONF -1

typedef bool(*FonctionMenu)(void);
typedef void(*FonctionModifActualiseAffTemp)(void);

struct LigneMenu{
  FonctionMenu fctMenu;
  signed char pos;
  const char* nomPGM;
};

namespace CuvinoGUI{

  void initGUI(void);

  void initAffStandard(void);

  void initEcran(); // initialisation de l'écran

  void actEcran();

  void splashScreen();

  Point8_t CoordonneesLigne(signed int pos);
  signed char positionActuelLigne(signed int i);
  void afficheLigne(signed int i, signed int pos, bool selectConsigne = false);

  void actualiseAffTemp(); // actualise l'affichage des temperatures à l'écran
  void actualiseAffTemp(signed int i, signed int pos); // actualise l'affichage des temperatures de la ligne i à la position pos
  void actualiseEtatEV(); // actualise l'etat de toutes les electrovannes
  void actualiseEtatEV(signed int i, signed int pos); // actualise l'etat de l'electrovanne i à la position pos

  bool addLigneMenuConf(uint8_t pos,const char* nomPGM,FonctionMenu fct); // ajoute une ligne dans le menu de configuration
  bool addModifActualiseAffTemp(FonctionModifActualiseAffTemp fct);

  bool menuConfiguration(void);
  bool arretEV(void);

  bool menuConfigurationHeureV2(void);

  bool menuConfigCuve(CuveV2& cuve);

  void _creeLigneMenuConfiguration(uint8_t i, char* chaine, uint8_t taille, void* arg);
  void _actualiseTemp_menuConfigCuve(void* arg);
  void _creeLigneMenuConfigCuve(uint8_t i, char* chaine, uint8_t taille, void* arg);
  void _creeLigneMenuArretEV(uint8_t i, char* chaine, uint8_t taille, void* arg);
}



struct structDataMenuConfigCuve {
  signed int temperature;
  CuveV2* cuve;
  signed char y;
  signed char timerID;
};

#endif
