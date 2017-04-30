#ifndef __CUVINO_WIDGETS_H__
#define __CUVINO_WIDGETS_H__

#include <Arduino.h>
#include <Widgets.h>
#include <IO_PCD8544_5T.h>
#include <Adafruit_GFX_v1.0.2_NB8bits.h>
#include "Sonde.h"
#include "SondeV2.h"
#include "Cuve.h"
#include "CuveV2.h"
#include "EV2.h"
#include <RtcDateTime.h>
#include "const.h"
#include "lang_fr.h"
#include "Programme.h"
#include <OneWire.h>
#include "brochage.h"

#define DEBUG

const char PROGMEM CHAINE_INIT_MENU_PROGRMMATEUR[] = "Tl   ";
const char PROGMEM CHAINE_MENU_PROGR_TL[] = "Toutl";

// -- POLICES & CHARACTERES -- //
const unsigned char PROGMEM BMPglyph_degC[] = {B01000000, B10100000, B01000000, B00011000, B00100000, B00100000, B00011000};

template<typename T> class CuvinoWidgets;

template<typename T>
class CuvinoWidgets:public Widgets<T>{
public:
  CuvinoWidgets(T& display);
  void printTemp(const int16_t& temp,uint8_t x,uint8_t y, bool forceFrac = false);
  void affLigneSelectSonde(uint8_t y, Sonde sonde, int16_t temp);
  bool menuSelectSondeV2(Cuve& cuve,const Cuve listeCuve[],const unsigned char tailleListeCuve,OneWire& ds);
  bool menuSelectSondeForV2(CuveV2& cuve,const CuveV2 listeCuve[],const unsigned char tailleListeCuve);
  bool menuSelectSondeV2(Cuve& cuve,OneWire& ds);

  Sonde* menuSelectSondeV3(const Sonde listeSondeExistante[],const unsigned char tailleListeSonde,OneWire& ds,signed char act=-1,bool interne=false,bool aucune=true);

  retourSelect menuListeChoixEV(ElectroVanne2& EV, uint8_t yMin = 0, uint8_t yMax = LCDHEIGHT, uint8_t xMin = 0, uint8_t xMax = LCDWIDTH) ;
  retourSelect menuListeChoixModeEV(ElectroVanne2& EV, uint8_t yMin = 0, uint8_t yMax = LCDHEIGHT, uint8_t xMin = 0, uint8_t xMax = LCDWIDTH) ;

  bool menuConfigEV(ElectroVanne2& EV,uint8_t yMin = 8, uint8_t yMax = LCDHEIGHT, uint8_t xMin = 0, uint8_t xMax = LCDWIDTH);
  bool menuManipuleEV(ElectroVanne2& EV,uint8_t yMin = 8, uint8_t yMax = LCDHEIGHT, uint8_t xMin = 0, uint8_t xMax = LCDWIDTH);
  bool menuConfigSondeLocal(Cuve& cuve,const Cuve listeCuve[],const unsigned char tailleListeCuve);

  bool menuCalibrageSonde(SondeV2& sonde,const byte& nom_cuve);
  bool menuCalibrageSonde(Sonde& sonde,const byte& nom_cuve);

  char menuModifNom(char nom);

  void menuConfigurationHeureV2();
  void menuConfigurationHeure();

  int8_t regleProgramme(Programme& programme);
  void partReglageProgramme(int8_t pos, uint8_t* val, bool surligne);
  int8_t regleHeure(RtcDateTime& dateTime);
  void partReglageHeure(int8_t pos, uint8_t* val, bool surligne);

  friend void _creeLigneMenuSelectSondeV3(uint8_t i, char* chaine, uint8_t taille, void* arg);
  friend void _creeLigneMenuSelectSondeV2(uint8_t i, char* chaine, uint8_t taille, void* arg);
  friend void _creeLigneMenuSelectSondeForV2(uint8_t i, char* chaine, uint8_t taille, void* arg);
  friend void _creeLigneMenuChoixEV(uint8_t i, char* chaine, uint8_t taille, void* arg);
  friend void _creeLigneMenuChoixModeEV(uint8_t i, char* chaine, uint8_t taille, void* arg);
  friend void _creeLigneMenuManipuleEV(uint8_t i, char* chaine, uint8_t taille, void* arg);

protected:
  T* display;
};

struct structDataMenuSelectSonde {
  Sonde sonde[NB_SONDE_MAX];
  signed int temperature[NB_SONDE_MAX];
  signed char use[NB_SONDE_MAX];
  unsigned char nb;
  signed char sel;
  signed char pos;
  signed char deb;
  signed char timerID;
};

struct structDataMenuSelectSondeForV2 {
  signed char numSonde[Brochage::nbSonde+1];
  char utilisation[Brochage::nbSonde+1];
  unsigned char nb;
  signed char sel;
  signed char pos;
  signed char deb;
};

struct structDataMenuSelectSondeV3 {
  Sonde* sonde;
  signed int* temperature;
  signed char* use;
  unsigned char nb;
  signed char sel;
  signed char pos;
  signed char deb;
  signed char timerID;
};


void _creeLigneMenuSelectSondeV3(uint8_t i, char* chaine, uint8_t taille, void* arg);
void _creeLigneMenuSelectSondeV2(uint8_t i, char* chaine, uint8_t taille, void* arg);
void _creeLigneMenuSelectSondeForV2(uint8_t i, char* chaine, uint8_t taille, void* arg);
void _creeLigneMenuChoixEV(uint8_t i, char* chaine, uint8_t taille, void* arg);
void _creeLigneMenuChoixModeEV(uint8_t i, char* chaine, uint8_t taille, void* arg);
void _creeLigneMenuManipuleEV(uint8_t i, char* chaine, uint8_t taille, void* arg);


template<typename T>
CuvinoWidgets<T>::CuvinoWidgets(T& _display): Widgets<T>(_display){
  display=&_display;
}

