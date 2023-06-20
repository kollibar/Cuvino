#ifndef __WIDGETS_H__
#define __WIDGETS_H__


#include <Adafruit_GFX_v1.0.2_NB8bits.h>
#include <IO_PCD8544_5T.h>

#undef DEBUG

template<typename T> class Widgets;

struct retourSelect {
  uint8_t touche;
  uint8_t result;
};

template<typename T>
class Widgets{
public:
  //Widgets();
  Widgets(T& _display);

  retourSelect menuListeOld(char* chaine, uint8_t nb, uint8_t taille = 15, uint8_t yMin = 0, uint8_t yMax = LCDHEIGHT, uint8_t xMin = 0, uint8_t xMax = LCDWIDTH, uint8_t hLigne = 1, uint8_t depart = 0, uint8_t textSize = 1, uint8_t actual = 0, uint8_t permissions = 0b10100000);  // pour ne pas supprimer le code si besoin
  retourSelect menuListe(char* chaine, uint8_t nb, uint8_t taille = 15, uint8_t yMin = 0, uint8_t yMax = LCDHEIGHT, uint8_t xMin = 0, uint8_t xMax = LCDWIDTH, uint8_t hLigne = 1, uint8_t depart = 0, uint8_t textSize = 1, uint8_t actual = 0, uint8_t permissions = 0b10100000); // pour compatibilité
  retourSelect menuListeV2(void (*genChaine)(uint8_t, char*, uint8_t, void*), void* arg, uint8_t nb, uint8_t taille = 15, uint8_t yMin = 0, uint8_t yMax = LCDHEIGHT, uint8_t xMin = 0, uint8_t xMax = LCDWIDTH, uint8_t hLigne = 1, uint8_t depart = 0, uint8_t textSize = 1, uint8_t actual = 0, uint8_t permissions = 0b10100000);
  retourSelect menuListeOuiNon(uint8_t yMin = 0, uint8_t yMax = LCDHEIGHT, uint8_t xMin = 0, uint8_t xMax = LCDWIDTH, uint8_t hLigne = 1, uint8_t depart = 0, uint8_t textSize = 1, uint8_t permissions = 0b10100000);


protected:
  T* display;
};

void _creeLigneMenuListeV1(uint8_t i, char* chaine, uint8_t taille, void* arg);
void _creeLigneMenuListeOuiNon(uint8_t i, char* chaine, uint8_t taille, void* arg);


//template<typename T>
//Widgets<T>::Widgets(){
//  display=NULL;
//}

template<typename T>
Widgets<T>::Widgets(T& _display){
  display=&_display;
}

