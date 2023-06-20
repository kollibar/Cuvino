#ifndef __FCTDS18B20_H__
#define __FCTDS18B20_H__

#include <Arduino.h>
#include <OneWire.h>

#define DS18B20 0x28     // Adresse 1-Wire du DS18B20

signed char getNbDS18B20(OneWire& ds);
bool getDS18B20(OneWire& ds,unsigned char* addr,unsigned char n);
bool initRechercheDS18B20(OneWire& ds);
bool getDS18B20suivant(OneWire& ds,unsigned char* addr);
void finRechercheDS18B20();

bool getAddresse1Wire(OneWire& ds,unsigned char addr[8]);

#endif