template<typename T>
bool CuvinoWidgets<T>::menuCalibrageSonde(SondeV2& sonde,const byte& nom_cuve) {
  signed int tempMesure=0,tempReelle=0,tempMesure2=0,tempReelle2=0;
  bool passUn=true;
  unsigned char precision=sonde.getPrecision(); // on stock la precision d'origine
  sonde.setPrecision(PRECISION_HAUTE);  // et on met la precision à haute

  for(;;){
    display->clearDisplay();
    display->printPGM(FR::TXT_CALIBRAGE);
    display->setCursor(0,8);
    display->printPGM(FR::TXT_SONDE);
    display->print(' ');
    display->print(sonde.num);
    display->setCursor(0,16);
    display->printPGM(FR::TXT_MESURE);
    display->print(' ');
    display->printPGM(FR::TXT_EN_COURS);
    display->display();
    tempMesure=sonde.getTemperature(false);
    tempReelle=sonde.correctionMesure(tempMesure);

    display->setCursor(0,16);
    display->print('T');
    display->print((char)247);
    display->printPGM(FR::TXT_MESUREE);
    display->setCursor(8*6,16);
    display->print(':');
    display->printTemp16e(tempMesure,1,1);


    display->setCursor(0,24);
    display->print('T');
    display->print((char)247);
    display->printPGM(FR::TXT_CORRIGEE);
    display->setCursor(8*6,24);
    display->print(':');
    display->printTemp16e(tempReelle,1,1);

    display->setCursor(0,32);
    display->print('T');
    display->print((char)247);
    display->printPGM(FR::TXT_REELLE,1,1);
    display->setCursor(8*6,32);
    display->print(':');

    for(;;){
      unsigned char touche;
      display->setCursor(9*6,32);
      display->printTemp16e(tempReelle,1,1);
      for(;;){
        display->display();
        sonde.demandeMesureTemp();
        touche=display->getTouche(1);
        if( touche == BT_ESC) {
          sonde.setPrecision(precision);  // on remet la precision à la precision d'origine
          return false;
        }
        if( touche == BT_SELECT || touche==BT_BAS || touche == BT_HAUT) break;
        tempMesure=sonde.litTemperature(false);
        display->setCursor(9*6,16);
        display->printTemp16e(tempMesure,1);
        display->setCursor(9*6,24);
        display->printTemp16e(sonde.correctionMesure(tempMesure),1,1);
      }
      if( touche == BT_SELECT) break;
      if( touche == BT_BAS) tempReelle-=2;
      if( touche == BT_HAUT) tempReelle+=2;
    }

    if( passUn ){ // une seule mesure faite
      display->setCursor(0,24);
      display->print('T');
      display->print((char)247);
      display->printPGM(FR::TXT_REELLE);
      display->setCursor(8*6,32);
      display->print(':');
      display->printTemp16e(tempReelle,1);

      display->defillementEcranVt(16);
      display->printlnPGM(FR::TXT_FAIRE_SECONDE);
      display->printlnPGM(FR::TXT_MESURE);

      retourSelect result =this->menuListeOuiNon(32);

      if( result.touche==BT_ESC) {
        sonde.setPrecision(precision); // on remet la precision à la precision d'origine
        return false;
      }

      if( result.result == 1){  // non
        sonde.calcCorrection(tempMesure,tempReelle);
        sonde.setPrecision(precision); // on remet la precision à la precision d'origine
        return true;
      } else passUn=false; // prépapation pour 1 2e passage
    } else { // 2 mesures faites
      sonde.calcCorrection(tempMesure,tempReelle,tempMesure2,tempReelle2);
      sonde.setPrecision(precision); // on remet la precision à la precision d'origine
      return true;
    }
  }
  sonde.setPrecision(precision); // on remet la precision à la precision d'origine
  return true;
}
template<typename T>
bool CuvinoWidgets<T>::menuCalibrageSonde(Sonde& sonde,const byte& nom_cuve) {
  signed int tempMesure,tempReelle,tempMesure2,tempReelle2;
  bool passUn=true;
  unsigned char precision=sonde.getPrecision(); // on stock la precision d'origine
  sonde.setPrecision(PRECISION_HAUTE);  // et on met la precision à haute

  for(;;){
    display->clearDisplay();
    display->printPGM(FR::TXT_CALIBRAGE);
    affLigneSelectSonde(8,sonde,TEMP_NON_LUE);
    display->setCursor(0,16);
    display->printPGM(FR::TXT_MESURE);
    display->print(' ');
    display->printPGM(FR::TXT_EN_COURS);
    display->display();
    tempMesure=sonde.getTemperature(false);
    tempReelle=sonde.correctionMesure(tempMesure);

    display->setCursor(0,16);
    display->print('T');
    display->print((char)247);
    display->printPGM(FR::TXT_MESUREE);
    display->setCursor(8*6,16);
    display->print(':');
    display->printTemp16e(tempMesure,1,1);


    display->setCursor(0,24);
    display->print('T');
    display->print((char)247);
    display->printPGM(FR::TXT_CORRIGEE);
    display->setCursor(8*6,24);
    display->print(':');
    display->printTemp16e(tempReelle,1,1);

    display->setCursor(0,32);
    display->print('T');
    display->print((char)247);
    display->printPGM(FR::TXT_REELLE,1,1);
    display->setCursor(8*6,32);
    display->print(':');

    for(;;){
      unsigned char touche;
      display->setCursor(9*6,32);
      display->printTemp16e(tempReelle,1,1);
      for(;;){
        display->display();
        sonde.demandeMesureTemp();
        touche=display->getTouche(1);
        if( touche == BT_ESC) {
          sonde.setPrecision(precision);  // on remet la precision à la precision d'origine
          return false;
        }
        if( touche == BT_SELECT || touche==BT_BAS || touche == BT_HAUT) break;
        tempMesure=sonde.litTemperature(false);
        display->setCursor(9*6,16);
        display->printTemp16e(tempMesure,1);
        display->setCursor(9*6,24);
        display->printTemp16e(sonde.correctionMesure(tempMesure),1,1);
      }
      if( touche == BT_SELECT) break;
      if( touche == BT_BAS) tempReelle-=2;
      if( touche == BT_HAUT) tempReelle+=2;
    }

    if( passUn ){ // une seule mesure faite
      display->setCursor(0,24);
      display->print('T');
      display->print((char)247);
      display->printPGM(FR::TXT_REELLE);
      display->setCursor(8*6,32);
      display->print(':');
      display->printTemp16e(tempReelle,1);

      display->defillementEcranVt(16);
      display->printlnPGM(FR::TXT_FAIRE_SECONDE);
      display->printlnPGM(FR::TXT_MESURE);

      retourSelect result =this->menuListeOuiNon(32);

      if( result.touche==BT_ESC) {
        sonde.setPrecision(precision); // on remet la precision à la precision d'origine
        return false;
      }

      if( result.result == 1){  // non
        sonde.calcCorrection(tempMesure,tempReelle);
        sonde.setPrecision(precision); // on remet la precision à la precision d'origine
        return true;
      } else passUn=false; // prépapation pour 1 2e passage
    } else { // 2 mesures faites
      sonde.calcCorrection(tempMesure,tempReelle,tempMesure2,tempReelle2);
      sonde.setPrecision(precision); // on remet la precision à la precision d'origine
      return true;
    }
  }
  sonde.setPrecision(precision); // on remet la precision à la precision d'origine
  return true;
}

template<typename T>
void CuvinoWidgets<T>::printTemp(const int16_t& temp, uint8_t x, uint8_t y, bool forceFrac) {
  /* affiche la temperature sur 4 caractère (càd 24 pixels)
    si temperature >100° => pas de décimale, sinon 1 décimale.
    temp = temperature en 1/16e de degre (affichage fait en °C)
    si forceFrac = true, force l'affichage de la partie fractionnaire
  */
  if ( display->getTextSize() == 0) display->fillRect(x, y,  16,  6, display->getTextColorBg());
  else display->fillRect(x, y,  24,  8, display->getTextColorBg()); // efface l'emplacement de la temperature
  display->setCursor(x, y);
  switch (temp) {
    case TEMP_ARRET:
      display->printPGM(FR::TXT_STOP);
      return;
    case TEMP_SONDE_NON_CONFIGURE:
      display->printPGM(FR::TXT_N_CF);
      return;
  }

  int dec, frac;

  dec = abs(temp) >> 4; // partie entiere de la temperature
  frac = ((abs(temp) & B1111) * 625) / 1000; // partie fractionnaire de la temperature en 1/10e de degre

  if (dec > 99) { // temperature >= 100°
    if (temp < 0) {
      display->write('-');
    }
    display->setCursor(x + 6, y);
    display->print(dec);
  } else if (dec > 9) { // temperature >= 10°
    if (temp < 0) {
      if (frac != 0) {
        display->drawLine(x, y + 3, x + 2, y + 3, BLACK); // -
        display->setCursor(x + 3, y);
        display->print(dec); // partie entiere
        display->drawRect(x + 15, y + 5, 2, 2, BLACK); // point
        display->setCursor(x + 18, y);
        display->print(frac); // partie decimale
      } else {
        display->setCursor(x + 6, y);
        display->write('-');
        display->print(dec);
      }
    } else { // si positif
      if (frac != 0 || forceFrac) {
        display->print(dec);
        display->write('.');
        display->print(frac);
      } else {
        display->setCursor(x + 12, y);
        display->print(dec);
      }
    }
  } else { // temperature <10°
    uint8_t c = 18;
    if (temp < 0) {
      dec = -dec;
      c = 12;
    }
    if (frac != 0 || forceFrac) c -= 12;
    display->setCursor(x + c, y);
    display->print(dec);
    if (frac != 0 || forceFrac) {
      display->write('.');
      display->print(frac);
    }
  }
  if ( display->getTextSize() == 0) display->print((char)247);
  else {
    if ( display->getTextColor() == BLACK) display->drawBitmap(x + 24, y, &BMPglyph_degC[0], 5, 7, BLACK, WHITE);
    else display->drawBitmap(x + 24, y, &BMPglyph_degC[0], 5, 7, WHITE, BLACK);
  }

  display->display();
}

