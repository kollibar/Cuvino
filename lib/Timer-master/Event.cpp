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

// For Arduino 1.0 and earlier
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Event.h"

Event::Event(void)
{
	eventType = EVENT_NONE;
}

boolean Event::update(void)
{
    unsigned long now = millis();
    return update(now);
}
unsigned long Event::next(void){
	return next(millis());
}
unsigned long Event::next(unsigned long now){
	if( now - lastEventTime >= period) return 0;
	else return (lastEventTime + period - now);
}

boolean Event::update(unsigned long now){
	if (now - lastEventTime >= period)
	{
#ifdef OSCILLATE
		switch (eventType&B11)
		{
			case EVENT_EVERY:
#endif
				switch(eventType&B1100){
					case FCT_VIDE:
						(*callback)();
						break;
					case FCT_BYTE:
						(*(void (*)(uint8_t))callback)(val);
						break;
					case FCT_POINTEUR:
						(*(void (*)(void*))callback)(arg);
						break;
				}
				// if(callback!=NULL) (*callback)();
				// else if(callbackArg!=NULL) (*callbackArg)(arg);
				// else (*callbackByte)(val);
#ifdef OSCILLATE
				break;
			case EVENT_OSCILLATE:
				pinState = ! pinState;
				digitalWrite(pin, pinState);
				break;
		}
#endif
		lastEventTime = now;
		//count++;
		if( count != -1){
			--count;
			if( count == 0) eventType = EVENT_NONE;
		}
		return true;
	}
	return false;
	//if (repeatCount > -1 && count >= repeatCount)
	//{
	//	eventType = EVENT_NONE;
	//}
}
