#pragma once


#include "config.h"
#include "define.h"
#include "Event16.h"
#include <inttypes.h>
#include <Arduino_FreeRTOS.h>
#include <timers.h>






class Timer16
{

public:
  Timer16(void);

  int8_t every(uint32_t period, void (*callback)(void));
  int8_t every(uint32_t period, void (*callback)(void), int repeatCount);
  int8_t after(uint32_t duration, void (*callback)(void));
  int8_t every(uint32_t period, void (*callback)(void*),void*);
  int8_t every(uint32_t period, void (*callback)(void*), void*,int repeatCount);
  int8_t after(uint32_t duration, void (*callback)(void*),void*);
  int8_t every(uint32_t period, void (*callback)(uint8_t),uint8_t);
  int8_t every(uint32_t period, void (*callback)(uint8_t), uint8_t,int repeatCount);
  int8_t after(uint32_t duration, void (*callback)(uint8_t),uint8_t);
  int8_t every(uint32_t period, void (*callback)(uint16_t),uint16_t);
  int8_t every(uint32_t period, void (*callback)(uint16_t), uint16_t,int repeatCount);
  int8_t after(uint32_t duration, void (*callback)(uint16_t),uint16_t);

  int8_t setEventReserve(int8_t id, uint32_t period, void(*callback)(), int repeatCount);
  int8_t setEventReserve(int8_t id, uint32_t period, void(*callback)(void*), void*, int repeatCount);
  int8_t setEventReserve(int8_t id, uint32_t period, void(*callback)(uint8_t), uint8_t, int repeatCount);
  int8_t setEventReserve(int8_t id, uint32_t period, void(*callback)(uint16_t), uint16_t, int repeatCount);
  int8_t setEventReserve(int8_t id, uint32_t period, void(*callback)(uint8_t, uint8_t), uint8_t, uint8_t, int repeatCount);

  int8_t changeFct(int8_t id, void(*callback)());
  int8_t changeFct(int8_t id, void(*callback)(void*), void*);
  int8_t changeFct(int8_t id, void(*callback)(uint8_t), uint8_t);
  int8_t changeFct(int8_t id, void(*callback)(uint16_t), uint16_t);
  int8_t changeFct(int8_t id, void(*callback)(uint8_t, uint8_t), uint8_t, uint8_t);

  uint8_t getValue8(int8_t id);
  uint16_t getValue16(int8_t id, bool force = false);
  void* getPointeurArg(int8_t id);

  void stop(int8_t id);

  uint32_t next(void);
  uint32_t next(uint32_t now);
  void update(void);
  void update(uint32_t now);
  

  int8_t reserve();
  void relache(int8_t id);

  static int8_t setMode(uint8_t mode);
  static uint8_t getMode();
  static uint16_t getPeriodeTimer_ms();

protected:
  uint32_t next(uint16_t now);
  void update(uint16_t now);

  int8_t setEvent(int8_t id, int8_t type, uint32_t period, void (*callback)(void), void* arg, int16_t repeatCount, bool reserved=false);

  Event16 _events[MAX_NUMBER_OF_EVENTS];
  int8_t findFreeEventIndex(void);

};