template<typename T>
void CuvinoWidgets<T>::affLigneSelectSonde(uint8_t y, Sonde sonde, int16_t temp) {
  display->setCursor(0, y);
  display->setTextSize(0);

  if ( !sonde.isSondeTemp()) { // si le numéro de la sonde commence par 0
    display->printPGM(FR::TXT_AUCUNE);
    display->setTextSize(1);
    return;
  }
  if ( sonde.isDS3231()) { // sonde interne (dans le rtc)
    display->printPGM(FR::TXT_INTERNE);
  } else { // sonde OneWire
    for (uint8_t j = 1; j < 8; ++j) {
      if ( (uint8_t)sonde.addr[j] < 16) display->print(0);
      display->print(sonde.addr[j], HEX);
    }
  }
  display->setCursor(66, y);

  if ( temp == TEMP_NON_LUE) {
    display->print(" -- ");
  } else if ( temp < TEMP_NON_LUE) {
    display->print("Err");
    display->print(TEMP_ERREUR - temp);
  } else {
    if (temp > 0) display->write(' ');
    display->print((int8_t)(temp / 16));
    display->print((char)248);
  }
  display->setTextSize(1);
}

template <typename T>
Sonde* CuvinoWidgets<T>::menuSelectSondeV3(const Sonde listeSondeExistante[],const unsigned char tailleListeSonde, OneWire& ds, signed char act, bool interne,bool aucune){
  #ifdef DEBUG
    Serial.print("menuSelectSondeV3");
  #endif
    structDataMenuSelectSondeV3 data;
    data.sel = -2;

    display->clearDisplay();

    display->printlnPGM(FR::TXT_SELECT_SONDE);

  // recherche du nb de sonde
  data.nb=getNbDS18B20(ds);
  if(interne) data.nb+=1;
  if( data.nb > NB_SONDE_MAX) data.nb=NB_SONDE_MAX;

  #ifdef DEBUG
    Serial.print("nb sonde trouvée:");
    Serial.println(data.nb);
  #endif

  data.sonde=new Sonde[data.nb];
  data.temperature=new signed int[data.nb];
  data.use=new signed char[data.nb];

  if( ! initRechercheDS18B20(ds)){
    if( !initRechercheDS18B20(ds) ) {
      Sonde sonde=Sonde();
      return &sonde;
    }
  }

    for(unsigned char i=0;i<data.nb;++i){
      // recherche des sondes
      if(i==0 && interne){ // si sonde 0 et cuve LOCAL alors DS3231 autorisé (interne)
        data.sonde[0].addr[0]='D';
        data.sonde[0].addr[1]='S';
        data.sonde[0].addr[2]='3';
        data.sonde[0].addr[3]='2';
        data.sonde[0].addr[4]='3';
        data.sonde[0].addr[5]='1';
        data.sonde[0].addr[6]=0;
      }
      else getDS18B20suivant(ds,data.sonde[i].addr);
    }

    finRechercheDS18B20();

    for (uint8_t i = 0; i < data.nb; ++i) {

      data.sonde[i].demandeMesureTemp(); // demande une mesure de la temperature de la sonde i
      data.temperature[i] = TEMP_NON_LUE;

        // alors on va chercher si dans la liste des sondes déjà affecté, elle ne serait pas déjà existante

        // on cherche dans les sondes de chaque cuve
      for (uint8_t j = 0; j < tailleListeSonde; ++j) {

        // puis vérification sonde pas déjà utilisée
        if( data.sonde[i] == listeSondeExistante[j]){
          data.use[i]=j;
        //  if( listeCuve[j]==cuve) data.sel=i;
        } else{
          data.use[i]=-2;
        }
        //  for (uint8_t k = 0; k < 8; ++k) {
        //    if (data.sonde[i].addr[k] != listeCuve[j].sonde.addr[k]) break;
        //    if ( k == 7) {
        //      data.use[i] = j;
        //      if ( j == cuve) data.sel = i;
        //    }
        //  }
      }
    }

    retourSelect result = this->menuListeV2(_creeLigneMenuSelectSondeV3, (void*)&data, data.nb+1, 15, 16, LCDHEIGHT, 0, LCDWIDTH, /*hauteurLigne*/ 1,/*depart*/ (data.sel != -2) ? data.sel : 0, /*textSize*/0, /*actual*/ 1);

    display->setTextSize(1); // on remet la taille du texte à 1

    delete[] data.use;
    delete[] data.sonde;
    delete[] data.temperature;

    Sonde sonde;

    if (result.touche == 0) {
  #ifdef DEBUG
      if (interne) {
        Serial.print("local");
      }else{
        Serial.print("cuve ");
      }
      Serial.print(" - Selection de la sonde :");
  #endif

      if( result.result==0){ // aucune sonde
        //
      }else{
        sonde=data.sonde[result.result-1];
        //for (uint8_t k = 0; k < 8; ++k) {
        //  cuve.sonde.addr[k] = data.sonde[result.result-1].addr[k];
    //#ifdef DEBUG
      //    Serial.print(data.sonde[result.result-1].addr[k], HEX);
    //#endif
    //    }
      }
    }
    #ifdef DEBUG
    Serial.print("exit");
    #endif
    return &sonde;
}