template<typename T>
retourSelect Widgets<T>::menuListeOld(char* chaine, uint8_t nb, uint8_t taille, uint8_t yMin, uint8_t yMax, uint8_t xMin, uint8_t xMax, uint8_t hLigne, uint8_t depart, uint8_t textSize, uint8_t actual, uint8_t permissions) {
  /*   affiche une liste est retourne le numéro de l'item selectionné
        liste est un tableau de pointeur vers des chaine comprenant la chaine de caractère de l'item
        nb est le nombre d'élément dans la liste principale
        taille donne la taille des élements de la chaine /!\ y compris le bit de fin de chaine /!\
        yMin est le point de départ de la liste en graphique (par défaut 0)
        yMax est le bas de la liste (par défaut bas de l'écran)
        xMin ... idem en X
        xMax ... idem en X
        hLigne est la hauteur de la ligne en nb de caractère (càd 1 car = 8 pixels de haut), par defaut 1
        permissions indique les permissions pour quitter le menu (autre que sélection), par défaut TIMEOUT_LED & BT_ESC

        /!\ si plusieurs lignes pour une chaine (càd hLigne!=0) compter un caractère en plus pour chaque ligne pour ajouter une fin de ligne
  */
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
  display->setTextSize(textSize);
  for (;;) { // boucle infini complète
    // efface la zone d'affichage
    display->fillRect(xMin, yMin, xMax, yMax, WHITE);
    for (uint8_t i = 0; i < (nbLigneVisible + derniereLigne); ++i) {

      for (uint8_t j = 0; j < hLigne; ++j) {
        display->setCursor(xMin, yMin + (i * hLigne + j) * hautCarac);
        uint8_t k = (i + deb) * taille + j * tailleLigne;
        uint8_t hexa = 0;
        char c = chaine[k];
        while (c != 0 || hexa != 0 ) {
          if ( c == -1) {
            ++k;
            hexa = chaine[k];
          } else {
            if ( hexa != 0) {
              if ( (uint8_t)c < 16) display->print(0);
              display->print((uint8_t)c, HEX);
              --hexa;
            } else {
              display->print(c);
            }
          }
          ++k;
          c = chaine[k];
        }
      }
    }
    display->invertRect(xMin, yMin + (pos * hLigne) * hautCarac, xMax - xMin, hLigne * hautCarac);

    if (deb + nbLigneVisible == nb) display->fillRect(xMin, yMin + nbLigneVisible * hautCarac * hLigne, largCarac * (taille - 1), yMax - yMin - nbLigneVisible * hautCarac * hLigne, colorF);
    display->display();
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
      display->aucuneTouche();
      if ( actual == 0) touche = display->getTouche(-1); // cas normal, pas d'actualisation
      else {
        touche = display->getTouche(actual);
        if( touche == TIMEOUT_DECOMPTE) break;
        /*if ( touche == TIMEOUT_LED  && millis() - display->getDateDerniereTouche() < 60000 ) {
          touche = 0; // si TIMEOUT_LED mais qu'on n'a pas atteint l'extinction de l'ecran -> touche = 0
          break;
        }*/
      }
      if ( ((touche == TIMEOUT_LED) && ((permissions & TIMEOUT) != 0))
        || ((touche == (BT_HAUT | BT_BAS)) && ((permissions & BT_ESC) != 0))
        || ((touche == (BT_HAUT | BT_BAS | BT_SELECT)) && ((permissions & BT_MENU) != 0))
        || (touche == BT_SELECT)
      ) break;

      if (touche == BT_BAS || touche == BT_HAUT) {
        display->invertRect(xMin, yMin + (pos * hLigne) * hautCarac, xMax - xMin, hLigne * hautCarac);
        if (touche == BT_BAS) {
          if ((pos == nbLigneVisible - 1) || (((deb + nbLigneVisible) != nb) && pos == nbLigneVisible - 2)) break;
          ++pos;
        } else { // touche == BT_HAUT
          if  ((pos == 0) || ((deb != 0 && pos == 1))) break;
          --pos;
        }
        display->invertRect(xMin, yMin + (pos * hLigne) * hautCarac, xMax - xMin, hLigne * hautCarac);
        display->display();
      }
    } // fin boucle infini gestion touche
    if ( (touche&TIMEOUT) == 0) { // si on a pas de timeout
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
    } else if( ((touche == TIMEOUT_LED) && ((permissions & TIMEOUT) != 0))) break;
  } // fin boucle infinie complète
  if (touche == BT_SELECT) touche = 0;
  display->invertRect(xMin, yMin + (pos * hLigne) * hautCarac, xMax - xMin, hLigne * hautCarac);
  display->display();

  retourSelect result;
  result.result = pos + deb;
  result.touche = touche;
  display->setTextSize(1);
  return result;
}

/**   affiche une liste est retourne le numéro de l'item selectionné
 * @param liste     un tableau de pointeur vers des chaine comprenant la chaine de caractère de l'item
 * @param nb        nombre d'élément dans la liste principale
 * @param taille    taille des élements de la chaine /!\ y compris le bit de fin de chaine /!\
 * @param yMin      position de la liste dans l'écran yMin (par défaut 0)
 * @param yMax      position de la liste dans l'écran yMax (par défaut bas de l'écran)
 * @param xMin      position de la liste dans l'écran xMin (par défaut 0)
 * @param xMax      position de la liste dans l'écran xMax (par défaut la droite de l'écran) 
 * @param hLigne    hauteur de la ligne en nb de caractère (càd 1 car = 8 pixels de haut), par defaut 1
 * @param depart    selection de départ
 * @param textSize  
 * @param actual
 * @param permissions   indique les permissions pour quitter le menu (autre que sélection), par défaut TIMEOUT_LED & BT_ESC
 * @return  le numéro de l'item selectionné
        l/!\ si plusieurs lignes pour une chaine (càd hLigne!=0) compter un caractère en plus pour chaque ligne pour ajouter une fin de ligne
  */
template<typename T>
retourSelect Widgets<T>::menuListe(char* chaine, uint8_t nb, uint8_t taille, uint8_t yMin, uint8_t yMax, uint8_t xMin, uint8_t xMax, uint8_t hLigne, uint8_t depart, uint8_t textSize, uint8_t actual, uint8_t permissions) {
  return menuListeV2( _creeLigneMenuListeV1, (void*)chaine, nb, taille, yMin, yMax, xMin, xMax, hLigne, depart, textSize, actual, permissions);
}

/**   affiche une liste est retourne le numéro de l'item selectionné
 * @param genChaine fonction générant les différentes chaines à afficher, recevant 4 paramètres: 
 *                                  uint8_t position dans le menu,
 *                                  char* pointeur vers la chaine où écrire les données, 
 *                                  uint8_t taille de la chaine,
 *                                  void* pointeur vers un argument supplémentaire (cf ligne suivante)
 * @param arg       pointeur à passer à la fonction genChaine
 * @param nb        nombre d'élément dans la liste principale
 * @param taille    taille des élements de la chaine /!\ y compris le bit de fin de chaine /!\
 * @param yMin      position de la liste dans l'écran yMin (par défaut 0)
 * @param yMax      position de la liste dans l'écran yMax (par défaut bas de l'écran)
 * @param xMin      position de la liste dans l'écran xMin (par défaut 0)
 * @param xMax      position de la liste dans l'écran xMax (par défaut la droite de l'écran) 
 * @param hLigne    hauteur de la ligne en nb de caractère (càd 1 car = 8 pixels de haut), par defaut 1
 * @param depart    selection de départ
 * @param textSize  
 * @param actual
 * @param permissions   indique les permissions pour quitter le menu (autre que sélection), par défaut TIMEOUT_LED & BT_ESC
 * @return  le numéro de l'item selectionné
        l/!\ si plusieurs lignes pour une chaine (càd hLigne!=0) compter un caractère en plus pour chaque ligne pour ajouter une fin de ligne
  */
