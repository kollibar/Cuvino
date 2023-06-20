void menuSelectSonde(int8_t cuve) {
#ifdef DEBUG
  Serial.print("menuSelectSonde(");
  Serial.print(cuve);
  Serial.println(')');
#endif
  structDataMenuSelectSonde data;
  byte touche;
  if ( cuve == NB_LIGNE_MAX) {
    data.nb = 1;
    data.addr[0][0] = 'D';
    data.addr[0][1] = 'S';
    data.addr[0][2] = '3';
    data.addr[0][3] = '2';
    data.addr[0][4] = '3';
    data.addr[0][5] = '1';
    data.addr[0][6] = 0;
  } else {
    data.nb = 0;
  }
  data.sel = -2;

  for (byte i = 0; i < NB_SONDE_MAX; ++i) data.use[i] = -2;

  display.clearDisplay();

  display.printlnPGM(TXT_SELECT_SONDE);
  display.setCursor(0, 8);
  if ( cuve == NB_LIGNE_MAX) display.printlnPGM(TXT_DU_LOCAL);
  else {
    display.printPGM(TXT_CUVE);
    display.write(' ');
    display.println(cuve);
  }

  ds.reset_search();    // Réinitialise la recherche de module
  while (ds.search(data.addr[data.nb])) {
#ifdef DEBUG
    Serial.print("addresse 1Wire ");
    for (byte c = 0; c < 8; ++c) Serial.print(data.addr[data.nb][c], HEX);
    Serial.println("");
#endif
    if (OneWire::crc8(data.addr[data.nb], 7) != data.addr[data.nb][7]) {// Vérifie que l'adresse a été correctement reçue
      data.nb = NB_SONDE_MAX + 1;

      display.clearDisplay();
      display.printlnPGM(TXT_ERR_COMM_SONDE);
      display.printPGM(TXT_DEFAUT_BRANCH_SONDE);
      aucuneTouche();
      getTouche(-1);
      return;
    }
    if (data.addr[data.nb][0] == DS18B20) // Vérifie qu'il s'agit bien d'un DS18B20
      ++data.nb;
    if (data.nb == NB_SONDE_MAX) break;
  }
#ifdef DEBUG
  Serial.print("nb sonde trouvée:");
  Serial.println(data.nb);
#endif

  for (byte i = 0; i < data.nb; ++i) {

    demandeMesureTemp(data.addr[data.nb]); // demande une mesure de la temperature de la sonde i
    data.temperature[data.nb] = TEMP_NON_LUE;

    if ( data.use[i] != -2) { // si la sonde i n'est pas déjà utilisé par un capteur
      // alors on va chercher si dans la liste des sondes déjà affecté, elle ne serait pas déjà existante

      // on cherche dans les sondes de chaque cuve
      for (byte j = 0; j <= NB_LIGNE_MAX; ++j) {
        for (byte k = 0; k < 8; ++k) {
          if (data.addr[i][k] != listeCuve[j].addrSonde[k]) break;
          if ( k == 7) {
            data.use[i] = j;
            if ( j == cuve) data.sel = i;
          }
        }
      }
    }
  }

  for (byte k = 0; k < 8; ++k) data.addr[data.nb][k] = 0; // ajout d'une sonde de température d'adresse 0000000000000000 pour pouvoir ne pas sélectionner de sonde
  ++data.nb;

  // declenche un timer toutes les secondes
  data.timerID = timer.every(1000, _actualiseTemp_selectSonde, (void*)&data);

  if ( data.sel != -2) { // si un déjà selectionné
    if ( data.nb < 6 or data.sel < 4) {
      data.deb = 0; data.pos = data.sel;
    } else {
      data.pos = 2; data.deb = data.sel - 2;
    }
  } else { // si aucune selection déjà faite
    data.pos = 0; data.deb = 0;
  }
  for (;;) { // boucle infinie modification tout l'écran

    // efface la partie basse de l'écran
    display.drawRect(0, 18, 84, 30, WHITE);
    // affiche les nouvelles lignes
    byte i = 0;
    do {
      affLigneSelectSonde(18 + 6 * i, data.addr[i + data.deb], data.temperature[i + data.deb]); // si use[i] != -2 alors la sonde est déjà utilisé -> surligne
      ++i;
    } while (i < 5 and data.deb + i < data.nb); // tant qu'on est < 5 (5lignes max à l'écran ou le max encore affichable)

    for (;;) { // boucle infinie déplacmeent flèche seul
      display.setCursor(0, 18 + data.pos * 6);
      display.setTextSize(0);
      display.write('>');
      display.display();

      for (;;) { // boucle infinie attente touche
        aucuneTouche();
        touche = getTouche(-1);
        if ( touche == BT_SELECT) { // selection de la sonde
#ifdef DEBUG
          if (cuve == NB_LIGNE_MAX) {
            Serial.print("selection de la sonde de la cuve ");
            Serial.print(cuve);
            Serial.print(":");
          } else {
            Serial.print("selection de la sonde du local:");
          }
#endif
          for (byte k = 0; k < 8; ++k) {
            listeCuve[cuve].addrSonde[k] = data.addr[data.deb + data.pos][k];
#ifdef DEBUG
            Serial.print(data.addr[data.deb + data.pos][k], HEX);
#endif
          }
          timer.stop(data.timerID); // on arrete l'actualisation des temperatures
          display.setTextSize(1); // on remet la taille du texte à 1
          return; // on a selectionné et enregistré la sonde... on quite!!
        }
        if ( touche == (BT_BAS | BT_HAUT) or touche == TIMEOUT ) { // bouton haut et bas en même temps, ou temps d'attente trop long
          if ( data.sel != -2) {
            timer.stop(data.timerID); // on arrete l'actualisation des temperatures
            display.setTextSize(1); // on remet la taille du texte à 1
            return; // si sonde déjà selectionnée -> on annule
          }
        }
        if ( ( touche == BT_BAS) | (touche == BT_HAUT) ) {
          break;
        }

      } // fin boucle infinie attente touche
      display.setTextSize(0);
      display.setCursor(0, 18 + data.pos * 6);
      display.write(' ');

      if ( touche == BT_HAUT ) {
        --data.pos;
        if ( data.nb > 5 and data.pos == 0 and data.deb > 0) {
          --data.deb;
          ++data.pos;
          break;
        }
        if ( data.pos < 0) {
          if ( data.nb > 5) {
            data.pos = 4;
            data.deb = data.nb - 5;
            break;
          } else data.pos = data.nb - 1;
        }
      }
      if ( touche == BT_BAS ) {
        ++data.pos;
        if ( data.nb > 5 and data.pos == 4 and data.nb - 5 - data.deb > 0) {
          ++data.deb;
          --data.pos;
          break;
        }
        if ( data.pos > 4 or data.pos >= data.nb) {
          if ( data.nb > 5) {
            data.deb = 0;
            data.pos = 0;
            break;
          } else data.pos = 0;
        }
      }

    } // fin boucle infinie déplacement flèche seul
  }// fin boucle infinie modification tout l'écran
  display.setTextSize(1); // on remet la taille du texte à 1
}

void _actualiseTemp_selectSonde(void* arg) {
  /* fonction d'actualisation des températures affichées appellé par la fonction menuSelectSonde */
  structDataMenuSelectSonde* data = (structDataMenuSelectSonde*)arg;

  for (byte i = 0; i < data->nb; ++i) {
    data->temperature[i] = litTemperature(data->addr[i]);
    if ( i >= data->deb and i < data->deb + 5) { // la ligne i est à l'écran
      affLigneSelectSonde(18 + (i - data->deb) * 6, data->addr[i], data->temperature[i]);
      display.display();
    }
    demandeMesureTemp(data->addr[i]);
  }
}
