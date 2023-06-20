#include "materiel.h"

void pinMCP23S17(uint8_t etat){
  if( etat ) {
    PORT_MCP23S17_CS |= ( 1 << BIT_MCP23S17_CS );
  } else {
    PORT_MCP23S17_CS &= ~( 1 << BIT_MCP23S17_CS );
  }
}
void initPinMCP23S17(){
  DDR_MCP23S17_CS |= ( 1 << BIT_MCP23S17_CS );
  PORT_MCP23S17_CS |= ( 1 << BIT_MCP23S17_CS );
}

void pinNE555(uint8_t etat){
  if( etat ) {
    PORT_NE555 |= ( 1 << BIT_NE555 );
  } else {
    PORT_NE555 &= ~( 1 << BIT_NE555 );
  }
}
void initPinNE555(){
  DDR_NE555 |= ( 1 << BIT_NE555 );
  PORT_NE555 &= ~( 1 << BIT_NE555 );
}

void pinMCP2515(uint8_t etat){
  if( etat ) {
    PORT_MCP2515_CS |= ( 1 << BIT_MCP2515_CS );
  } else {
    PORT_MCP2515_CS &= ~( 1 << BIT_MCP2515_CS );
  }
}
void initPinMCP2515(){
  DDR_MCP2515_CS |= ( 1 << BIT_MCP2515_CS );
  PORT_MCP2515_CS |= ( 1 << BIT_MCP2515_CS );
}

void pinPCD8544(uint8_t CS, uint8_t DC){
  if( DC ) {
    PORT_PCD8544_DC |= ( 1 << BIT_PCD8544_DC );
  } else {
    PORT_PCD8544_DC &= ~( 1 << BIT_PCD8544_DC );
  }

  if( CS ) {
    PORT_PCD8544_CS |= ( 1 << BIT_PCD8544_CS );
    PORT_PCD8544_DC |= ( 1 << BIT_PCD8544_DC ); // pour contrer erreur de cablage
  } else {
    PORT_PCD8544_CS &= ~( 1 << BIT_PCD8544_CS );
  }
}
void initPinPCD8544(){
  // CS
  DDR_PCD8544_CS |= ( 1 << BIT_PCD8544_CS );
  PORT_PCD8544_CS |= ( 1 << BIT_PCD8544_CS );
  // DC
  DDR_PCD8544_DC |= ( 1 << BIT_PCD8544_DC );
  // RST
  DDR_PCD8544_RST |= ( 1 << BIT_PCD8544_RST );
  PORT_PCD8544_RST |= ( 1 << BIT_PCD8544_RST );
}

void initPinTS(){
  // CS
  DDR_TS_CS |= ( 1 << BIT_TS_CS );
  PORT_TS_CS |= ( 1 << BIT_TS_CS );
}
void pinTS(uint8_t etat){
  if( etat ) {
    PORT_TS_CS |= ( 1 << BIT_TS_CS );
  } else {
    PORT_TS_CS &= ~( 1 << BIT_TS_CS );
  }
}

void ResetPCD8544(){
  PORT_PCD8544_RST &= ~( 1 << BIT_PCD8544_RST );
  delay(500);
  PORT_PCD8544_RST |= ( 1 << BIT_PCD8544_RST );
}

void initPinACT_output(){
  DDR_busACT |= ( 1 << BIT_busACT );
}

void initPinACT_intput(){
  DDR_busACT &= ~( 1 << BIT_busACT );
}

void pinBusACT(uint8_t etat){
  if( etat ) {
    PORT_busACT |= ( 1 << BIT_busACT );
  } else {
    PORT_busACT &= ~( 1 << BIT_busACT );
  }
}

void initPins(){
    initPinPCD8544();
    initPinMCP2515();
    initPinMCP23S17();
    initPinTS();

    initPinNE555();
    pinNE555(1);

    initPinACT_output();
    pinBusACT(1);
}

void initMCP23S17(MCP23S17& gpio){
  initPinMCP23S17();
  gpio.setSPIspeed(4000000);
  gpio.begin();
  gpio.writeReg(MCP23S17_IOCON, MCP23S17_IOCR_MIRROR | MCP23S17_IOCR_HAEN | MCP23S17_IOCR_ODR);
  gpio.writeRegWord(MCP23S17_IODIR, 0xFFFF);  // met tous les pins en entrée
  gpio.writeRegWord(MCP23S17_GPPU, 0xFFFF);  // met tous les pins en pull up
  gpio.writeRegWord(MCP23S17_IPOL, 0x0000);
}

void testMCP23S17(MCP23S17& gpio){
  
  Serial.print(F("gpio: IODIR="));
  Serial.println(gpio.readReg(MCP23S17_IODIR), BIN);

  Serial.print(F("gpio: IPOL="));
  Serial.println(gpio.readReg(MCP23S17_IPOL), BIN);

  Serial.print(F("gpio: GPINTEN="));
  Serial.println(gpio.readReg(MCP23S17_GPINTEN), BIN);

  Serial.print(F("gpio: DEFVAL="));
  Serial.println(gpio.readRegWord(MCP23S17_DEFVAL), BIN);

  Serial.print(F("gpio: INTCON="));
  Serial.println(gpio.readRegWord(MCP23S17_INTCON), BIN);

  Serial.print(F("gpio: IOCON="));
  Serial.println(gpio.readReg(MCP23S17_IOCON), BIN);

  Serial.print(F("gpio: GPPU="));
  Serial.println(gpio.readRegWord(MCP23S17_GPPU), BIN);

  Serial.print(F("gpio: INTF="));
  Serial.println(gpio.readRegWord(MCP23S17_INTF), BIN);

  Serial.print(F("gpio: INTCAP="));
  Serial.println(gpio.readRegWord(MCP23S17_INTCAP), BIN);

  Serial.print(F("gpio: GPIO="));
  Serial.println(gpio.readRegWord(MCP23S17_GPIO), BIN);

  Serial.print(F("gpio: OLAT="));
  Serial.println(gpio.readRegWord(MCP23S17_OLAT), BIN);

  gpio.writeReg(MCP23S17_IPOL,0b01010101);
  Serial.print(F("gpio: IPOL="));
  Serial.println(gpio.readRegWord(MCP23S17_IPOL), BIN);


  for(uint32_t i =0;i<650000;i++){
    
    gpio.writeReg(MCP23S17_IPOL, gpio.readReg(MCP23S17_IPOL)<<1);
  }
  Serial.print(F("gpio: IPOL="));
  Serial.println(gpio.readRegWord(MCP23S17_IPOL), BIN);

  delay(5000);
}