template <typename T>
bool CuvinoWidgets<T>::menuSelectSondeV2(Cuve& cuve,const Cuve listeCuve[],const unsigned char tailleListeCuve,OneWire& ds) {
#ifdef DEBUG
  Serial.print("menuSelectSondeV2(");
  Serial.print(cuve.nom);
  Serial.println(')');
#endif
  structDataMenuSelectSonde data;
  data.sel = -2;
  for( unsigned char i=0;i< NB_SONDE_MAX; ++i) data.sonde[i].defaut();

  display->clearDisplay();

  display->printlnPGM(FR::TXT_SELECT_SONDE);
  display->setCursor(0, 8);
  if ( cuve.tempConsigneCuve==TEMP_SONDE_LOCAL) display->printlnPGM(FR::TXT_DU_LOCAL);
  else {
    display->printPGM(FR::TXT_CUVE);
    display->write(' ');
    display->println((char)cuve.nom);
  }

  if(cuve.tempConsigneCuve==TEMP_SONDE_LOCAL) {
    data.nb=1;
    data.sonde[0].addr[0]='D';
    data.sonde[0].addr[1]='S';
    data.sonde[0].addr[2]='3';
    data.sonde[0].addr[3]='2';
    data.sonde[0].addr[4]='3';
    data.sonde[0].addr[5]='1';
    data.sonde[0].addr[6]=0;
  } else data.nb=0;

/*  if( ! prendRessourceSonde() ){
    if( ! prendRessourceSonde() ){
      return false;
    }
  }*/

  ds.reset_search();    // Réinitialise la recherche de module
  while (ds.search(data.sonde[data.nb].addr)) {
  #ifdef DEBUG
    Serial.print("addresse 1Wire ");
    for (uint8_t c = 0; c < 8; ++c) Serial.print(data.sonde[data.nb].addr[c], HEX);
    Serial.println("");
  #endif
    if (OneWire::crc8(data.sonde[data.nb].addr, 7) != data.sonde[data.nb].addr[7]) {// Vérifie que l'adresse a été correctement reçue
  #ifdef DEBUG
      Serial.print("Erreur lors de la transmission");
  #endif
      break;
    }
    if (data.sonde[data.nb].addr[0] == DS18B20) // Vérifie qu'il s'agit bien d'un DS18B20
      ++data.nb;
      if( data.nb >= NB_SONDE_MAX) break;
  }
  //rendRessourceSonde();

  #ifdef DEBUG
    Serial.print("nb sonde trouvée:");
    Serial.println(data.nb);
  #endif
  if( data.nb<NB_SONDE_MAX) data.sonde[data.nb].addr[0]=0;

  for (unsigned char i = 0; i < data.nb; ++i) {

    if( data.sonde[i].isDS3231()){
      data.temperature[i]=data.sonde[i].litTemperature(); // si DS3231 lecture directe de la température
    }else{
      data.sonde[i].demandeMesureTemp(); // sinon demande une mesure de la temperature de la sonde i
      data.temperature[i] = TEMP_NON_LUE;
    }

    if( data.sonde[i]==cuve.sonde) data.sel=i;

      // alors on va chercher si dans la liste des sondes déjà affecté, elle ne serait pas déjà existante
      // on cherche dans les sondes de chaque cuve
    for (unsigned char j = 0; j < tailleListeCuve; ++j) {

      // puis vérification sonde pas déjà utilisée
      if( data.sonde[i] == listeCuve[j].sonde){
        data.use[i]=j;
      } else{
        data.use[i]=-2;
      }
    }
  }

  retourSelect result = this->menuListeV2(_creeLigneMenuSelectSondeV2, (void*)&data, data.nb+1, 15, 16, LCDHEIGHT, 0, LCDWIDTH, 1, (data.sel != -2) ? data.sel : 0, 0, 1);
  if( result.touche==TIMEOUT_LED)return false;

  display->setTextSize(1); // on remet la taille du texte à 1

  if (result.touche == 0) {
#ifdef DEBUG
    if (cuve.tempConsigneCuve==TEMP_SONDE_LOCAL) {
      Serial.print("local");
    }else{
      Serial.print("cuve ");
      Serial.print(cuve.nom);
    }
    Serial.print(" - Selection de la sonde :");
#endif
    if( result.result==0){ // aucune sonde
      cuve.sonde.defaut();
    }else{
      cuve.sonde=data.sonde[result.result-1];
    }
  }
  return true;
}

template <typename T>
bool CuvinoWidgets<T>::menuSelectSondeForV2(CuveV2& cuve,const CuveV2 listeCuve[],const unsigned char tailleListeCuve) {
#ifdef DEBUG
  Serial.print("menuSelectSondeForV2(");
  Serial.print(cuve.nom);
  Serial.println(')');

  Serial.print("sonde num:");
  if( cuve.sonde==NULL) Serial.print("aucune");
  else  Serial.println(cuve.sonde->num);

#endif
  structDataMenuSelectSondeForV2 data;
  data.sel = 0;

  display->clearDisplay();

  display->printlnPGM(FR::TXT_SELECT_SONDE);
  display->setCursor(0, 8);
  if ( cuve.tempConsigneCuve==TEMP_SONDE_LOCAL) display->printlnPGM(FR::TXT_DU_LOCAL);
  else {
    display->printPGM(FR::TXT_CUVE);
    display->write(' ');
    display->println((char)cuve.nom);
  }

  if(cuve.tempConsigneCuve==TEMP_SONDE_LOCAL) { // si sonde interne
    data.nb=1;
    data.numSonde[0]=SONDE_DS3231;
    data.utilisation[0]=0;
    for(unsigned char j=0;j<tailleListeCuve;++j){ // parcours toutes les cuves
      if( (unsigned int)listeCuve[j].sonde == (unsigned int)Brochage::sondeV2(SONDE_DS3231) ){
        data.utilisation[0]=listeCuve[j].nom;
      }
    }
  } else data.nb=0;

  for(unsigned char i=0;i<Brochage::nbSonde;++i){ // regarde toutes les sondes
    if( ((unsigned int)cuve.sonde) == ((unsigned int)Brochage::sondeV2(i)) ){ // si la sonde est déjà attribué à la cuve en cours d'édition
      data.numSonde[data.nb]=Brochage::sondeV2(i)->num;
      data.sel=data.nb + 1;
      data.nb++;
      data.utilisation[data.nb]=1;
    } else { // sonde non attribué à cuve en cours d'édition
      unsigned char j;
      data.utilisation[data.nb]=0;
      for(j=0;j<tailleListeCuve;++j){ // parcours toutes les cuves
        if( (unsigned int)listeCuve[j].sonde == (unsigned int)Brochage::sondeV2(i) ){ // si sonde attribuée à une autre cuve
          data.utilisation[data.nb]=listeCuve[j].nom;
          break;
        }
      }
      if( (unsigned int)listeCuve[j].sonde != (unsigned int)Brochage::sondeV2(i) ){ // si une sonde n'est pas déjà attribué à une cuve
        data.utilisation[data.nb]=0;
      }

      data.numSonde[data.nb]=Brochage::sondeV2(i)->num;
      data.nb++;
    }
  }

  #ifdef DEBUG
    Serial.print("nb sonde trouvée:");
    Serial.println(data.nb);
  #endif

  retourSelect result = this->menuListeV2(_creeLigneMenuSelectSondeForV2, (void*)&data, data.nb+1, 15, 16, LCDHEIGHT, 0, LCDWIDTH, 1, data.sel, 1, 1);
  if( result.touche==TIMEOUT_LED)return false;

  display->setTextSize(1); // on remet la taille du texte à 1

  if (result.touche == 0) {
#ifdef DEBUG
    if (cuve.tempConsigneCuve==TEMP_SONDE_LOCAL) {
      Serial.print("local");
    }else{
      Serial.print("cuve ");
      Serial.print(cuve.nom);
    }
    Serial.print(" - Selection de la sonde :");
#endif
    if( result.result==0){ // aucune sonde
      cuve.sonde=Brochage::sondeV2(AUCUNE_SONDE);
    }else{
      cuve.sonde=Brochage::sondeV2(result.result-1);

      // // il faut vérifier que la sonde ne soit pas déjà utilisée ailleurs et éventuellement la déssafectéer
      // for( unsigned char i=0;i<tailleListeCuve;++i){
      //   if( cuve.nom != listeCuve[i].nom && (unsigned int)listeCuve[i].sonde == (unsigned int)cuve.sonde ){
      //     listeCuve[i].sondeNum=AUCUNE_SONDE;
      //     listeCuve[i].sonde=NULL;
      //   }
      // }
    }
  }
  return true;
}

template <typename T>
bool CuvinoWidgets<T>::menuSelectSondeV2(Cuve& cuve,OneWire& ds) {
  Cuve listeCuve[0];
  return menuSelectSondeV2(cuve.sonde,listeCuve,0,(cuve.tempConsigneCuve==TEMP_SONDE_LOCAL),ds);
}

template <typename T>
retourSelect CuvinoWidgets<T>::menuListeChoixEV(ElectroVanne2& EV, uint8_t yMin, uint8_t yMax, uint8_t xMin, uint8_t xMax) {
  uint8_t depart=EV.numero;
  if( depart >= NB_EV_MAX) depart=0;
  return this->menuListeV2(_creeLigneMenuChoixEV, (void*)&EV, NB_EV_MAX + 1, 7, yMin, yMax, xMin, xMax, 1, depart, 1, 0, B10010000);
}

