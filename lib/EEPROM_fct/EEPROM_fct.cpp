#include "EEPROM_fct.h"

// --------------------------- EEPROM --------------------------- //

/*
  Larger AVR processors have larger EEPROM sizes, E.g:
    - Arduno Duemilanove: 512b EEPROM storage.
    - Arduino Uno:        1kb EEPROM storage.
    - Arduino Mega:       4kb EEPROM storage.
    - Arduino Nano: 512 bytes (ATmega168) or 1 KB (ATmega328)
*/
void videEEPROM()
{
  /***
    Iterate through each byte of the EEPROM storage.
  ***/

  for (uint16_t i = 0 ; i < EEPROM.length() ; ++i )
    EEPROM.write(i, 0);

}

uint16_t reserveEEPROM(uint16_t taille, uint16_t adresse){
  taille=adresse+taille;
  for(;adresse<taille;++adresse){
    if ( EEPROM.read(adresse) != 0) EEPROM.write(adresse, 0);
  }
  return adresse;
}

uint16_t ecritEEPROM(byte *objet, uint16_t taille, uint16_t adresse) {
#ifdef DEBUG
  Serial.print("ecritEEPROM(objet@");
  Serial.print(uint16_t(objet), HEX);
  Serial.print(", taille=");
  Serial.print(taille);
  Serial.print(", adresse=");
  Serial.println(adresse);
  //Serial.println(" ! Attention Lecture Seule ! ");
#endif
  for ( uint16_t i = 0; i < taille; ++i) {
    // pour éviter trop d'écriture dans la même cellule
    if ( EEPROM.read(adresse + i) != objet[i]) EEPROM.write(adresse + i, objet[i]);
#ifdef DEBUG
    Serial.print(objet[i], HEX);
#endif
  }
#ifdef DEBUG
  if ( taille == 1 || taille == 2 || taille == 4) {
    Serial.print('(');
    switch (taille) {
      case 1:
        Serial.print("(uint8_t)");
        Serial.print(*objet);
        break;
      case 2:
        Serial.print("(uint16_t)");
        Serial.print((uint16_t)*objet);
        break;
      case 3:
        Serial.print("(uint32_t)");
        Serial.print((uint32_t)*objet);
        break;
    }
    Serial.print(')');
  }
  Serial.println("");
#endif
  return adresse + taille;
}

uint16_t litEEPROM(byte *objet, uint16_t taille, uint16_t adresse) {
#ifdef DEBUG
  Serial.print("litEEPROM(objet@");
  Serial.print(uint16_t(objet), HEX);
  Serial.print(", taille=");
  Serial.print(taille);
  Serial.print(", adresse=");
  Serial.print(adresse);
  Serial.print(")=>");
#endif
  for ( uint16_t i = 0;  i < taille; ++i) {
    objet[i] = EEPROM.read(adresse + i);
#ifdef DEBUG
    Serial.print(objet[i], HEX);
#endif
  }
#ifdef DEBUG
  if ( taille == 1 || taille == 2 || taille == 4) {
    Serial.print('(');
    switch (taille) {
      case 1:
        Serial.print("(uint8_t)");
        Serial.print(*objet);
        break;
      case 2:
        Serial.print("(uint16_t)");
        Serial.print(*((uint16_t*)objet));
        break;
      case 3:
        Serial.print("(uint32_t)");
        Serial.print(*((uint32_t*)objet));
        break;
    }
    Serial.print(')');
  }
  Serial.println("");
#endif
  return adresse + taille;
}

uint32_t lit32EEPROM(uint16_t adresse){
  uint32_t data;
  litEEPROM((uint8_t*)&data, 4, adresse);
  return data;
}

uint8_t lit8EEPROM(uint16_t adresse){
  return EEPROM.read(adresse);
}

uint16_t crc16Memoire(uint16_t debut, uint16_t taille) {
  uint16_t crc = 0xFFFF;
  for (uint16_t i = debut; i < debut + taille; ++i) {
    crc = crc ^ EEPROM.read(i);
    for (byte j = 0; j < 8; ++j) {
      if ( (crc & 1) == 1) crc = (crc / 2) ^ 0xA001;
      else crc = crc >> 1;
    }
  }
  return crc;
}


uint8_t crc8OneWireMemoire(uint16_t debut, uint16_t taille) {
  uint8_t crc = 0;

  while (taille--) {
    uint8_t inbyte = EEPROM.read(debut++);
    for (uint8_t i = 8; i; i--) {
      uint8_t mix = (crc ^ inbyte) & 0x01;
      crc >>= 1;
      if (mix) crc ^= 0x8C;
      inbyte >>= 1;
    }
  }
  return crc;
}
