/*
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

/*  * * * * * * * * * * * * * * * * * * * * * * * * * * *
 Code by Simon Monk
 http://www.simonmonk.org
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef Event_h
#define Event_h

#include <inttypes.h>

#define EVENT_NONE 0
#define EVENT_EVERY 1
#define EVENT_OSCILLATE 2

#define FCT_VIDE		B0000
#define FCT_BYTE		B0100
#define FCT_POINTEUR	B1000

class Event
{

public:
  Event(void);
  bool update(void);
  bool update(unsigned long now);
  unsigned long next(void);
  unsigned long next(unsigned long now);
  int8_t eventType;
  unsigned long period;
 // int repeatCount;
#ifdef OSCILLATE
  uint8_t pin;
  uint8_t pinState;
#endif
  void (*callback)(void);
  // void (*callbackArg)(void*);
  // void (*callbackByte)(uint8_t);
  void* arg;
  uint8_t val;
  unsigned long lastEventTime;
  int count;
};

#endif
