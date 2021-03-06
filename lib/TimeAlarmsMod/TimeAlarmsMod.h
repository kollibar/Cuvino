//  TimeAlarms.h - Arduino Time alarms header for use with Time library

#ifndef TimeAlarmsMod_h
#define TimeAlarmsMod_h

#include <inttypes.h>
#include <stddef.h>

#include "Time.h"

#ifndef dtNBR_ALARMS
	#define dtNBR_ALARMS 6   // max is 255
#endif

#define USE_SPECIALIST_METHODS  // define this for testing

typedef  void* pointeurArg;

typedef enum { dtMillisecond, dtSecond, dtMinute, dtHour, dtDay } dtUnits_t;

typedef struct  {
	uint8_t alarmType              :4 ;  // enumeration of daily/weekly (in future: biweekly/semimonthly/monthly/annual)
	                                     // note that the current API only supports daily or weekly alarm periods
    uint8_t isEnabled              :1 ;  // the timer is only actioned if isEnabled is true 
    uint8_t isOneShot              :1 ;  // the timer will be de-allocated after trigger is processed 
										 }
    AlarmMode_t   ;
	
// new time based alarms should be added just before dtLastAlarmType
typedef enum  {dtNotAllocated, dtTimer, dtExplicitAlarm, dtDailyAlarm, dtWeeklyAlarm, dtLastAlarmType } dtAlarmPeriod_t ; // in future: dtBiweekly, dtMonthly, dtAnnual

// macro to return true if the given type is a time based alarm, false if timer or not allocated
#define dtIsAlarm(_type_)  (_type_ >= dtExplicitAlarm && _type_ < dtLastAlarmType) 

typedef uint8_t AlarmID_t;
typedef AlarmID_t AlarmId;  // Arduino friendly name

#define dtINVALID_ALARM_ID 255
#define dtINVALID_TIME     0L

class AlarmClass;  // forward reference
typedef void (*OnTick_t)();  // alarm callback function typedef 
typedef void (*OnTick_arg_t)(pointeurArg); // alarm callback function typedef  avec argument pointeurArg

// class defining an alarm instance, only used by dtAlarmsClass
class AlarmClass
{  
private:

public:
  AlarmClass(); 
  OnTick_t onTickHandler; 
  OnTick_arg_t onTickArgHandler; 
  void updateNextTrigger();
  time_t value;
  time_t nextTrigger;
  AlarmMode_t Mode;
  pointeurArg arg;
};

// class containing the collection of alarms
class TimeAlarmsClass
{
private:
   AlarmClass Alarm[dtNBR_ALARMS];
   void serviceAlarms();
   uint8_t isServicing;
   uint8_t servicedAlarmId; // the alarm currently being serviced
   AlarmID_t create( time_t value, OnTick_t onTickHandler, uint8_t isOneShot, dtAlarmPeriod_t alarmType, uint8_t isEnabled=true);
   AlarmID_t create( time_t value, OnTick_arg_t onTickHandler,pointeurArg arg, uint8_t isOneShot, dtAlarmPeriod_t alarmType, uint8_t isEnabled=true);
   

public:
  TimeAlarmsClass();
  // functions to create alarms and timers

  AlarmID_t triggerOnce(time_t value, OnTick_t onTickHandler);   // trigger once at the given time_t

  AlarmID_t alarmRepeat(time_t value, OnTick_t onTickHandler);                    // trigger daily at given time of day
  AlarmID_t alarmRepeat(const int H,  const int M,  const int S, OnTick_t onTickHandler); // as above, with hms arguments
  AlarmID_t alarmRepeat(const timeDayOfWeek_t DOW, const int H,  const int M,  const int S, OnTick_t onTickHandler); // as above, with day of week 
 
  AlarmID_t alarmOnce(time_t value, OnTick_t onTickHandler);                     // trigger once at given time of day
  AlarmID_t alarmOnce( const int H,  const int M,  const int S, OnTick_t onTickHandler);  // as above, with hms arguments
  AlarmID_t alarmOnce(const timeDayOfWeek_t DOW, const int H,  const int M,  const int S, OnTick_t onTickHandler); // as above, with day of week 
  
  AlarmID_t timerOnce(time_t value, OnTick_t onTickHandler);   // trigger once after the given number of seconds 
  AlarmID_t timerOnce(const int H,  const int M,  const int S, OnTick_t onTickHandler);   // As above with HMS arguments
  
