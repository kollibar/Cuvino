#ifndef __BROCHES_H__
#define __BROCHES_H__

#ifdef NB_EV_MAX
  #if NB_EV_MAX > 9
    #undef NB_EV_MAX
    #define NB_EV_MAX 9
  #endif
#else
  #define NB_EV_MAX 9
#endif

// BROCHE RESET
/*
#define BROCHE_RESET  9
#define PORT_RESET    PORTH
#define DDR_RESET     DDRH
#define PIN_RESET     PINH
#define BIT_RESET     6*/

// BROCHE ONEWIRE (SONDE DE TEMPERATURE)/*
/*#define BROCHE_ONEWIRE 5 // Broche utilisée pour le bus 1-Wire
#define PORT_ONEWIRE PORTG
#define DDR_ONEWIRE DDRG
#define PIN_ONEWIRE PING
#define BIT_ONEWIRE 2*/

// ECRAN

#define BROCHE_ECRAN_SCLK 52 //7   / clock       (display pin 2) /  // SPI_hardware -> 52
#define BROCHE_ECRAN_SDIN 51 //6   /* data-in     (display pin 3) / // MOSI   // SPI_hardware -> 51
#define BROCHE_ECRAN_DC 38  //PD7   /* data select (display pin 4) /
#define BROCHE_ECRAN_RESET 40 //PG1  /* reset       (display pin 8) /
#define BROCHE_ECRAN_CS 70   // PD6 /* enable      (display pin 5) / // pas obligatoire si le LCD est l'unique élément sur la ligne SP
/*
#define BROCHE_ECRAN_DC 5  //PE3   /* data select (display pin 4) /
#define BROCHE_ECRAN_RESET 0 //Reset  /* reset       (display pin 8) /
#define BROCHE_ECRAN_CS 3   // PE5 /* enable      (display pin 5) / // pas obligatoire si le LCD est l'unique élément sur la ligne SPI
*/
// LED ECRAN
#define BROCHE_LED_ECRAN  41  // PG0
#define PORT_LED_ECRAN    PORTG  // port sur lequel est branché la led de l'écran (broche 22)
#define DDR_LED_ECRAN     DDRG    // registre de mode du port
#define BIT_LED_ECRAN     0       // pin correspondant à la led de l'écran


  // CLAVIER
//  #define BROCHE_CLAVIER        A4
//  #define PORT_CLAVIER          PORTF
//  #define DDR_CLAVIER           DDRF
//  #define PIN_CLAVIER           PINF
//  #define BIT_CLAVIER           4

  #define PORT_CLAVIER    PORTK
  #define DDR_CLAVIER     DDRK
  #define PIN_CLAVIER     PINK
  #define MASQUE_CLAVIER  0b01111100

  #define BROCHE_CLAVIER_HAUT   A10
  #define BIT_CLAVIER_HAUT      2
  #define BROCHE_CLAVIER_BAS    A11
  #define BIT_CLAVIER_BAS       3
  #define BROCHE_CLAVIER_GAUCHE A12
  #define BIT_CLAVIER_GAUCHE    4
  #define BROCHE_CLAVIER_DROITE A13
  #define BIT_CLAVIER_DROITE    5
  #define BROCHE_CLAVIER_MENU A14
  #define BIT_CLAVIER_MENU    6


#endif
