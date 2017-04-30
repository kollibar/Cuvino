#include "IO_PCD8544_3touches_FreeRTOS.h"



IO_PCD8544_3touches_FreeRTOS::IO_PCD8544_3touches_FreeRTOS(int8_t SCLK, int8_t DIN, int8_t DC,
  int8_t CS, int8_t RST) : Adafruit_PCD8544(SCLK, DIN, DC, CS, RST), Clavier_3T_Analog_FreeRTOS( _fausseUpdate){
    //widgets=Widgets(this);
    return;
}

IO_PCD8544_3touches_FreeRTOS::IO_PCD8544_3touches_FreeRTOS(int8_t SCLK, int8_t DIN, int8_t DC,
  int8_t RST) : Adafruit_PCD8544(SCLK, DIN, DC, RST), Clavier_3T_Analog_FreeRTOS(_fausseUpdate) {
    //widgets=Widgets(this);
    return;
}

IO_PCD8544_3touches_FreeRTOS::IO_PCD8544_3touches_FreeRTOS(int8_t DC, int8_t CS,
  int8_t RST): Adafruit_PCD8544(DC, CS, RST), Clavier_3T_Analog_FreeRTOS(_fausseUpdate){
    //widgets=Widgets(this);
    return;
}

IO_PCD8544_3touches_FreeRTOS::IO_PCD8544_3touches_FreeRTOS(int8_t SCLK, int8_t DIN, int8_t DC,
  int8_t CS, int8_t RST,void(*_fct_update)(void)) : Adafruit_PCD8544(SCLK, DIN, DC, CS, RST), Clavier_3T_Analog_FreeRTOS(_fct_update) {
    //widgets=Widgets(this);
    return;
}

IO_PCD8544_3touches_FreeRTOS::IO_PCD8544_3touches_FreeRTOS(int8_t SCLK, int8_t DIN, int8_t DC,
  int8_t RST,void(*_fct_update)(void)) : Adafruit_PCD8544(SCLK, DIN, DC, RST), Clavier_3T_Analog_FreeRTOS(_fct_update) {
    //widgets=Widgets(this);
    return;
}

IO_PCD8544_3touches_FreeRTOS::IO_PCD8544_3touches_FreeRTOS(int8_t DC, int8_t CS, int8_t RST,
  void(*_fct_update)(void)): Adafruit_PCD8544(DC, CS, RST), Clavier_3T_Analog_FreeRTOS(_fct_update) {
    //widgets=Widgets(this);
    return;
}

void IO_PCD8544_3touches_FreeRTOS::display(void){
  taskENTER_CRITICAL();
    Adafruit_PCD8544::display();
  taskEXIT_CRITICAL();
}

void IO_PCD8544_3touches_FreeRTOS::command(uint8_t c){
  taskENTER_CRITICAL();
    Adafruit_PCD8544::command(c);
  taskEXIT_CRITICAL();
}
void IO_PCD8544_3touches_FreeRTOS::data(uint8_t c){
  taskENTER_CRITICAL();
    Adafruit_PCD8544::data(c);
  taskEXIT_CRITICAL();
}

void IO_PCD8544_3touches_FreeRTOS::setUpdateFct(FonctionUpdate _fctUpdate){
  fctUpdate=_fctUpdate;
}

void IO_PCD8544_3touches_FreeRTOS::onOffEcran(bool etat){
  #ifdef PORT_LED_ECRAN
  if(etat) PORT_LED_ECRAN |= (1 << BIT_LED_ECRAN); // on ralume l'écran
  else PORT_LED_ECRAN &= ~(1 << BIT_LED_ECRAN); // éteint l'écran
  #else
  if( _led !=0 ){
    if(etat) digitalWrite(_led,HIGH);
    else digitalWrite(_led, LOW);
    _etatEcran=etat;
  }
  #endif
  return;
}

bool IO_PCD8544_3touches_FreeRTOS::etatEcran(void){
  #ifdef PORT_LED_ECRAN
    return ((PORT_LED_ECRAN&(1<<BIT_LED_ECRAN)) != 0);
  #else
    return _etatEcran;
    return (bool)digitalRead(_led);
  #endif
}

void IO_PCD8544_3touches_FreeRTOS::begin(uint8_t contrast, uint8_t bias){
  Adafruit_PCD8544::begin(contrast, bias);
  Clavier_3T_Analog_FreeRTOS::begin();
  #ifdef PORT_LED_ECRAN
    DDR_LED_ECRAN |= (1 << BIT_LED_ECRAN); // met la broche de la led de l'ecran en mode out
    PORT_LED_ECRAN |= (1 << BIT_LED_ECRAN); // allume la led
  #else
    if( _led >=0) pinMode(_led, OUTPUT);
    digitalWrite(_led,HIGH);
    _etatEcran=true;
  #endif
}

IO_PCD8544_3touches_SPI_software_FreeRTOS::IO_PCD8544_3touches_SPI_software_FreeRTOS(int8_t SCLK, int8_t DIN, int8_t DC,
  int8_t CS, int8_t RST) : PCD8544_SPI_Software(SCLK, DIN, DC, CS, RST), Clavier_3T_Analog_FreeRTOS(_fausseUpdate){
    //widgets=Widgets(this);
    return;
}

IO_PCD8544_3touches_SPI_software_FreeRTOS::IO_PCD8544_3touches_SPI_software_FreeRTOS(int8_t SCLK, int8_t DIN, int8_t DC,
  int8_t RST) : PCD8544_SPI_Software(SCLK, DIN, DC, RST), Clavier_3T_Analog_FreeRTOS(_fausseUpdate) {
    //widgets=Widgets(this);
    return;
}