template <typename T>
retourSelect CuvinoWidgets<T>::menuListeChoixModeEV(ElectroVanne2& EV, uint8_t yMin, uint8_t yMax, uint8_t xMin, uint8_t xMax) {
  uint8_t n = sizeListeEV(); uint8_t depart=0;

  retourSelect result;

  if ( n == 1) {
    result.touche = 0;
    //result.result = mode[0];
    result.result = 0;
  } else {
    result = this->menuListeV2(_creeLigneMenuChoixModeEV, (void*)&EV, n, 15, yMin, yMax, xMin, xMax, 1, depart, 1, 0, B10010000);
    //result.result = mode[result.result];
  }
  return result;
}



template <typename T>
char CuvinoWidgets<T>::menuModifNom(char nom) {
  //return nom;

  if ( nom < 0) nom = 65;
  uint8_t touche;
  uint8_t x = (nom % 64) % 14;
  uint8_t y = (nom % 64) / 14;
  uint8_t page = nom / 64;

  for (;;) {
    display->clearDisplay();
    display->printPGM(FR::TXT_CHOIX_);
    display->printPGM(FR::TXT_NOM);

    display->setCursor(0, 8);
    for (uint8_t i = 0; i < 5; ++i) {
      for (uint8_t j = 0; j < 14; ++j) {
        if (i * 14 + j == 64) break;
        switch (page * 64 + i * 14 + j) {
          case 0:
          case 10:
          case 13:
          case 255:
          case 254:
            display->write(' ');
            break;
          default:
            display->print((char)(page * 64 + i * 14 + j));
        }
      }
    }
    //for (;;) {
      display->invertRect(x * 6, (y + 1) * 8, 6, 8);
      display->display();
      display->aucuneTouche();
      touche = display->getTouche(-1);
      display->invertRect(x * 6, (y + 1) * 8, 6, 8);
      switch(touche){
        case BT_ESC:
          if ( x != 0) --x;
          else {
            if (y != 0) {
              --y; x = 13;
            } else {
              page = ~page & 1;
              y = 4; x = 7;
              break;
            }
          }
          break;
        case BT_SELECT:
          if ( x == 7  && y == 4) {
            page = ~page & 1;
            x = 0; y = 0;
            break;
          } else if ( x < 13) ++x;
          else {
            x = 0; ++y;
          }
          break;
        case BT_HAUT:
          if( y != 0 ) x--;
          else {
            page = ~page & 1;
            y=4;
            if(x>7) x=7;
          }
          break;
        case BT_BAS:
          if( y == 4 || (y==3 && x>7)){
            y=0;
            page = ~page & 1;
          } else ++y;
          break;
        case BT_MENU:
          return (page * 64 + y * 14 + x);
        case TIMEOUT_LED:
          return (char)0;
        case BT_BAS | BT_HAUT:
          return nom;
    //  }
    }
  }
}


template <typename T>
bool CuvinoWidgets<T>::menuManipuleEV(ElectroVanne2& EV,uint8_t yMin, uint8_t yMax, uint8_t xMin, uint8_t xMax){
  display->fillRect(xMin, yMin, xMax-xMin, yMax-yMin, WHITE);

  display->setCursor(xMin,yMin);
  display->printPGM(FR::TXT_EV);
  display->write(':');
  display->println(EV.numero);
  display->printPGM(FR::TXT_MODE);
  display->write(':');
  display->printlnPGM(TXT_EV(EV.mode));
  //display->print(EV.mode,BIN);

  retourSelect result;
  result.result=0;

  for(;;){
    display->setCursor(xMin,yMin+16);
    display->printPGM(FR::TXT_ETAT);
    display->write(':');
    if( EV.etatEV() == OUVERT) display->printlnPGM(FR::TXT_OUVERT);
    else if( EV.etatEV() == FERME) display->printlnPGM(FR::TXT_FERME);
    else display->printlnPGM(FR::TXT_PARTIEL);

    result = this->menuListeV2(_creeLigneMenuManipuleEV, (void*)&EV, 5, 15, yMin+3*8, yMax, xMin, xMax, 1, result.result, 1, 0, B10010000);

    if( result.touche==TIMEOUT_LED)return false;

    if( result.touche==BT_ESC){
      return true;
    }else if (result.touche == 0) {
      switch(result.result){
        case 0: // ouverture
          EV.bougeEV(OUVERT);
          break;
        case 1: // fermeture
          EV.bougeEV(FERME);
          break;
        case 2: // etat
          break;
        case 3: // valider
          return true;
          break;
        case 4: // annuler
          return false;
          break;
      }
    }
  }
  return true;
}


template <typename T>
bool CuvinoWidgets<T>::menuConfigEV(ElectroVanne2& EV,uint8_t yMin, uint8_t yMax, uint8_t xMin, uint8_t xMax){
  // choix de l'electrovanne
  display->setCursor(xMin, yMin);
  display->printPGM(FR::TXT_SELECT);
  display->write(' ');
  display->printPGM(FR::TXT_EV);


  retourSelect result = menuListeChoixEV(EV, yMin+8,yMax,xMin,xMax);
  if(result.touche==TIMEOUT_LED) return false;

  if ( result.touche == 0) {
    // choix du mode de l'electrovanne

    if ( result.result == NB_EV_MAX) { // alors aucune electrovanne
        EV.numero = 255;
        EV.mode = EV_NON_CONFIGURE;
        EV.tempsOuvertureVanne=0;
    } else {
      uint8_t numero = result.result;

      display->fillRect(xMin,xMax-xMin,yMin,yMax-yMin,WHITE);
      display->printPGM(FR::TXT_SELECTION_MODE);
      display->printPGM(FR::TXT_EV);
      display->write(' ');
      if ( numero < 10) display->print(numero);
      else display->print((char)(numero + 55));

      result = menuListeChoixModeEV(EV, yMin+8,yMax,xMin,yMax);
      if(result.touche==TIMEOUT_LED) return false;

      if ( result.touche == (BT_BAS | BT_HAUT) || ((result.touche&TIMEOUT) != 0)) {
        EV.mode = EV_NON_CONFIGURE;
        EV.numero = 255;
        EV.tempsOuvertureVanne=0;
      } else {
        /*#ifdef DEBUG
          Debug.print("CW:");
          Debug.print("result.result:");
          Debug.print(result.result,BIN);
          Debug.print(" => mode:");
          Debug.println(EVinListe(result.result),BIN);
        #endif*/
        EV.mode = EVinListe(result.result);
        EV.numero = numero;
        EV.tempsOuvertureVanne=0;
        EV.initEV();
      }

      if (EV.mode != EV_NON_CONFIGURE ){
        if( ! this->menuManipuleEV(EV)){
          EV.mode=EV_NON_CONFIGURE;
          EV.numero=255;
          EV.tempsOuvertureVanne=0;
          return false;
        }

        display->clearDisplay();
        display->printlnPGM(FR::TXT_TEST_DE_L);
        display->printlnPGM(FR::TXT_ELECTROVANNE);
        display->printlnPGM(FR::TXT_EN_COURS);
        display->display();
        uint8_t erreur=EV.testEV();
        if (  erreur != 0) {
          // si test de l'électrovanne échoué
          EV.mode = EV_NON_CONFIGURE;
          display->clearDisplay();
          display->printlnPGM(FR::TXT_TEST_DE_L);
          display->printlnPGM(FR::TXT_ELECTROVANNE);
          display->printlnPGM(FR::TXT_ECHOUE);
          display->printPGM(FR::TXT_ERREUR);
          display->write(' ');
          display->print(erreur);
        }
      }
      if (EV.mode != EV_NON_CONFIGURE ){
        // si test electrovanne réussi
        display->clearDisplay();
        display->printlnPGM(FR::TXT_TEST_DE_L);
        display->printlnPGM(FR::TXT_ELECTROVANNE);
        display->printlnPGM(FR::TXT_OK);
      }
      display->display();
      display->aucuneTouche();
      display->getTouche(-1);
    }
  }
  return true;
}


