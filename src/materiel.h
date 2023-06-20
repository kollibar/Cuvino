#ifndef __MATERIEL_H__
#define __MATERIEL_H__

#include <inttypes.h>
#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include <MCP23S17.h>
#include <SPI.h>

#define PORT_MCP23S17_CS  PORTG
#define DDR_MCP23S17_CS   DDRG
#define BIT_MCP23S17_CS   4

#define PORT_PCD8544_CS  PORTD
#define DDR_PCD8544_CS   DDRD
#define BIT_PCD8544_CS   6

#define PORT_PCD8544_DC  PORTD
#define DDR_PCD8544_DC   DDRD
#define BIT_PCD8544_DC   7

#define PORT_PCD8544_RST  PORTG
#define DDR_PCD8544_RST   DDRG
#define BIT_PCD8544_RST   1

#define PORT_MCP2515_CS  PORTG
#define DDR_MCP2515_CS   DDRG
#define BIT_MCP2515_CS   3

#define PORT_TS_CS  PORTD
#define DDR_TS_CS   DDRD
#define BIT_TS_CS   5

#define PORT_busACT  PORTE
#define DDR_busACT   DDRE
#define BIT_busACT   7

#define PORT_NE555   PORTG
#define DDR_NE555    DDRG
#define BIT_NE555    2


void pinMCP23S17(uint8_t etat);
void initCsPinMCP23S17();

void pinMCP2515(uint8_t etat);
void initPinMCP2515();

void pinPCD8544(uint8_t CS, uint8_t DC);
void initPinPCD8544();
void ResetPCD8544();

void initPinTS();
void pinTS(uint8_t etat);

void initPinACT_output();
void initPinACT_intput();
void pinBusACT(uint8_t etat);

void initPinNE555();
void pinNE555(uint8_t etat);

void initPins();

void initMCP23S17(MCP23S17& gpio);
void testMCP23S17(MCP23S17& gpio);

#endif