template<typename T>
retourSelect Widgets<T>::menuListeV2( void (*genChaine)(uint8_t, char*, uint8_t, void*), void* arg, uint8_t nb, uint8_t taille, uint8_t yMin, uint8_t yMax, uint8_t xMin, uint8_t xMax, uint8_t hLigne, uint8_t depart, uint8_t textSize, uint8_t actual, uint8_t permissions) {
#ifdef DEBUG
  Serial.print("menuListeV2(genChaine@");
  Serial.print((uint16_t)genChaine, HEX);
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
  display->setTextSize(textSize);
  for (;;) { // boucle infini complète
    // efface la zone d'affichage
    display->fillRect(xMin, yMin, xMax, yMax, WHITE);
    for (uint8_t i = 0; i < (nbLigneVisible + derniereLigne); ++i) {

      genChaine(i + deb, chaine, taille, arg);

      for (uint8_t j = 0; j < hLigne; ++j) {
        display->setCursor(xMin, yMin + (i * hLigne + j) * hautCarac);
        uint8_t hexa = 0;
        for ( uint8_t k = 0; k < taille && k < 22; ++k) {
          char c = chaine[k];
          if ( c == 0 && hexa == 0) break;
          if ( c == -1) {
            ++k;
            hexa = chaine[k];
          } else {
            if ( hexa != 0) {
              if ( (uint8_t)c < 16) display->print(0);
              display->print((uint8_t)c, HEX);
              --hexa;
            } else {
              display->print(c);
            }
          }
        }
      }
    }
    display->invertRect(xMin, yMin + (pos * hLigne) * hautCarac, xMax - xMin, hLigne * hautCarac);

    if (deb + nbLigneVisible == nb) display->fillRect(xMin, yMin + nbLigneVisible * hautCarac * hLigne, largCarac * (taille - 1), yMax - yMin - nbLigneVisible * hautCarac * hLigne, colorF);
    display->display();
    for (;;) { // boucle infini gestion touche
      display->aucuneTouche();
      if ( actual == 0) touche = display->getTouche(-1); // cas normal, pas d'actualisation
      else {
        touche = display->getTouche(actual);
        if( touche == TIMEOUT_DECOMPTE) break;
      }
      if ( ((touche == TIMEOUT_LED) && ((permissions & TIMEOUT_LED) != 0))
        || ((touche == (BT_HAUT | BT_BAS)) && ((permissions & BT_ESC) != 0))
        || ((touche == BT_ESC) && ((permissions & BT_ESC) != 0))
        || ((touche == (BT_HAUT | BT_BAS | BT_SELECT)) && ((permissions & BT_MENU) != 0))
        || ((touche == BT_MENU) && ((permissions & BT_MENU) != 0))
        || (touche == BT_SELECT)
      ) break;

      if (touche == BT_BAS || touche == BT_HAUT) {
        display->invertRect(xMin, yMin + (pos * hLigne) * hautCarac, xMax - xMin, hLigne * hautCarac);
        if (touche == BT_BAS) {
          if ((pos == nbLigneVisible - 1) || (((deb + nbLigneVisible) != nb) && pos == nbLigneVisible - 2)) break;
          ++pos;
        } else { // touche == BT_HAUT
          if  ((pos == 0) || ((deb != 0 && pos == 1))) break;
          --pos;
        }
        display->invertRect(xMin, yMin + (pos * hLigne) * hautCarac, xMax - xMin, hLigne * hautCarac);
        display->display();
      }
    } // fin boucle infini gestion touche
    if ( touche != TIMEOUT_DECOMPTE) {
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
  display->invertRect(xMin, yMin + (pos * hLigne) * hautCarac, xMax - xMin, hLigne * hautCarac);
  display->display();

  retourSelect result;
  result.result = pos + deb;
  result.touche = touche;
  display->setTextSize(1);
  return result;
}


template<typename T>
retourSelect Widgets<T>::menuListeOuiNon(uint8_t yMin, uint8_t yMax, uint8_t xMin, uint8_t xMax, uint8_t hLigne, uint8_t depart, uint8_t textSize, uint8_t permissions) {
  return menuListeV2( _creeLigneMenuListeOuiNon, (void*)&yMin, 2, 4, yMin, yMax, xMin, xMax, hLigne, depart, textSize, 0, permissions);
}



#endif
