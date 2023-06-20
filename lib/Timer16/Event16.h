#pragma once

#include "config.h"
#include "define.h"

#ifndef TIMER16_USE_FREERTOS 
#define TIMER16_USE_FREERTOS 0
#endif


#if TIMER16_USE_FREERTOS
#include <Arduino_FreeRTOS.h>
#else
#include <Arduino.h>
#endif
#include <inttypes.h>


#define MASQ_PARAM_4    0b1111
#define EVENT_RESERVED  0b1000

#define MASQ_FCT_4      0b0111
#define FCT_UINT8       0b0001
#define FCT_BYTE		    FCT_UINT8
#define FCT_UINT16      0b0010
#define FCT_2_BYTE      0b0011
#define FCT_POINTEUR	  0b0100
#define FCT_VIDE        0b0101

#define MASQ_FCT_16       0b0111000000000000
#define MASQ_PARAM_16     0b1111000000000000
#define EVENT_RESERVED_16 0b1000000000000000


#define EVENT_MODE_128MS  7     // 128 ms => env 2.3h
#define EVENT_MODE_256MS  8     // 256 ms => env 4.5h
#define EVENT_MODE_512MS  9     // 512 ms => env 9h
#define EVENT_MODE_1024MS  10   // 1024 ms => env 18h
#define EVENT_MODE_2048MS  11   // 2048 ms => env 1.5j
#define EVENT_MODE_4096MS  12   // 4096 ms => env 3j
#define EVENT_MODE_8192MS  13   // 8192 ms => env 6j




class Event16
{

public:
  Event16(void);

  static int8_t setMode(uint8_t mode);
  static uint8_t getMode();
  static uint16_t getPeriodeTimer_ms();

  static inline uint16_t concatenation(uint8_t val1, uint8_t val2);
  static inline uint8_t deconcatenationVal1(uint16_t val);
  static inline uint8_t deconcatenationVal2(uint16_t val);

  static inline uint16_t now16();
  static inline uint16_t date32toTick16(uint32_t date);
  static inline uint32_t tick16toDate32(uint16_t tick);


  int8_t set(uint8_t type, uint32_t period_ms, void (*callback)(void), void* arg, uint16_t repeatCount, bool reserved = false);
  int8_t changeFct(uint8_t type, void (*callback)(void), void* arg);
  inline void setReserved();
  inline void unsetReserved();
  inline bool isReserved();

  inline bool isFreeEvent();

  inline bool isActif();

  uint8_t getValue8();
  uint16_t getValue16(bool force=false);
  void* getPointeurArg();

  uint32_t getPeriod_ms();
  uint32_t getLastEventTime_ms();
  
  int8_t setCount(uint16_t repeatCount);
  uint16_t getCount();

  uint8_t getEventType();
  uint8_t getFctType();

  void stop();

  bool update(void);
  bool update(uint32_t now);
  bool update(uint16_t now);

  uint16_t next(void);
  uint16_t next(uint32_t now);
  uint16_t next(uint16_t now);

  uint32_t next_ms(void);
  uint32_t next_ms(uint32_t now);
  uint32_t next_ms(uint16_t now);

protected:
  bool setEventType(uint8_t type);


  static uint8_t mode;

  uint16_t period_tick;

  void (*callback)(void);
  void* arg;

  uint16_t lastEventTime_tick;

  uint16_t count;
};
