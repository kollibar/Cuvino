#ifndef __BROCHES_H__
#define __BROCHES_H__

#ifdef NB_EV_MAX
  #if NB_EV_MAX > 6
    #undef NB_EV_MAX
    #define NB_EV_MAX 6
  #endif
#endif

// BROCHE RESET
#define BROCHE_RESET  9
#define PORT_RESET    PORTH
#define DDR_RESET     DDRH
#define PIN_RESET     PINH
#define BIT_RESET     6

// BROCHE ONEWIRE (SONDE DE TEMPERATURE)
#define BROCHE_ONEWIRE 39 // Broche utilisée pour le bus 1-Wire
#define PORT_ONEWIRE PORTG
#define DDR_ONEWIRE DDRG
#define PIN_ONEWIRE PING
#define BIT_ONEWIRE 2

// ECRAN
#define BROCHE_ECRAN_SCLK 52 //7   /* clock       (display pin 2) */  // SPI_hardware -> 52
#define PORT_ECRAN_SCLK PORTB
#define DDR_ECRAN_SCLK DDRB
#define BIT_ECRAN_SCLK 1
#define BROCHE_ECRAN_SDIN 51 //6   /* data-in     (display pin 3) */ // MOSI   // SPI_hardware -> 51
#define PORT_ECRAN_SDIN PORTB
#define DDR_ECRAN_SDIN DDRB
#define BIT_ECRAN_SDIN 2
#define BROCHE_ECRAN_DC 41     /* data select (display pin 4) */
#define PORT_ECRAN_DC PORTG
#define DDR_ECRAN_DC DDRG
#define BIT_ECRAN_DC 0
#define BROCHE_ECRAN_RESET 40  /* reset       (display pin 8) */
#define PORT_ECRAN_RESET PORTG
#define DDR_ECRAN_RESET DDRG
#define BIT_ECRAN_RESET 1
#define BROCHE_ECRAN_CS 53    /* enable      (display pin 5) */ // pas obligatoire si le LCD est l'unique élément sur la ligne SPI
#define PORT_ECRAN_CS PORTB
#define DDR_ECRAN_CS DDRB
#define BIT_ECRAN_SC 0

// LED ECRAN
#define BROCHE_LED_ECRAN  17  // PH0
#define PORT_LED_ECRAN    PORTH  // port sur lequel est branché la led de l'écran (broche 22)
#define DDR_LED_ECRAN     DDRH    // registre de mode du port
#define BIT_LED_ECRAN     0       // pin correspondant à la led de l'écran

// Commandes Electrovannes (attention, les 2 broches de chaques electrovannes DOIVENT être sur le même port)
#define BROCHE_EV_0_O   22    // relais 0_1 -> broche 17
#define BROCHE_EV_0_F   23    // relais 0_2 -> broche 16
#define PORT_EV_0       PORTA
#define DDR_EV_0        DDRA
#define BIT_EV_0_O      0
#define BIT_EV_0_F      1
#define MASQUE_EV_0     B11

#define BROCHE_EV_1_O   24    // relais 1_1 -> broche 19
#define BROCHE_EV_1_F   25    // relais 1_2 -> broche 18
#define PORT_EV_1       PORTA
#define DDR_EV_1        DDRA
#define BIT_EV_1_O      2
#define BIT_EV_1_F      3
#define MASQUE_EV_1     B1100

#define BROCHE_EV_2_O   26    // relais 2_1 -> broche 22
#define BROCHE_EV_2_F   27    // relais 2_2 -> broche 23
#define PORT_EV_2       PORTA
#define DDR_EV_2        DDRA
#define BIT_EV_2_O      4
#define BIT_EV_2_F      5
#define MASQUE_EV_2     B110000

#define BROCHE_EV_3_O   28    // relais 3_1 -> broche 24
#define BROCHE_EV_3_F   29   // relais 3_2 -> broche 25
#define PORT_EV_3       PORTA
#define DDR_EV_3        DDRA
#define BIT_EV_3_O      6
#define BIT_EV_3_F      7
#define MASQUE_EV_3     B11000000


// Etat des électrovannes
#define BROCHE_E_EV_0_O 30
#define BROCHE_E_EV_0_F 31
#define PORT_E_EV_0     PORTC
#define DDR_E_EV_0      DDRC
#define PIN_E_EV_0      PINC
#define BIT_E_EV_0_O    7
#define BIT_E_EV_0_F    6
#define MASQUE_E_EV_0   B11000000

#define BROCHE_E_EV_1_O 32
#define BROCHE_E_EV_1_F 33
#define PORT_E_EV_1     PORTC
#define DDR_E_EV_1      DDRC
#define PIN_E_EV_1      PINC
#define BIT_E_EV_1_O    5
#define BIT_E_EV_1_F    4
#define MASQUE_E_EV_1   B110000

#define BROCHE_E_EV_2_O 34
#define BROCHE_E_EV_2_F 35
#define PORT_E_EV_2     PORTC
#define DDR_E_EV_2      DDRC
#define PIN_E_EV_2      PINC
#define BIT_E_EV_2_O    3
#define BIT_E_EV_2_F    2
#define MASQUE_E_EV_2   B11000000

#define BROCHE_E_EV_3_O 36
#define BROCHE_E_EV_3_F 37
#define PORT_E_EV_3     PORTC
#define DDR_E_EV_3      DDRC
#define PIN_E_EV_3      PINC
#define BIT_E_EV_3_O    1
#define BIT_E_EV_3_F    0
#define MASQUE_E_EV_3   B11

#define BROCHE_E_EV_0   A0
#define BROCHE_E_EV_1   A1
#define BROCHE_E_EV_2   A2
#define BROCHE_E_EV_3   A3
#define PORT_E_EV       PORTF
#define DDR_E_EV        DDRF
#define PIN_E_EV        PINF
#define MASQUE_E_EV     B1111
#define BIT_E_EV_0       0
#define BIT_E_EV_1       1
#define BIT_E_EV_2       2
#define BIT_E_EV_3       3

  // CLAVIER
//  #define BROCHE_CLAVIER        A4
//  #define PORT_CLAVIER          PORTF
//  #define DDR_CLAVIER           DDRF
//  #define PIN_CLAVIER           PINF
//  #define BIT_CLAVIER           4

#define PORT_CLAVIER    PORTF
#define DDR_CLAVIER     DDRF
#define PIN_CLAVIER     PINF
#define MASQUE_CLAVIER  B11111000

#define BROCHE_CLAVIER_HAUT   A5
#define BIT_CLAVIER_HAUT      5
#define BROCHE_CLAVIER_BAS    A4
#define BIT_CLAVIER_BAS       4
#define BROCHE_CLAVIER_GAUCHE A3
#define BIT_CLAVIER_GAUCHE    3
#define BROCHE_CLAVIER_DROITE A6
#define BIT_CLAVIER_DROITE    6
#define BROCHE_CLAVIER_MENU   A7
#define BIT_CLAVIER_MENU      7

// LED DE LA CARTE
#define BROCHE_LED  13
#define PORT_LED    PORTB
#define DDR_LED     DDRB
#define BIT_LED     7

#endif
