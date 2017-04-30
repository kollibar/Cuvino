#ifndef __EEPROM_FCT_H__
#define __EEPROM_FCT_H__

#include <EEPROM.h>
#include <Arduino.h>

// pour passer directement par les fonctions eeprom_read_byte/eeprom_write_byte
#include <inttypes.h>
#include <avr/eeprom.h>
#include <avr/io.h>


//#define DEBUG

// --------------------------- EEPROM --------------------------- //
#define ADRESSE_CRC_V0_1 0 // unsigned int
#define ADRESSE_TAILLE_V0_1 2 //unsigned int
#define ADRESSE_VERSION_MEMOIRE_V0_1 4 // char
#define ADRESSE_VERSION_LOGICIEL_V0_1 5 // char

void videEEPROM();
uint16_t ecritEEPROM(byte *objet, uint16_t taille, uint16_t adresse);
uint16_t litEEPROM(byte *objet, uint16_t taille, uint16_t adresse);
uint8_t lit8EEPROM(uint16_t adresse);
uint32_t lit32EEPROM(uint16_t adresse);
uint16_t crc16Memoire(uint16_t debut, uint16_t taille);
uint16_t reserveEEPROM(uint16_t taille, uint16_t adresse);

uint8_t crc8OneWireMemoire(uint16_t debut, uint16_t taille);

#endif