IO_PCD8544_3touches_SPI_software_FreeRTOS::IO_PCD8544_3touches_SPI_software_FreeRTOS(int8_t SCLK, int8_t DIN, int8_t DC,
  int8_t CS, int8_t RST,void(*_fct_update)(void)) : PCD8544_SPI_Software(SCLK, DIN, DC, CS, RST), Clavier_3T_Analog_FreeRTOS(_fct_update) {
    //widgets=Widgets(this);
    return;
}

IO_PCD8544_3touches_SPI_software_FreeRTOS::IO_PCD8544_3touches_SPI_software_FreeRTOS(int8_t SCLK, int8_t DIN, int8_t DC,
  int8_t RST,void(*_fct_update)(void)) : PCD8544_SPI_Software(SCLK, DIN, DC, RST), Clavier_3T_Analog_FreeRTOS(_fct_update) {
    //widgets=Widgets(this);
    return;
}

void IO_PCD8544_3touches_SPI_software_FreeRTOS::display(void){
  taskENTER_CRITICAL();
    Adafruit_PCD8544::display();
  taskEXIT_CRITICAL();
}

void IO_PCD8544_3touches_SPI_software_FreeRTOS::command(uint8_t c){
  taskENTER_CRITICAL();
    Adafruit_PCD8544::command(c);
  taskEXIT_CRITICAL();
}
void IO_PCD8544_3touches_SPI_software_FreeRTOS::data(uint8_t c){
  taskENTER_CRITICAL();
    Adafruit_PCD8544::data(c);
  taskEXIT_CRITICAL();
}

void IO_PCD8544_3touches_SPI_software_FreeRTOS::setUpdateFct(FonctionUpdate _fctUpdate){
  fctUpdate=_fctUpdate;
}

void IO_PCD8544_3touches_SPI_software_FreeRTOS::onOffEcran(bool etat){
  #ifdef PORT_LED_ECRAN
  if(etat) PORT_LED_ECRAN |= (1 << BIT_LED_ECRAN); // on ralume l'écran
  else PORT_LED_ECRAN &= ~(1 << BIT_LED_ECRAN); // éteint l'écran
  #else
  if( _led !=0 ){
    if(etat) digitalWrite(_led,HIGH);
    else digitalWrite(_led, LOW);
    _etatEcran=etat;
  }
  #endif
  return;
}
bool IO_PCD8544_3touches_SPI_software_FreeRTOS::etatEcran(void){
  #ifdef PORT_LED_ECRAN
    return ((PORT_LED_ECRAN&(1<<BIT_LED_ECRAN)) != 0);
  #else
    return _etatEcran;
    return (bool)digitalRead(_led);
    _etatEcran=true;
  #endif
}

void IO_PCD8544_3touches_SPI_software_FreeRTOS::begin(uint8_t contrast, uint8_t bias){
  PCD8544_SPI_Software::begin(contrast, bias);
  Clavier_3T_Analog_FreeRTOS::begin();
  #ifdef PORT_LED_ECRAN
    DDR_LED_ECRAN |= (1 << BIT_LED_ECRAN); // met la broche de la led de l'ecran en mode out
    PORT_LED_ECRAN |= (1 << BIT_LED_ECRAN); // allume la led
  #else
    if( _led >=0) pinMode(_led, OUTPUT);
    digitalWrite(_led,HIGH);
  #endif
}