  AlarmID_t timerRepeat(time_t value, OnTick_t onTickHandler); // trigger after the given number of seconds continuously
  AlarmID_t timerRepeat(const int H,  const int M,  const int S, OnTick_t onTickHandler);   // As above with HMS arguments


  // les mêmes avec un pointeurArg
  AlarmID_t triggerOnce(time_t value, OnTick_arg_t onTickHandler,pointeurArg arg);   // trigger once at the given time_t

  AlarmID_t alarmRepeat(time_t value, OnTick_arg_t onTickHandler,pointeurArg arg);                    // trigger daily at given time of day
  AlarmID_t alarmRepeat(const int H,  const int M,  const int S, OnTick_arg_t onTickHandler,pointeurArg arg); // as above, with hms arguments
  AlarmID_t alarmRepeat(const timeDayOfWeek_t DOW, const int H,  const int M,  const int S, OnTick_arg_t onTickHandler,pointeurArg arg); // as above, with day of week 
 
  AlarmID_t alarmOnce(time_t value, OnTick_arg_t onTickHandler,pointeurArg arg);                     // trigger once at given time of day
  AlarmID_t alarmOnce( const int H,  const int M,  const int S, OnTick_arg_t onTickHandler,pointeurArg arg);  // as above, with hms arguments
  AlarmID_t alarmOnce(const timeDayOfWeek_t DOW, const int H,  const int M,  const int S, OnTick_arg_t onTickHandler,pointeurArg arg); // as above, with day of week 
  
  AlarmID_t timerOnce(time_t value, OnTick_arg_t onTickHandler,pointeurArg arg);   // trigger once after the given number of seconds 
  AlarmID_t timerOnce(const int H,  const int M,  const int S, OnTick_arg_t onTickHandler,pointeurArg arg);   // As above with HMS arguments
  
  AlarmID_t timerRepeat(time_t value, OnTick_arg_t onTickHandler,pointeurArg arg); // trigger after the given number of seconds continuously
  AlarmID_t timerRepeat(const int H,  const int M,  const int S, OnTick_arg_t onTickHandler,pointeurArg arg);   // As above with HMS arguments
  
  void delay(unsigned long ms);
   
  // utility methods
  uint8_t getDigitsNow( dtUnits_t Units);         // returns the current digit value for the given time unit
  void waitForDigits( uint8_t Digits, dtUnits_t Units);  
  void waitForRollover(dtUnits_t Units);
 
  // low level methods 
  void enable(AlarmID_t ID);                // enable the alarm to trigger
  void disable(AlarmID_t ID);               // prevent the alarm from triggering   
  AlarmID_t getTriggeredAlarmId();          // returns the currently triggered  alarm id
  void write(AlarmID_t ID, time_t value);   // write the value (and enable) the alarm with the given ID  
  time_t read(AlarmID_t ID);                // return the value for the given timer  
  dtAlarmPeriod_t readType(AlarmID_t ID);   // return the alarm type for the given alarm ID 
  
#ifndef USE_SPECIALIST_METHODS  
private:  // the following methods are for testing and are not documented as part of the standard library
#endif
  void free(AlarmID_t ID);                  // free the id to allow its reuse 
  uint8_t count();                          // returns the number of allocated timers
  time_t getNextTrigger();                  // returns the time of the next scheduled alarm
  bool isAllocated(AlarmID_t ID);           // returns true if this id is allocated  
  bool isAlarm(AlarmID_t ID);               // returns true if id is for a time based alarm, false if its a timer or not allocated
};

extern TimeAlarmsClass Alarm;  // make an instance for the user

/*==============================================================================
 * MACROS
 *============================================================================*/

/* public */
#define waitUntilThisSecond(_val_) waitForDigits( _val_, dtSecond)
#define waitUntilThisMinute(_val_) waitForDigits( _val_, dtMinute)
#define waitUntilThisHour(_val_)   waitForDigits( _val_, dtHour)
#define waitUntilThisDay(_val_)    waitForDigits( _val_, dtDay)
#define waitMinuteRollover() waitForRollover(dtSecond)
#define waitHourRollover()   waitForRollover(dtMinute)
#define waitDayRollover()    waitForRollover(dtHour)

#define AlarmHMS(_hr_, _min_, _sec_) (_hr_ * SECS_PER_HOUR + _min_ * SECS_PER_MIN + _sec_)


#endif /* TimeAlarmsMod_h */