template <typename T>
int8_t CuvinoWidgets<T>::regleProgramme(Programme& programme) {
  #ifdef DEBUG
    Serial.print("widgets.regleProgramme(");
    Serial.print(programme.date, HEX);
    Serial.println(')');
    programme.println(Serial);
  #endif

    RtcDateTime dateTime;
    uint8_t mode = programme.mode();
    uint8_t repetition = programme.freqRepetition();

    if( programme.estRepetition() ) dateTime=programme.getDate();
    else{
      if( programme.date==0) dateTime = RtcDateTime(16, 8, 1, 0, 0, 0);
      else dateTime=programme.getDate();
    }

    uint8_t h = dateTime.Hour();
    uint8_t m = dateTime.Minute();
    uint8_t J = dateTime.Day();
    uint8_t M = dateTime.Month();
    uint8_t A = (uint8_t)(dateTime.Year() - 2000);

    uint8_t* val = &mode;

    for (int8_t pos = -3;;) {
      display->fillRect(0, 8, 84, 40, WHITE);

      display->setCursor(0, 8);
      display->printPGM(FR::TXT_REPETITION);
      display->write(':');
      partReglageProgramme(-3, &mode, (pos == -3));
      if ( mode != 0) {
        display->setCursor(0, 17);
        display->setTextSize(0);
        display->printPGM(FR::TXT_SELECTION_MODE, true, 10);
        display->write(':');
        partReglageProgramme(-2, &mode, (pos == -2));
        if ( mode == MODE_PROG_H || mode == MODE_PROG_J) {
          if ( repetition == 0) repetition = 1;
          partReglageProgramme(-1, &repetition, false);
        }
      }

      if ( mode < 0b100) {
        partReglageProgramme(0, &h, (pos == 0));
        display->write(':');
        partReglageProgramme(1, &m, false);
        switch (mode) {
          case 0:
          case MODE_PROG_M:
            partReglageProgramme(2, &J, false);
            if ( mode == 0) {
              display->write('/');
              partReglageProgramme(3, &M, false);
              display->write('/');
              partReglageProgramme(4, &A, false);
            }
            break;
          case MODE_PROG_S:
            partReglageProgramme(-3, &J, false);
        }
        if ( mode == MODE_PROG_M || mode == MODE_PROG_S) {
          display->setCursor(18, 32);
          display->printPGM(FR::TXT_LE);
        }
      }

      partReglageProgramme(5, &h, false);
      partReglageProgramme(6, &h, false);

      for (;;) {
        uint8_t touche;
        if ( mode == 0 ) {
          display->setCursor(0, 32);
          display->printPGM(FR::TXT_LE, true);
          display->write(' ');

          display->printPGM(FR::JOURS[dateTime.DayOfWeek()]);
        }
        display->display();
        display->aucuneTouche();
        touche = display->getTouche(-1);
        if ( touche == BT_ESC && pos < 5) { // diminue
          *val = *val - 1;
          switch (pos) {
            case -1:
              if ( repetition == 0 ) repetition = 31;
              break;
            case -2:
              if ( mode == 0) mode = 7;
              break;
            case -3:
              if ( mode > 8 ) mode = 1;
              break;
            case 0:
              if ( h > 23) h = 23;
              break;
            case 1:
              if ( m > 59) m = 59;
              break;
            case 3:
              if ( M == 0) M = 12;
            case 4:
              if ( A < 16) A = 25;
              if ( A > 25) A = 16;
            case 2:
              switch (mode) {
                case 0:  // jour
                  if( J==0){
                    if ( pos == 2) {
                      if( M == 4 || M == 6 || M == 9 || M == 11) J=30;
                      else if( M == 2){
                        if( A % 4 == 0 ) J=29;
                        else J=28;
                      }
                      else J=31;
                    }
                  } else {
                    if (J>30 && (M == 4 || M == 6 || M == 9 || M == 11)) J = 30;
                    else if (M==2 && J > 29 && (A % 4) == 0) J = 29;
                    else if( M==2 && J > 28) J = 28;
                    partReglageProgramme(2, &J, false);
                  }
                  break;
                case MODE_PROG_S: // jour de la semaine
                  J = J % 7;
                  break;
                case MODE_PROG_M:
                  J = J % 31;
              }
              break;
          }
          // calcul du nombre de jour plein écoulés
          //res = 0;
          //for ( int8_t i = 0; i < A; ++i) res += ((i % 4 == 0) ? 366 : 365);
          //for ( int8_t i = 0; i < (M - 1); ++i) res += pgm_read_uint8_t(c_daysInMonth + i);
          //res += J - 1;

          // calcul du  nombre de seconde
          //res = ((res * 24 + h) * 60 + m) * 60;
          //dateTime = RtcDateTime(res);
          dateTime = RtcDateTime(A, M, J, h, m, 0);
          partReglageProgramme(((mode == MODE_PROG_S && pos == 2) ? -10 : pos), val, true);
          if ( pos == -3 || pos == -2 ) break;

        } else if ( touche == BT_SELECT && pos < 5) { // diminue
          *val = *val + 1;
          switch (pos) {
            case -1:
              if ( repetition > 31) repetition = 1;
              break;
            case -2:
              if ( mode > 7) mode = 1;
              break;
            case -3:
              if ( mode > 1) mode = 0;
              break;
            case 0:
              if ( h > 23) h = 0;
              break;
            case 1:
              if ( m > 59) m = 0;
              break;
            case 3:
              if ( M > 12) M = 1;
            case 4:
              if ( A > 25) A = 16;
              if ( A < 16) A = 25;
            case 2:
              switch (mode) {
                case 0:  // jour
                  if ( J > 31 || ((M == 4 || M == 6 || M == 9 || M == 11) && J > 30) || (M == 2 && (J > 29)) || (M == 2 && (J > 28) && A % 4 != 0)) {
                    if ( pos == 2) {
                      J = 1;
                      //              M++; partReglageHeure(4, &M, false);
                      //              if ( M > 12) {
                      //                M = 1;
                      //                A++; partReglageHeure(5, &A, false);
                      //                if ( A > 99) A = 0;
                      //              }
                    } else {
                      if (M == 4 || M == 6 || M == 9 || M == 11) J = 30;
                      else if (A % 4 == 0) J = 29;
                      else J = 28;
                      partReglageProgramme(2, &J, false);
                    }
                  }
                  break;
                case MODE_PROG_S: // jour de la semaine
                  J = J % 7;
                  break;
                case MODE_PROG_M:
                  J = J % 31;
              }
              break;
          }
          // calcul du nombre de jour plein écoulés
          //res = 0;
          //for ( int8_t i = 0; i < A; ++i) res += ((i % 4 == 0) ? 366 : 365);
          //for ( int8_t i = 0; i < (M - 1); ++i) res += pgm_read_uint8_t(c_daysInMonth + i);
          //res += J - 1;

          // calcul du  nombre de seconde
          //res = ((res * 24 + h) * 60 + m) * 60;
          //dateTime = RtcDateTime(res);
          dateTime = RtcDateTime(A, M, J, h, m, 0);
          partReglageProgramme(((mode == MODE_PROG_S && pos == 2) ? -10 : pos), val, true);
          if ( pos == -3 || pos == -2 ) break;

        } else if ( touche == BT_SELECT || touche == (BT_BAS | BT_HAUT) || ((touche&TIMEOUT) != 0)) {
          if ( touche == BT_SELECT && pos == 5) { // OK
            // Rtc.SetDateTime(dateTime);
            // lastDate = dateTime.TotalSeconds();

  #ifdef DEBUG
            Serial.print("date=");
            Serial.print(dateTime.TotalSeconds(), HEX);
            Serial.print('(');
            Serial.print(dateTime.Day());
            Serial.print('/');
            Serial.print(dateTime.Month());
            Serial.print('/');
            Serial.print(dateTime.Year());
            Serial.print(' ');
            Serial.print(dateTime.Hour());
            Serial.print(':');
            Serial.print(dateTime.Minute());
            Serial.print(':');
            Serial.print(dateTime.Second());
            Serial.print(") mode=");
            Serial.print(mode, HEX);
            Serial.print(" repetition=");
            Serial.println(repetition);
  #endif

            uint32_t date = dateTime.TotalSeconds();
            switch (mode) {
              case MODE_PROG_HC_HP:
              case MODE_PROG_HP_HC:
              case MODE_PROG_10M:
                repetition = 1;
              case MODE_PROG_H:
                date = 0;
                break;
              case MODE_PROG_J:
                date = date % 86400;
                break;
              case MODE_PROG_M:
                repetition = 1;
                date = date % 2678400;
                break;
              case MODE_PROG_S:
                repetition = 1;
                date = date % 604800;
              case 0:
                repetition = 0;
            }
#ifdef DEBUG
            dateTime = RtcDateTime(date);
            Serial.print("   date=");
            Serial.print(dateTime.TotalSeconds(), HEX);
            Serial.print('(');
            Serial.print(dateTime.Day());
            Serial.print('/');
            Serial.print(dateTime.Month());
            Serial.print('/');
            Serial.print(dateTime.Year());
            Serial.print(' ');
            Serial.print(dateTime.Hour());
            Serial.print(':');
            Serial.print(dateTime.Minute());
            Serial.print(':');
            Serial.print(dateTime.Second());
            Serial.println(")");
            Serial.print(" 'mode'=");
            Serial.println(((((uint32_t)mode) & 0b110) << 29) | ((((uint32_t)mode) & 0b001) << 27), HEX);
            Serial.print("'repet'=");
            Serial.println((((uint32_t)repetition) << 22), HEX);
#endif
            switch(mode){
              case MODE_PROG_NUL:
                programme.setDate(dateTime);
                break;
              case MODE_PROG_10M:
              case MODE_PROG_HC_HP:
              case MODE_PROG_HP_HC:
                programme.setRepetition(mode);
                break;
              case MODE_PROG_H:
                programme.setRepetition(mode,repetition);
                break;
              case MODE_PROG_S:
              case MODE_PROG_M:
                repetition=1;
              case MODE_PROG_J:
                programme.setRepetition(mode,repetition,dateTime);
                break;
            }
            return 1;
          } else { // annuler
            return 0;
          }
        } else if ( touche == BT_BAS || touche == BT_HAUT) { // chgt position
          partReglageProgramme(((mode == MODE_PROG_S && pos == 2) ? -10 : pos), val, false);
          if ( touche == BT_BAS) {
            ++pos;
            if ( (pos == -2 && mode == 0) || ( pos == -1 && mode != MODE_PROG_H && mode != MODE_PROG_J)) pos = 0;
            if ( pos > 6) pos = -3;
            if ( pos < 5 && ((mode >= 0b100 && pos >= 0) || (mode == MODE_PROG_J && pos > 1) || ((mode == MODE_PROG_S || mode == MODE_PROG_M) && pos > 2))) pos = 5;
          } else {
            if ( pos == -3) pos = 6;
            else {
              --pos;
              if ( pos == 4 && mode != 0) {
                if ( mode >= 0b100 ) pos = -1;
                else if ( mode == MODE_PROG_J) pos = 1;
                else pos = 2;
              }
              if ( pos == -1 && mode == 0) pos = -3;
              if ( pos == -1 && mode != MODE_PROG_H && mode != MODE_PROG_J) pos = -2;
            }
          }

          switch (pos) {
            case -1:
              val = &repetition;
              break;
            case -3:
            case -2:
              val = &mode;
              break;
            case 0:
              val = &h;
              break;
            case 1:
              val = &m;
              break;
            case -10:
            case 2:
              val = &J;
              break;
            case 3:
              val = &M;
              break;
            case 4:
              val = &A;
              break;
          }

          partReglageProgramme(((mode == MODE_PROG_S && pos == 2) ? -10 : pos), val, true);

        }
      }
    }
  }

