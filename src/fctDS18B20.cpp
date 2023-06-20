#include "fctDS18B20.h"

bool getAddresse1Wire(OneWire& ds,unsigned char addr[8]) {
  /* recherche l'adresse du 1er module 1 wire trouvé */
  //unsigned char data[9], addr[8];
  // data : Données lues depuis le scratchpad
  // addr : adresse du module 1-Wire détecté

  if (!ds.search(addr)) { // Recherche un module 1-Wire
    ds.reset_search();    // Réinitialise la recherche de module
    return false;         // Retourne une erreur
  }

  if (OneWire::crc8(addr, 7) != addr[7]) // Vérifie que l'adresse a été correctement reçue
    return false;                        // Si le message est corrompu on retourne une erreur

  if (addr[0] != DS18B20) // Vérifie qu'il s'agit bien d'un DS18B20
    return false;         // Si ce n'est pas le cas on retourne une erreur
  return true;
}


signed char getNbDS18B20(OneWire& ds){
  // retourne le nombre de sonde DS18b20 trouvé
  signed char nb;
  unsigned char addr[8];
  ds.reset_search();    // Réinitialise la recherche de module
  while (ds.search(addr)) {
#ifdef DEBUG
    debugSonde.print(F("addresse 1Wire "));
    for (uint8_t c = 0; c < 8; ++c) debugSonde.print(addr[c], HEX);
    debugSonde.println();
#endif
    if (OneWire::crc8(addr, 7) != addr[7]) {// Vérifie que l'adresse a été correctement reçue
#ifdef DEBUG
      debugSonde.print(F("Erreur lors de la transmission"));
#endif
      return -1;
    }
    if (addr[0] == DS18B20) // Vérifie qu'il s'agit bien d'un DS18B20
      ++nb;
  }
  return nb;
}

bool getDS18B20(OneWire& ds,unsigned char* addr,unsigned char n){
  // remplit addr avec l'addresse dela n-eme sonde DS18B20
  // 00000000 en cas d'erreur
  ds.reset_search();    // Réinitialise la recherche de module
  while (ds.search(addr)) {
#ifdef DEBUG
    debugSonde.print(F("addresse 1Wire "));
    for (uint8_t c = 0; c < 8; ++c) debugSonde.print(addr[c], HEX);
    debugSonde.println();
#endif
    if (OneWire::crc8(addr, 7) != addr[7]) {// Vérifie que l'adresse a été correctement reçue
#ifdef DEBUG
      debugSonde.print(F("Erreur lors de la transmission"));
#endif
      addr[0]=0;
      return false;
    }
    if (addr[0] == DS18B20) // Vérifie qu'il s'agit bien d'un DS18B20
      --n;
      if( n==0) return true;
  }
  addr[0]=0;
  return false;
}


bool initRechercheDS18B20(OneWire& ds){
  ds.reset_search();    // Réinitialise la recherche de module
  return true;
}

bool getDS18B20suivant(OneWire& ds,unsigned char* addr){
  while (ds.search(addr)) {
#ifdef DEBUG
    debugSonde.print(F("addresse 1Wire "));
    for (uint8_t c = 0; c < 8; ++c) debugSonde.print(addr[c], HEX);
    debugSonde.println();
#endif
    if (OneWire::crc8(addr, 7) != addr[7]) {// Vérifie que l'adresse a été correctement reçue
#ifdef DEBUG
      debugSonde.print(F("Erreur lors de la transmission"));
#endif
      addr[0]=0;
      return false;
    }
    if (addr[0] == DS18B20) // Vérifie qu'il s'agit bien d'un DS18B20
      return true;
  }
  return false;
}

void finRechercheDS18B20(){
  //rendRessourceSonde();
}
