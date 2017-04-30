#include "CuvinoWidgets.h"

void _creeLigneMenuSelectSondeV3(uint8_t i, char* chaine, uint8_t taille, void* arg) {
  structDataMenuSelectSondeV3* data = (structDataMenuSelectSondeV3*)arg;
  int16_t temp;

  if ( i == 0) {
    strcpy_P(chaine, FR::TXT_AUCUNE);
  } else {
    i--; // comme la première ligne est la sonde AUCUNE ... on décrémente i pour correspondre à la liste
    if( data->sonde[i].isSondeTemp()){

      if ( data->temperature[i] > TEMP_NON_LUE) temp = data->sonde[i].litTemperature();
      else temp = data->temperature[i];

      if (data->sonde[i].isDS3231()) { // si sonde interne au DS3231(RTC)
        strcpy_P(chaine, FR::TXT_INTERNE);
        chaine[7] = ' ';
        chaine[8] = ' ';
      } else { // DS18B20 sur OneWire
        chaine[0] = -1;
        chaine[1] = 7;
        for (uint8_t j = 1; j < 8; ++j) {
          chaine[j + 1] = data->sonde[i].addr[j];
        }
      }
      chaine[9] = ' ';

      if ( temp == TEMP_NON_LUE) strcpy(&chaine[15], " -- ");
      else if ( temp < TEMP_NON_LUE) {
        strcpy_P(&chaine[10], FR::TXT_ERR);
        chaine[13] = (char)(TEMP_ERREUR - temp + 48);
      } else {
        if ( temp > 1584) { // 1584 (1/16e °C) = 99°C
          strcpy(&chaine[10], ">99");
        } else {
          if (temp < 0) {
            chaine[10] = '-';
            temp = -temp;
          } else chaine[10] = ' ';
          temp = temp >> 4;
          if ( temp > 10) chaine[11] = temp / 10 + 48;
          else chaine[11] = ' ';
          chaine[12] = temp % 10 + 48;
        }
        chaine[13] = (char)248;
      }

      chaine[14] = 0;

      if ( temp == TEMP_NON_LUE) data->temperature[i] = 0;
      data->sonde[i].demandeMesureTemp();
    }
  }
}
void _creeLigneMenuSelectSondeForV2(uint8_t i, char* chaine, uint8_t taille, void* arg) {
  structDataMenuSelectSondeForV2* data = (structDataMenuSelectSondeForV2*)arg;
  if( i==0 ) strcpy_P(chaine,FR::TXT_AUCUNE);
  else{
    if( data->numSonde[i-1] == SONDE_DS3231 ){ // sonde interne
      strcpy_P(chaine, FR::TXT_INTERNE);
    } else{
      chaine[0] = data->numSonde[i-1] + 48;
      chaine[1] = 0;
      if( data->utilisation[i-1] == 1){ // sonde actuellement affectée à la cuve en cours d'édition

      } else if ( data->utilisation[i-1] != 0){ // sonde affectée à une autre cuve
        chaine[1]=' ';
        chaine[2]='(';
        strcpy_P(&chaine[3],FR::TXT_CUVE);
        chaine[7]=' ';
        chaine[8]=data->utilisation[i-1];
        chaine[9]=')';
        chaine[10]=0;
      }
    }
  }
}

void _creeLigneMenuSelectSondeV2(uint8_t i, char* chaine, uint8_t taille, void* arg) {
  structDataMenuSelectSonde* data = (structDataMenuSelectSonde*)arg;
  int16_t temp;

  if ( i == 0) {
    strcpy_P(chaine, FR::TXT_AUCUNE);
  } else {
    i--; // comme la première ligne est la sonde AUCUNE ... on décrémente i pour correspondre à la liste
    if( data->sonde[i].isSondeTemp()){

      if ( data->temperature[i] > TEMP_NON_LUE) temp = data->sonde[i].litTemperature();
      else temp = data->temperature[i];

      if (data->sonde[i].isDS3231()) { // si sonde interne au DS3231(RTC)
        strcpy_P(chaine, FR::TXT_INTERNE);
        chaine[7] = ' ';
        chaine[8] = ' ';
      } else { // DS18B20 sur OneWire
        chaine[0] = -1;
        chaine[1] = 7;
        for (uint8_t j = 1; j < 8; ++j) {
          chaine[j + 1] = data->sonde[i].addr[j];
        }
      }
      chaine[9] = ' ';

      if ( temp == TEMP_NON_LUE) strcpy(&chaine[15], " -- ");
      else if ( temp < TEMP_NON_LUE) {
        strcpy_P(&chaine[10], FR::TXT_ERR);
        chaine[13] = (char)(TEMP_ERREUR - temp + 48);
      } else {
        if ( temp > 1584) { // 1584 (1/16e °C) = 99°C
          strcpy(&chaine[10], ">99");
        } else {
          if (temp < 0) {
            chaine[10] = '-';
            temp = -temp;
          } else chaine[10] = ' ';
          temp = temp >> 4;
          if ( temp > 10) chaine[11] = temp / 10 + 48;
          else chaine[11] = ' ';
          chaine[12] = temp % 10 + 48;
        }
        chaine[13] = (char)248;
      }

      chaine[14] = 0;

      if ( temp == TEMP_NON_LUE) data->temperature[i] = 0;
      data->sonde[i].demandeMesureTemp();
    }
  }
}

void _creeLigneMenuChoixEV(uint8_t i, char* chaine, uint8_t taille, void* arg) {
  if ( i == NB_EV_MAX) strcpy_P(chaine, FR::TXT_AUCUNE);
  else {
    if ( i < 10) chaine[0] = i + 48;
    else chaine[0] = i + 55;
    chaine[1] = 0;
  }
}

void _creeLigneMenuChoixModeEV(uint8_t i, char* chaine, uint8_t taille, void* arg) {
  strcpy_P(chaine, TXT_EV(EVinListe(i)));
}

void _creeLigneMenuManipuleEV(uint8_t i, char* chaine, uint8_t taille, void* arg) {
  switch(i){
    case 0:
      strcpy_P(chaine,FR::TXT_OUVRIR);
      break;
    case 1:
      strcpy_P(chaine,FR::TXT_FERMER);
      break;
    case 2:
      strcpy_P(chaine,FR::TXT_ETAT);
      break;
    case 3:
      strcpy_P(chaine,FR::TXT_VALIDER);
      break;
    case 4:
      strcpy_P(chaine,FR::TXT_ANNULER);
      break;
  }
}