template <typename T>
int8_t CuvinoWidgets<T>::regleHeure(RtcDateTime & dateTime) {
#ifdef DEBUG
  Serial.print("regleHeure(");
  Serial.print(dateTime.TotalSeconds(), HEX);
  Serial.println(')');
#endif

  uint8_t mode = 0, repetition = 1;

  if ( dateTime.TotalSeconds() == 0 ) {
    dateTime = RtcDateTime(16, 8, 1, 0, 0, 0);
  }

  uint8_t h = dateTime.Hour();
  uint8_t m = dateTime.Minute();
  uint8_t J = dateTime.Day();
  uint8_t M = dateTime.Month();
  uint8_t A = (uint8_t)(dateTime.Year() - 2000);

  uint8_t* val = &h;

  for (int8_t pos = 0;;) {
    display->fillRect(0, 8, 84, 40, WHITE);


    partReglageHeure(0, &h, (pos == 0));
    display->write(':');
    partReglageHeure(1, &m, false);
    partReglageHeure(2, &J, false);
    display->write('/');
    partReglageHeure(3, &M, false);
    display->write('/');
    partReglageHeure(4, &A, false);
    partReglageHeure(5, &h, false);
    partReglageHeure(6, &h, false);

    for (;;) {
      uint8_t touche;

      display->setCursor(0, 24);
      display->printPGM(FR::JOURS[dateTime.DayOfWeek()]);

      display->display();
      display->aucuneTouche();
      touche = display->getTouche(-1);
      if ( touche == BT_SELECT && pos < 5) { // augmente valeur
        *val = *val + 1;
        switch (pos) {
          case 0:
            if ( h > 23) h = 0;
            break;
          case 1:
            if ( m > 59) m = 0;
            break;
          case 3:
            if ( M > 12) M = 1;
          case 4:
            if ( A > 25) A = 16;
            if ( A < 16) A = 25;
          case 2:
            if ( J > 31 || ((M == 4 || M == 6 || M == 9 || M == 11) && J > 30) || (M == 2 && (J > 29)) || (M == 2 && (J > 28) && A % 4 != 0)) {
              if ( pos == 2) {
                J = 1;
                //              M++; partReglageHeure(4, &M, false);
                //              if ( M > 12) {
                //                M = 1;
                //                A++; partReglageHeure(5, &A, false);
                //                if ( A > 99) A = 0;
                //              }
              } else {
                if (M == 4 || M == 6 || M == 9 || M == 11) J = 30;
                else if (A % 4 == 0) J = 29;
                else J = 28;
                partReglageHeure(3, &J, false);
              }
            }

            break;
        }
        // calcul du nombre de jour plein écoulés
        //res = 0;
        //for ( int8_t i = 0; i < A; ++i) res += ((i % 4 == 0) ? 366 : 365);
        //for ( int8_t i = 0; i < (M - 1); ++i) res += pgm_read_uint8_t(c_daysInMonth + i);
        //res += J - 1;

        // calcul du  nombre de seconde
        //res = ((res * 24 + h) * 60 + m) * 60;
        //dateTime = RtcDateTime(res);
        dateTime = RtcDateTime(A, M, J, h, m, 0);
        partReglageHeure( pos, val, true);
        //      } else if ( touche == BT_SELECT && (pos == -1 || pos == -2)) {
        //        break;
      } else if ( touche == BT_SELECT || touche == (BT_BAS | BT_HAUT) || ((touche&TIMEOUT) != 0)) {
        if ( touche == BT_SELECT && pos == 5) { // OK
          // Rtc.SetDateTime(dateTime);
          // lastDate = dateTime.TotalSeconds();

#ifdef DEBUG
          Serial.print("date=");
          Serial.print(dateTime.TotalSeconds(), HEX);
          Serial.print('(');
          Serial.print(dateTime.Day());
          Serial.print('/');
          Serial.print(dateTime.Month());
          Serial.print('/');
          Serial.print(dateTime.Year());
          Serial.print(' ');
          Serial.print(dateTime.Hour());
          Serial.print(':');
          Serial.print(dateTime.Minute());
          Serial.print(':');
          Serial.print(dateTime.Second());
          Serial.print(") mode=");
          Serial.print(mode, HEX);
          Serial.print(" repetition=");
          Serial.println(repetition);
#endif

#ifdef DEBUG
          Serial.print("date=");
          Serial.print(dateTime.TotalSeconds(), HEX);
          Serial.print('(');
          Serial.print(dateTime.Day());
          Serial.print('/');
          Serial.print(dateTime.Month());
          Serial.print('/');
          Serial.print(dateTime.Year());
          Serial.print(' ');
          Serial.print(dateTime.Hour());
          Serial.print(':');
          Serial.print(dateTime.Minute());
          Serial.print(':');
          Serial.print(dateTime.Second());
          Serial.println(")");
#endif
          return 1;
        } else { // annuler
          return 0;
        }
      } else if ( touche == BT_BAS || touche == BT_HAUT) { // chgt position
        partReglageHeure( pos, val, false);
        if ( touche == BT_BAS) {
          ++pos;
          if ( pos == -1 ) pos = 0;
          if ( pos > 6) pos = 0;
        } else {
          if ( pos == 0 ) pos = 6;
          else {
            --pos;
          }
        }

        switch (pos) {
          case 0:
            val = &h;
            break;
          case 1:
            val = &m;
            break;
          case 2:
            val = &J;
            break;
          case 3:
            val = &M;
            break;
          case 4:
            val = &A;
            break;
        }

        partReglageHeure( pos, val, true);

      }
    }
  }
}