/*
// widgets supplémentaires
retourSelect IO_PCD8544_3touches_FreeRTOS::menuListeOld(char* chaine, uint8_t nb, uint8_t taille, uint8_t yMin, uint8_t yMax, uint8_t xMin, uint8_t xMax, uint8_t hLigne, uint8_t depart, uint8_t textSize, uint8_t actual, uint8_t permissions) {
//      affiche une liste est retourne le numéro de l'item selectionné
//      liste est un tableau de pointeur vers des chaine comprenant la chaine de caractère de l'item
//      nb est le nombre d'élément dans la liste principale
//      taille donne la taille des élements de la chaine /!\ y compris le bit de fin de chaine /!\.
//      yMin est le point de départ de la liste en graphique (par défaut 0)
//      yMax est le bas de la liste (par défaut bas de l'écran)
//      xMin ... idem en X
//      xMax ... idem en X
//      hLigne est la hauteur de la ligne en nb de caractère (càd 1 car = 8 pixels de haut), par defaut 1
//      permissions indique les permissions pour quitter le menu (autre que sélection), par défaut TIMEOUT_LED & BT_ESC
//
//      /!\ si plusieurs lignes pour une chaine (càd hLigne!=0) compter un caractère en plus pour chaque ligne pour ajouter une fin de ligne

#ifdef DEBUG
  Serial.print("menuListe(...,");
  Serial.print(nb);
  Serial.print(',');
  Serial.print(taille);
  Serial.print(',');
  Serial.print(yMin);
  Serial.print(',');
  Serial.print(yMax);
  Serial.print(',');
  Serial.print(xMin);
  Serial.print(',');
  Serial.print(xMax);
  Serial.print(',');
  Serial.print(hLigne);
  Serial.print(',');
  Serial.print(permissions);
  Serial.println(")");
  Serial.print("chaine=[");
  for (uint8_t i = 0; i < nb; i++) {
    Serial.print((char*)&chaine[i * taille]);
    Serial.print(",");
  }
  Serial.println("]");
#endif

  uint8_t tailleLigne;
  uint8_t largCarac = (textSize == 0) ? 4 : (6 * textSize);
  uint8_t hautCarac = (textSize == 0) ? 6 : (8 * textSize);

  taille = min(taille, ((xMax - xMin)  / largCarac + 1) * hLigne); // taille de la ligne en caractères
  if (hLigne == 1)tailleLigne = taille;
  else tailleLigne = (xMax - xMin) / largCarac + 1;
#ifdef DEBUG
  Serial.print("taille:");
  Serial.print(taille);
#endif

  uint8_t touche, pos, deb;
  uint8_t nbLigneVisible = min(((yMax - yMin) / (hautCarac * hLigne) ), nb); // nombre de ligne visible à la fois
  uint8_t derniereLigne = (((yMax - yMin) % (hautCarac * hLigne)  == 0) || (nb == nbLigneVisible)) ? 0 : 1;
  permissions &= 0b11110000; // pas de permissions pour les 4 1ere touche ( BT_SELECT,BT_HAUT,BT_BAS)
  uint16_t colorT = BLACK, colorF = WHITE;

  if ( actual == 0) {
    // remise en forme de la liste (seulement si pas d'actualisation)

    for (uint8_t i = 0; i < nb; ++i) { // parcours de tous les éléments de la liste
      uint8_t j = 0;
      chaine[(i + 1) * taille - 1] = 0; // ajout d'un caractère fin de chaine à la fin
      while (chaine[i * taille + j] != 0) ++j;
      while (j < (taille - 1)) { // complément de la chaine de car par des espace
        chaine[i * taille + j] = ' ';
        ++j;
      }
      if (hLigne > 1) { // insertion de tous les caractères fin de ligne
        uint8_t dec = hLigne - 1;
        j = taille - 2;
        do {
          if ((j + dec) % tailleLigne == 0) {
            dec--;
            chaine[i * taille + j] = 0;
            if (dec == 0) break;
          }
          chaine[i * taille + j] = chaine[i * taille + j - dec];
          j--;
        } while (1);
      }
    }
  }
#ifdef DEBUG
  Serial.print("chaine=[");
  for (uint8_t i = 0; i < nb; i++) {
    Serial.print((char*)&chaine[i * taille]);
    Serial.print(",");
  }
  Serial.println("]");
#endif
  // ajout depart
  if (depart < nbLigneVisible) {
    pos = 0;
    deb = depart;
  } else if ( depart <= nb - nbLigneVisible) {
    pos = 1;
    deb = depart - 1;
  } else {
    deb = nb - nbLigneVisible;
    pos = depart - deb;
  }
  setTextSize(textSize);
  for (;;) { // boucle infini complète
    // efface la zone d'affichage
    fillRect(xMin, yMin, xMax, yMax, WHITE);
    for (uint8_t i = 0; i < (nbLigneVisible + derniereLigne); ++i) {

      for (uint8_t j = 0; j < hLigne; ++j) {
        setCursor(xMin, yMin + (i * hLigne + j) * hautCarac);
        uint8_t k = (i + deb) * taille + j * tailleLigne;
        uint8_t hexa = 0;
        char c = chaine[k];
        while (c != 0 || hexa != 0 ) {
          if ( c == -1) {
            ++k;
            hexa = chaine[k];
          } else {
            if ( hexa != 0) {
              if ( (uint8_t)c < 16) print(0);
              print((uint8_t)c, HEX);
              --hexa;
            } else {
              print(c);
            }
          }
          ++k;
          c = chaine[k];
        }
      }
    }
    invertRect(xMin, yMin + (pos * hLigne) * hautCarac, xMax - xMin, hLigne * hautCarac);

    if (deb + nbLigneVisible == nb) fillRect(xMin, yMin + nbLigneVisible * hautCarac * hLigne, largCarac * (taille - 1), yMax - yMin - nbLigneVisible * hautCarac * hLigne, colorF);
    display();
    for (;;) { // boucle infini gestion touche
#ifdef DEBUG
      Serial.print("deb:");
      Serial.print(deb);
      Serial.print("     pos:");
      Serial.print(pos);
      Serial.print("     choix:");
      Serial.print((char*)&chaine[(pos + deb) * taille]);
      Serial.print("     nb:");
      Serial.print(nb);
      Serial.print("     nbLigneVisible:");
      Serial.println(nbLigneVisible);
#endif
      aucuneTouche();
      if ( actual == 0) touche = getTouche(-1); // cas normal, pas d'actualisation
      else {
        touche = getTouche(actual);
        if ( touche == TIMEOUT_LED  && millis() - getDateDerniereTouche() < 60000 ) {
          touche = 0; // si TIMEOUT_LED mais qu'on n'a pas atteint l'extinction de l'ecran -> touche = 0
          break;
        }
      }
      if ( ((touche == TIMEOUT_LED) && ((permissions & TIMEOUT_LED) != 0)) || ((touche == (BT_HAUT | BT_BAS)) && ((permissions & BT_ESC) != 0)) || ((touche == (BT_HAUT | BT_BAS | BT_SELECT)) && ((permissions & BT_MENU) != 0)) || (touche == BT_SELECT)) break;

      if (touche == BT_BAS || touche == BT_HAUT) {
        invertRect(xMin, yMin + (pos * hLigne) * hautCarac, xMax - xMin, hLigne * hautCarac);
        if (touche == BT_BAS) {
          if ((pos == nbLigneVisible - 1) || (((deb + nbLigneVisible) != nb) && pos == nbLigneVisible - 2)) break;
          ++pos;
        } else { // touche == BT_HAUT
          if  ((pos == 0) || ((deb != 0 && pos == 1))) break;
          --pos;
        }
        invertRect(xMin, yMin + (pos * hLigne) * hautCarac, xMax - xMin, hLigne * hautCarac);
        display();
      }
    } // fin boucle infini gestion touche
    if ( touche != 0) {
      if (touche != BT_BAS && touche != BT_HAUT) break;
      if (touche == BT_BAS) {
        if (pos == nbLigneVisible - 1) {
          pos = 0;
          deb = 0;
        } else ++deb;
      } else { // touche BT_HAUT
        if (pos == 0) {
          pos = nbLigneVisible - 1;
          deb = (nbLigneVisible < nb) ? nb - nbLigneVisible : 0;
        }
        else --deb;
      }
    }
  } // fin boucle infinie complète
  if (touche == 0) touche = TIMEOUT_LED;
  if (touche == BT_SELECT) touche = 0;
  invertRect(xMin, yMin + (pos * hLigne) * hautCarac, xMax - xMin, hLigne * hautCarac);
  display();

  retourSelect result;
  result.result = pos + deb;
  result.touche = touche;
  setTextSize(1);
  return result;
}

retourSelect IO_PCD8544_3touches_FreeRTOS::menuListe(char* chaine, uint8_t nb, uint8_t taille, uint8_t yMin, uint8_t yMax, uint8_t xMin, uint8_t xMax, uint8_t hLigne, uint8_t depart, uint8_t textSize, uint8_t actual, uint8_t permissions) {
  //      affiche une liste est retourne le numéro de l'item selectionné
  //      liste est un tableau de pointeur vers des chaine comprenant la chaine de caractère de l'item
  //      nb est le nombre d'élément dans la liste principale
  //      taille donne la taille des élements de la chaine /!\ y compris le bit de fin de chaine /!\.
  //      yMin est le point de départ de la liste en graphique (par défaut 0)
  //      yMax est le bas de la liste (par défaut bas de l'écran)
  //      xMin ... idem en X
  //      xMax ... idem en X
  //      hLigne est la hauteur de la ligne en nb de caractère (càd 1 car = 8 pixels de haut), par defaut 1
  //      permissions indique les permissions pour quitter le menu (autre que sélection), par défaut TIMEOUT_LED & BT_ESC
  //
  //      /!\ si plusieurs lignes pour une chaine (càd hLigne!=0) compter un caractère en plus pour chaque ligne pour ajouter une fin de ligne

  return menuListeV2(_creeLigneMenuListeV1, (void*)chaine, nb, taille, yMin, yMax, xMin, xMax, hLigne, depart, textSize, actual, permissions);
}



retourSelect IO_PCD8544_3touches_FreeRTOS::menuListeV2(void (*callback)(uint8_t, char*, uint8_t, void*), void* arg, uint8_t nb, uint8_t taille, uint8_t yMin, uint8_t yMax, uint8_t xMin, uint8_t xMax, uint8_t hLigne, uint8_t depart, uint8_t textSize, uint8_t actual, uint8_t permissions) {
  //      affiche une liste est retourne le numéro de l'item selectionné
  //      liste est un tableau de pointeur vers des chaine comprenant la chaine de caractère de l'item
  //      nb est le nombre d'élément dans la liste principale
  //      taille donne la taille des élements de la chaine /!\ y compris le bit de fin de chaine /!\.
  //      yMin est le point de départ de la liste en graphique (par défaut 0)
  //      yMax est le bas de la liste (par défaut bas de l'écran)
  //      xMin ... idem en X
  //      xMax ... idem en X
  //      hLigne est la hauteur de la ligne en nb de caractère (càd 1 car = 8 pixels de haut), par defaut 1
  //      permissions indique les permissions pour quitter le menu (autre que sélection), par défaut TIMEOUT_LED & BT_ESC
  //
  //      /!\ si plusieurs lignes pour une chaine (càd hLigne!=0) compter un caractère en plus pour chaque ligne pour ajouter une fin de ligne

#ifdef DEBUG
  Serial.print("menuListeV2(callback@");
  Serial.print((uint16_t)callback, HEX);
  Serial.print(", arg@");
  Serial.print((uint16_t)arg, HEX);
  Serial.print(", nb=");
  Serial.print(nb);
  Serial.print(", taille=");
  Serial.print(taille);
  Serial.print(", yMin=");
  Serial.print(yMin);
  Serial.print(", yMax=");
  Serial.print(yMax);
  Serial.print(", xMin=");
  Serial.print(xMin);
  Serial.print(", xMax=");
  Serial.print(xMax);
  Serial.print(", hLigne=");
  Serial.print(hLigne);
  Serial.print(", depart=");
  Serial.print(depart);
  Serial.print(", textSize=");
  Serial.print(textSize);
  Serial.print(", actual=");
  Serial.print(actual);
  Serial.print(", permissions=0b");
  Serial.print(permissions, BIN);
  Serial.println(");");
#endif

  //uint8_t tailleLigne;
  uint8_t largCarac = (textSize == 0) ? 4 : (6 * textSize);
  uint8_t hautCarac = (textSize == 0) ? 6 : (8 * textSize);

  taille = min(taille, ((xMax - xMin)  / largCarac + 1) * hLigne); // taille de la ligne en caractères
  //if (hLigne == 1)tailleLigne = taille;
  //else tailleLigne = (xMax - xMin) / largCarac + 1;
#ifdef DEBUG
  Serial.print("taille:");
  Serial.print(taille);
#endif

  uint8_t touche, pos, deb;
  uint8_t nbLigneVisible = min(((yMax - yMin) / (hautCarac * hLigne) ), nb); // nombre de ligne visible à la fois
  uint8_t derniereLigne = (((yMax - yMin) % (hautCarac * hLigne)  == 0) || (nb == nbLigneVisible)) ? 0 : 1;
  permissions &= 0b11110000; // pas de permissions pour les 4 1ere touche ( BT_SELECT,BT_HAUT,BT_BAS)
  uint16_t colorT = BLACK, colorF = WHITE;
  char chaine[22];


  // ajout depart
  if (depart == 0) {
    pos = 0;
    deb = depart;
  } else if ( depart <= nb - nbLigneVisible) {
    pos = 1;
    deb = depart - 1;
  } else {
    deb = nb - nbLigneVisible;
    pos = depart - deb;
  }
  setTextSize(textSize);
  for (;;) { // boucle infini complète
    // efface la zone d'affichage
    fillRect(xMin, yMin, xMax, yMax, WHITE);
    for (uint8_t i = 0; i < (nbLigneVisible + derniereLigne); ++i) {

      callback(i + deb, chaine, taille, arg);

      for (uint8_t j = 0; j < hLigne; ++j) {
        setCursor(xMin, yMin + (i * hLigne + j) * hautCarac);
        uint8_t hexa = 0;
        for ( uint8_t k = 0; k < taille && k < 22; ++k) {
          char c = chaine[k];
          if ( c == 0 && hexa == 0) break;
          if ( c == -1) {
            ++k;
            hexa = chaine[k];
          } else {
            if ( hexa != 0) {
              if ( (uint8_t)c < 16) print(0);
              print((uint8_t)c, HEX);
              --hexa;
            } else {
              print(c);
            }
          }
        }
      }
    }
    invertRect(xMin, yMin + (pos * hLigne) * hautCarac, xMax - xMin, hLigne * hautCarac);

    if (deb + nbLigneVisible == nb) fillRect(xMin, yMin + nbLigneVisible * hautCarac * hLigne, largCarac * (taille - 1), yMax - yMin - nbLigneVisible * hautCarac * hLigne, colorF);
    display();
    for (;;) { // boucle infini gestion touche
      aucuneTouche();
      if ( actual == 0) touche = getTouche(-1); // cas normal, pas d'actualisation
      else {
        touche = getTouche(actual);
        if ( touche == TIMEOUT_LED  && millis() - getDateDerniereTouche() < 60000 ) {
          touche = 0; // si TIMEOUT_LED mais qu'on n'a pas atteint l'extinction de l'ecran -> touche = 0
          break;
        }
      }
      if ( ((touche == TIMEOUT_LED) && ((permissions & TIMEOUT_LED) != 0)) || ((touche == (BT_HAUT | BT_BAS)) && ((permissions & BT_ESC) != 0)) || ((touche == (BT_HAUT | BT_BAS | BT_SELECT)) && ((permissions & BT_MENU) != 0)) || (touche == BT_SELECT)) break;

      if (touche == BT_BAS || touche == BT_HAUT) {
        invertRect(xMin, yMin + (pos * hLigne) * hautCarac, xMax - xMin, hLigne * hautCarac);
        if (touche == BT_BAS) {
          if ((pos == nbLigneVisible - 1) || (((deb + nbLigneVisible) != nb) && pos == nbLigneVisible - 2)) break;
          ++pos;
        } else { // touche == BT_HAUT
          if  ((pos == 0) || ((deb != 0 && pos == 1))) break;
          --pos;
        }
        invertRect(xMin, yMin + (pos * hLigne) * hautCarac, xMax - xMin, hLigne * hautCarac);
        display();
      }
    } // fin boucle infini gestion touche
    if ( touche != 0) {
      if (touche != BT_BAS && touche != BT_HAUT) break;
      if (touche == BT_BAS) {
        if (pos == nbLigneVisible - 1) {
          pos = 0;
          deb = 0;
        } else ++deb;
      } else { // touche BT_HAUT
        if (pos == 0) {
          pos = nbLigneVisible - 1;
          deb = (nbLigneVisible < nb) ? nb - nbLigneVisible : 0;
        }
        else --deb;
      }
    }
  } // fin boucle infinie complète
  if (touche == 0) touche = TIMEOUT_LED;
  if (touche == BT_SELECT) touche = 0;
  invertRect(xMin, yMin + (pos * hLigne) * hautCarac, xMax - xMin, hLigne * hautCarac);
  display();

  retourSelect result;
  result.result = pos + deb;
  result.touche = touche;
  setTextSize(1);
  return result;
}



retourSelect IO_PCD8544_3touches_FreeRTOS::menuListeOuiNon(uint8_t yMin, uint8_t yMax, uint8_t xMin, uint8_t xMax, uint8_t hLigne, uint8_t depart, uint8_t textSize, uint8_t permissions) {
  return menuListeV2(_creeLigneMenuListeOuiNon, (void*)&yMin, 2, 4, yMin, yMax, xMin, xMax, hLigne, depart, textSize, 0, permissions);
}


void _creeLigneMenuListeV1(uint8_t i, char* chaine, uint8_t taille, void* arg) {
  char* chaineCom = (char*)arg;
  strcpy(chaine, &chaineCom[i * taille]);
  return;
}

void _creeLigneMenuListeOuiNon(uint8_t i, char* chaine, uint8_t taille, void* arg) {
  switch (i) {
    case 0:
      //strcpy_P(chaine, TXT_OUI);
      strcpy(chaine,"OUI");
      break;
    case 1:
      //strcpy_P(chaine, TXT_NON);
      strcpy(chaine,"NON");
      break;
    default:
      break;
  }
}


retourSelect IO_PCD8544_3touches_SPI_software_FreeRTOS::menuListeOld(char* chaine, uint8_t nb, uint8_t taille, uint8_t yMin, uint8_t yMax, uint8_t xMin, uint8_t xMax, uint8_t hLigne, uint8_t depart, uint8_t textSize, uint8_t actual, uint8_t permissions) {
  //      affiche une liste est retourne le numéro de l'item selectionné
  //      liste est un tableau de pointeur vers des chaine comprenant la chaine de caractère de l'item
  //      nb est le nombre d'élément dans la liste principale
  //      taille donne la taille des élements de la chaine /!\ y compris le bit de fin de chaine /!\.
  //      yMin est le point de départ de la liste en graphique (par défaut 0)
  //      yMax est le bas de la liste (par défaut bas de l'écran)
  //      xMin ... idem en X
  //      xMax ... idem en X
  //      hLigne est la hauteur de la ligne en nb de caractère (càd 1 car = 8 pixels de haut), par defaut 1
  //      permissions indique les permissions pour quitter le menu (autre que sélection), par défaut TIMEOUT_LED & BT_ESC
  //
  //      /!\ si plusieurs lignes pour une chaine (càd hLigne!=0) compter un caractère en plus pour chaque ligne pour ajouter une fin de ligne

#ifdef DEBUG
  Serial.print("menuListe(...,");
  Serial.print(nb);
  Serial.print(',');
  Serial.print(taille);
  Serial.print(',');
  Serial.print(yMin);
  Serial.print(',');
  Serial.print(yMax);
  Serial.print(',');
  Serial.print(xMin);
  Serial.print(',');
  Serial.print(xMax);
  Serial.print(',');
  Serial.print(hLigne);
  Serial.print(',');
  Serial.print(permissions);
  Serial.println(")");
  Serial.print("chaine=[");
  for (uint8_t i = 0; i < nb; i++) {
    Serial.print((char*)&chaine[i * taille]);
    Serial.print(",");
  }
  Serial.println("]");
#endif

  uint8_t tailleLigne;
  uint8_t largCarac = (textSize == 0) ? 4 : (6 * textSize);
  uint8_t hautCarac = (textSize == 0) ? 6 : (8 * textSize);

  taille = min(taille, ((xMax - xMin)  / largCarac + 1) * hLigne); // taille de la ligne en caractères
  if (hLigne == 1)tailleLigne = taille;
  else tailleLigne = (xMax - xMin) / largCarac + 1;
#ifdef DEBUG
  Serial.print("taille:");
  Serial.print(taille);
#endif

  uint8_t touche, pos, deb;
  uint8_t nbLigneVisible = min(((yMax - yMin) / (hautCarac * hLigne) ), nb); // nombre de ligne visible à la fois
  uint8_t derniereLigne = (((yMax - yMin) % (hautCarac * hLigne)  == 0) || (nb == nbLigneVisible)) ? 0 : 1;
  permissions &= 0b11110000; // pas de permissions pour les 4 1ere touche ( BT_SELECT,BT_HAUT,BT_BAS)
  uint16_t colorT = BLACK, colorF = WHITE;

  if ( actual == 0) {
    // remise en forme de la liste (seulement si pas d'actualisation)

    for (uint8_t i = 0; i < nb; ++i) { // parcours de tous les éléments de la liste
      uint8_t j = 0;
      chaine[(i + 1) * taille - 1] = 0; // ajout d'un caractère fin de chaine à la fin
      while (chaine[i * taille + j] != 0) ++j;
      while (j < (taille - 1)) { // complément de la chaine de car par des espace
        chaine[i * taille + j] = ' ';
        ++j;
      }
      if (hLigne > 1) { // insertion de tous les caractères fin de ligne
        uint8_t dec = hLigne - 1;
        j = taille - 2;
        do {
          if ((j + dec) % tailleLigne == 0) {
            dec--;
            chaine[i * taille + j] = 0;
            if (dec == 0) break;
          }
          chaine[i * taille + j] = chaine[i * taille + j - dec];
          j--;
        } while (1);
      }
    }
  }
#ifdef DEBUG
  Serial.print("chaine=[");
  for (uint8_t i = 0; i < nb; i++) {
    Serial.print((char*)&chaine[i * taille]);
    Serial.print(",");
  }
  Serial.println("]");
#endif
  // ajout depart
  if (depart < nbLigneVisible) {
    pos = 0;
    deb = depart;
  } else if ( depart <= nb - nbLigneVisible) {
    pos = 1;
    deb = depart - 1;
  } else {
    deb = nb - nbLigneVisible;
    pos = depart - deb;
  }
  setTextSize(textSize);
  for (;;) { // boucle infini complète
    // efface la zone d'affichage
    fillRect(xMin, yMin, xMax, yMax, WHITE);
    for (uint8_t i = 0; i < (nbLigneVisible + derniereLigne); ++i) {

      for (uint8_t j = 0; j < hLigne; ++j) {
        setCursor(xMin, yMin + (i * hLigne + j) * hautCarac);
        uint8_t k = (i + deb) * taille + j * tailleLigne;
        uint8_t hexa = 0;
        char c = chaine[k];
        while (c != 0 || hexa != 0 ) {
          if ( c == -1) {
            ++k;
            hexa = chaine[k];
          } else {
            if ( hexa != 0) {
              if ( (uint8_t)c < 16) print(0);
              print((uint8_t)c, HEX);
              --hexa;
            } else {
              print(c);
            }
          }
          ++k;
          c = chaine[k];
        }
      }
    }
    invertRect(xMin, yMin + (pos * hLigne) * hautCarac, xMax - xMin, hLigne * hautCarac);

    if (deb + nbLigneVisible == nb) fillRect(xMin, yMin + nbLigneVisible * hautCarac * hLigne, largCarac * (taille - 1), yMax - yMin - nbLigneVisible * hautCarac * hLigne, colorF);
    display();
    for (;;) { // boucle infini gestion touche
#ifdef DEBUG
      Serial.print("deb:");
      Serial.print(deb);
      Serial.print("     pos:");
      Serial.print(pos);
      Serial.print("     choix:");
      Serial.print((char*)&chaine[(pos + deb) * taille]);
      Serial.print("     nb:");
      Serial.print(nb);
      Serial.print("     nbLigneVisible:");
      Serial.println(nbLigneVisible);
#endif
      aucuneTouche();
      if ( actual == 0) touche = getTouche(-1); // cas normal, pas d'actualisation
      else {
        touche = getTouche(actual);
        if ( touche == TIMEOUT_LED  && millis() - getDateDerniereTouche() < 60000 ) {
          touche = 0; // si TIMEOUT_LED mais qu'on n'a pas atteint l'extinction de l'ecran -> touche = 0
          break;
        }
      }
      if ( ((touche == TIMEOUT_LED) && ((permissions & TIMEOUT_LED) != 0)) || ((touche == (BT_HAUT | BT_BAS)) && ((permissions & BT_ESC) != 0)) || ((touche == (BT_HAUT | BT_BAS | BT_SELECT)) && ((permissions & BT_MENU) != 0)) || (touche == BT_SELECT)) break;

      if (touche == BT_BAS || touche == BT_HAUT) {
        invertRect(xMin, yMin + (pos * hLigne) * hautCarac, xMax - xMin, hLigne * hautCarac);
        if (touche == BT_BAS) {
          if ((pos == nbLigneVisible - 1) || (((deb + nbLigneVisible) != nb) && pos == nbLigneVisible - 2)) break;
          ++pos;
        } else { // touche == BT_HAUT
          if  ((pos == 0) || ((deb != 0 && pos == 1))) break;
          --pos;
        }
        invertRect(xMin, yMin + (pos * hLigne) * hautCarac, xMax - xMin, hLigne * hautCarac);
        display();
      }
    } // fin boucle infini gestion touche
    if ( touche != 0) {
      if (touche != BT_BAS && touche != BT_HAUT) break;
      if (touche == BT_BAS) {
        if (pos == nbLigneVisible - 1) {
          pos = 0;
          deb = 0;
        } else ++deb;
      } else { // touche BT_HAUT
        if (pos == 0) {
          pos = nbLigneVisible - 1;
          deb = (nbLigneVisible < nb) ? nb - nbLigneVisible : 0;
        }
        else --deb;
      }
    }
  } // fin boucle infinie complète
  if (touche == 0) touche = TIMEOUT_LED;
  if (touche == BT_SELECT) touche = 0;
  invertRect(xMin, yMin + (pos * hLigne) * hautCarac, xMax - xMin, hLigne * hautCarac);
  display();

  retourSelect result;
  result.result = pos + deb;
  result.touche = touche;
  setTextSize(1);
  return result;
}

retourSelect IO_PCD8544_3touches_SPI_software_FreeRTOS::menuListe(char* chaine, uint8_t nb, uint8_t taille, uint8_t yMin, uint8_t yMax, uint8_t xMin, uint8_t xMax, uint8_t hLigne, uint8_t depart, uint8_t textSize, uint8_t actual, uint8_t permissions) {
//      affiche une liste est retourne le numéro de l'item selectionné
//      liste est un tableau de pointeur vers des chaine comprenant la chaine de caractère de l'item
//      nb est le nombre d'élément dans la liste principale
//      taille donne la taille des élements de la chaine /!\ y compris le bit de fin de chaine /!\.
//      yMin est le point de départ de la liste en graphique (par défaut 0)
//      yMax est le bas de la liste (par défaut bas de l'écran)
//      xMin ... idem en X
//      xMax ... idem en X
//      hLigne est la hauteur de la ligne en nb de caractère (càd 1 car = 8 pixels de haut), par defaut 1
//      permissions indique les permissions pour quitter le menu (autre que sélection), par défaut TIMEOUT_LED & BT_ESC
//
//      /!\ si plusieurs lignes pour une chaine (càd hLigne!=0) compter un caractère en plus pour chaque ligne pour ajouter une fin de ligne

  return menuListeV2(_creeLigneMenuListeV1, (void*)chaine, nb, taille, yMin, yMax, xMin, xMax, hLigne, depart, textSize, actual, permissions);
}



retourSelect IO_PCD8544_3touches_SPI_software_FreeRTOS::menuListeV2(void (*callback)(uint8_t, char*, uint8_t, void*), void* arg, uint8_t nb, uint8_t taille, uint8_t yMin, uint8_t yMax, uint8_t xMin, uint8_t xMax, uint8_t hLigne, uint8_t depart, uint8_t textSize, uint8_t actual, uint8_t permissions) {
//      affiche une liste est retourne le numéro de l'item selectionné
//      liste est un tableau de pointeur vers des chaine comprenant la chaine de caractère de l'item
//      nb est le nombre d'élément dans la liste principale
//      taille donne la taille des élements de la chaine /!\ y compris le bit de fin de chaine /!\.
//      yMin est le point de départ de la liste en graphique (par défaut 0)
//      yMax est le bas de la liste (par défaut bas de l'écran)
//      xMin ... idem en X
//      xMax ... idem en X
//      hLigne est la hauteur de la ligne en nb de caractère (càd 1 car = 8 pixels de haut), par defaut 1
//      permissions indique les permissions pour quitter le menu (autre que sélection), par défaut TIMEOUT_LED & BT_ESC
//
//      /!\ si plusieurs lignes pour une chaine (càd hLigne!=0) compter un caractère en plus pour chaque ligne pour ajouter une fin de ligne

#ifdef DEBUG
  Serial.print("menuListeV2(callback@");
  Serial.print((uint16_t)callback, HEX);
  Serial.print(", arg@");
  Serial.print((uint16_t)arg, HEX);
  Serial.print(", nb=");
  Serial.print(nb);
  Serial.print(", taille=");
  Serial.print(taille);
  Serial.print(", yMin=");
  Serial.print(yMin);
  Serial.print(", yMax=");
  Serial.print(yMax);
  Serial.print(", xMin=");
  Serial.print(xMin);
  Serial.print(", xMax=");
  Serial.print(xMax);
  Serial.print(", hLigne=");
  Serial.print(hLigne);
  Serial.print(", depart=");
  Serial.print(depart);
  Serial.print(", textSize=");
  Serial.print(textSize);
  Serial.print(", actual=");
  Serial.print(actual);
  Serial.print(", permissions=0b");
  Serial.print(permissions, BIN);
  Serial.println(");");
#endif

  //uint8_t tailleLigne;
  uint8_t largCarac = (textSize == 0) ? 4 : (6 * textSize);
  uint8_t hautCarac = (textSize == 0) ? 6 : (8 * textSize);

  taille = min(taille, ((xMax - xMin)  / largCarac + 1) * hLigne); // taille de la ligne en caractères
  //if (hLigne == 1)tailleLigne = taille;
  //else tailleLigne = (xMax - xMin) / largCarac + 1;
#ifdef DEBUG
  Serial.print("taille:");
  Serial.print(taille);
#endif

  uint8_t touche, pos, deb;
  uint8_t nbLigneVisible = min(((yMax - yMin) / (hautCarac * hLigne) ), nb); // nombre de ligne visible à la fois
  uint8_t derniereLigne = (((yMax - yMin) % (hautCarac * hLigne)  == 0) || (nb == nbLigneVisible)) ? 0 : 1;
  permissions &= 0b11110000; // pas de permissions pour les 4 1ere touche ( BT_SELECT,BT_HAUT,BT_BAS)
  uint16_t colorT = BLACK, colorF = WHITE;
  char chaine[22];


  // ajout depart
  if (depart == 0) {
    pos = 0;
    deb = depart;
  } else if ( depart <= nb - nbLigneVisible) {
    pos = 1;
    deb = depart - 1;
  } else {
    deb = nb - nbLigneVisible;
    pos = depart - deb;
  }
  setTextSize(textSize);
  for (;;) { // boucle infini complète
    // efface la zone d'affichage
    fillRect(xMin, yMin, xMax, yMax, WHITE);
    for (uint8_t i = 0; i < (nbLigneVisible + derniereLigne); ++i) {

      callback(i + deb, chaine, taille, arg);

      for (uint8_t j = 0; j < hLigne; ++j) {
        setCursor(xMin, yMin + (i * hLigne + j) * hautCarac);
        uint8_t hexa = 0;
        for ( uint8_t k = 0; k < taille && k < 22; ++k) {
          char c = chaine[k];
          if ( c == 0 && hexa == 0) break;
          if ( c == -1) {
            ++k;
            hexa = chaine[k];
          } else {
            if ( hexa != 0) {
              if ( (uint8_t)c < 16) print(0);
              print((uint8_t)c, HEX);
              --hexa;
            } else {
              print(c);
            }
          }
        }
      }
    }
    invertRect(xMin, yMin + (pos * hLigne) * hautCarac, xMax - xMin, hLigne * hautCarac);

    if (deb + nbLigneVisible == nb) fillRect(xMin, yMin + nbLigneVisible * hautCarac * hLigne, largCarac * (taille - 1), yMax - yMin - nbLigneVisible * hautCarac * hLigne, colorF);
    display();
    for (;;) { // boucle infini gestion touche
      aucuneTouche();
      if ( actual == 0) touche = getTouche(-1); // cas normal, pas d'actualisation
      else {
        touche = getTouche(actual);
        if ( touche == TIMEOUT_LED  && millis() - getDateDerniereTouche() < 60000 ) {
          touche = 0; // si TIMEOUT_LED mais qu'on n'a pas atteint l'extinction de l'ecran -> touche = 0
          break;
        }
      }
      if ( ((touche == TIMEOUT_LED) && ((permissions & TIMEOUT_LED) != 0)) || ((touche == (BT_HAUT | BT_BAS)) && ((permissions & BT_ESC) != 0)) || ((touche == (BT_HAUT | BT_BAS | BT_SELECT)) && ((permissions & BT_MENU) != 0)) || (touche == BT_SELECT)) break;

      if (touche == BT_BAS || touche == BT_HAUT) {
        invertRect(xMin, yMin + (pos * hLigne) * hautCarac, xMax - xMin, hLigne * hautCarac);
        if (touche == BT_BAS) {
          if ((pos == nbLigneVisible - 1) || (((deb + nbLigneVisible) != nb) && pos == nbLigneVisible - 2)) break;
          ++pos;
        } else { // touche == BT_HAUT
          if  ((pos == 0) || ((deb != 0 && pos == 1))) break;
          --pos;
        }
        invertRect(xMin, yMin + (pos * hLigne) * hautCarac, xMax - xMin, hLigne * hautCarac);
        display();
      }
    } // fin boucle infini gestion touche
    if ( touche != 0) {
      if (touche != BT_BAS && touche != BT_HAUT) break;
      if (touche == BT_BAS) {
        if (pos == nbLigneVisible - 1) {
          pos = 0;
          deb = 0;
        } else ++deb;
      } else { // touche BT_HAUT
        if (pos == 0) {
          pos = nbLigneVisible - 1;
          deb = (nbLigneVisible < nb) ? nb - nbLigneVisible : 0;
        }
        else --deb;
      }
    }
  } // fin boucle infinie complète
  if (touche == 0) touche = TIMEOUT_LED;
  if (touche == BT_SELECT) touche = 0;
  invertRect(xMin, yMin + (pos * hLigne) * hautCarac, xMax - xMin, hLigne * hautCarac);
  display();

  retourSelect result;
  result.result = pos + deb;
  result.touche = touche;
  setTextSize(1);
  return result;
}



retourSelect IO_PCD8544_3touches_SPI_software_FreeRTOS::menuListeOuiNon(uint8_t yMin, uint8_t yMax, uint8_t xMin, uint8_t xMax, uint8_t hLigne, uint8_t depart, uint8_t textSize, uint8_t permissions) {
  return menuListeV2(_creeLigneMenuListeOuiNon, (void*)&yMin, 2, 4, yMin, yMax, xMin, xMax, hLigne, depart, textSize, 0, permissions);
}
*/
