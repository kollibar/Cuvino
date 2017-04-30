#ifndef __TEMPDS3231_H__
#define __TEMPDS3231_H__

/** FreeRTOS **/
#include <Arduino_FreeRTOS.h>
#include <projdefs.h>
#include <portmacro.h>
#include <list.h>
#include <timers.h>
#include <event_groups.h>
#include <semphr.h>
#include <FreeRTOSVariant.h>
#include <queue.h>
#include <FreeRTOSConfig.h>
#include <croutine.h>
#include <StackMacros.h>
#include <mpu_wrappers.h>
#include <task.h>
#include <portable.h>
/** !FreeRTOS **/

#include <broches.h>
#include <RtcDS3231.h>
#include <Wire.h>
#include <Timer.h>

#define ADDRESSE_DS3231 0x68
#define DS3231_CMD_TEMP 0x11

bool addrIsDS3231(unsigned char addr[8]);
signed int getTemperatureDS3231();

#endif