template <typename T>
void CuvinoWidgets<T>::partReglageHeure(int8_t pos, uint8_t* val, bool surligne) {

  if (surligne)display->setTextColor(WHITE, BLACK);
  else display->setTextColor(BLACK, WHITE);

  switch (pos) {
    case 5: // ok
      display->setCursor(6, 40);
      display->printPGM(FR::TXT_OK);
      break;
    case 6: // retour
      display->setCursor(48, 40);
      display->printPGM(FR::TXT_RETOUR);
      break;
    case -3: // repetition
      display->setCursor(66, 8);
      if (*val != 0) display->printPGM(FR::TXT_OUI);
      else display->printPGM(FR::TXT_NON);
      break;
    case -10: // jour de la semaine
      display->setCursor(36, 32);
      display->printPGM(FR::JOURS[*val]);
      break;
    case -2: //mode
      display->setCursor(20, 17);
      display->setTextSize(0);
      switch (*val) {
        case MODE_PROG_HC_HP:
          display->printPGM(FR::TXT_PASSAGE_HC_HP, false, 8);
          break;
        case MODE_PROG_HP_HC:
          display->printPGM(FR::TXT_PASSAGE_HP_HC, false, 8);
          break;
        case MODE_PROG_10M:
        case MODE_PROG_H:
        case MODE_PROG_J:
        case MODE_PROG_S:
        case MODE_PROG_M:
          display->printPGM(CHAINE_MENU_PROGR_TL);
          display->setCursor(48, 17);
          switch (*val) {
            case MODE_PROG_10M:
              display->print(10);
              display->printPGM(FR::TXT_MINUTE);
              break;
            case MODE_PROG_H:
              display->printPGM(FR::TXT_HEURE);
              break;
            case MODE_PROG_J:
              display->printPGM(FR::TXT_JOUR);
              break;
            case MODE_PROG_S:
              display->printPGM(FR::TXT_SEMAINE);
              break;
            case MODE_PROG_M:
              display->printPGM(FR::TXT_MOIS);
              break;
          }
          if ( *val != MODE_PROG_M) display->write('s');
          break;
      }
      break;
    default:
      int8_t x, y;
      switch (pos) {
        case -1: // nb de repetition
          //          x = 66;
          //          y = 32;
          display->setTextSize(0);
          x = 40;
          y = 17;
          break;
        case 0: // heures
        case 1: // minutes
          y = 16;
          x = (pos == 0) ? 30 : 48;
          break;
        case 2: // jour
        case 3: // mois
        case 4: // année
          y = 24;
          x = -6 + 18 * pos;
          break;
        default:
          x=100;
          y=100;
      }
      display->setCursor(x, y);
      display->print2digits(*val);
  }
  if (surligne)display->setTextColor(BLACK, WHITE);
  display->setTextSize(1);
  display->display();
}

template <typename T>
void CuvinoWidgets<T>::partReglageProgramme(int8_t pos, uint8_t* val, bool surligne) {

  if (surligne)display->setTextColor(WHITE, BLACK);
  else display->setTextColor(BLACK, WHITE);

  switch (pos) {
    case 5: // ok
      display->setCursor(6, 40);
      display->printPGM(FR::TXT_OK);
      break;
    case 6: // retour
      display->setCursor(48, 40);
      display->printPGM(FR::TXT_RETOUR);
      break;
    case -3: // repetition
      display->setCursor(66, 8);
      if (*val != 0) display->printPGM(FR::TXT_OUI);
      else display->printPGM(FR::TXT_NON);
      break;
    case -10: // jour de la semaine
      display->setCursor(36, 32);
      display->printPGM(FR::JOURS[*val]);
      break;
    case -2: //mode
      display->setCursor(20, 17);
      display->setTextSize(0);
      switch (*val) {
        case MODE_PROG_HC_HP:
          display->printPGM(FR::TXT_PASSAGE_HC_HP, false, 8);
          break;
        case MODE_PROG_HP_HC:
          display->printPGM(FR::TXT_PASSAGE_HP_HC, false, 8);
          break;
        case MODE_PROG_10M:
        case MODE_PROG_H:
        case MODE_PROG_J:
        case MODE_PROG_S:
        case MODE_PROG_M:
          display->printPGM(CHAINE_MENU_PROGR_TL);
          display->setCursor(48, 17);
          switch (*val) {
            case MODE_PROG_10M:
              display->print(10);
              display->printPGM(FR::TXT_MINUTE);
              break;
            case MODE_PROG_H:
              display->printPGM(FR::TXT_HEURE);
              break;
            case MODE_PROG_J:
              display->printPGM(FR::TXT_JOUR);
              break;
            case MODE_PROG_S:
              display->printPGM(FR::TXT_SEMAINE);
              break;
            case MODE_PROG_M:
              display->printPGM(FR::TXT_MOIS);
              break;
          }
          if ( *val != MODE_PROG_M) display->write('s');
          break;
      }
      break;
    default:
      int8_t x, y;
      switch (pos) {
        case -1: // nb de repetition
          //          x = 66;
          //          y = 32;
          display->setTextSize(0);
          x = 40;
          y = 17;
          break;
        case 0: // heures
        case 1: // minutes
          y = 24;
          x = (pos == 0) ? 30 : 48;
          break;
        case 2: // jour
        case 3: // mois
        case 4: // année
          y =  32;
          x = -6 + 18 * pos + 6;
          break;
        default:
          x=100;
          y=100;
      }
      display->setCursor(x, y);
      display->print2digits(*val);
  }
  if (surligne)display->setTextColor(BLACK, WHITE);
  display->setTextSize(1);
  display->display();
}


#endif
