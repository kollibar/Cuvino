#ifndef __CLAVIER_3T_ANALOG_FREERTOS_H__
#define __CLAVIER_3T_ANALOG_FREERTOS_H__

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


#include <semphr.h>
#include <task.h>
#include <stdint.h>
#include <Arduino.h>
#include <broches.h>
#include <Clavier_3T_Analog.h>


class Clavier_3T_Analog_FreeRTOS : public Clavier_3T_Analog {
public:
  Clavier_3T_Analog_FreeRTOS(FonctionUpdate _fctUpdate);
  Clavier_3T_Analog_FreeRTOS();
  ~Clavier_3T_Analog_FreeRTOS();
  uint8_t getTouche(int16_t attenteTouche);
  uint8_t getToucheAct(bool valDerTouche = true);
  BaseType_t startTask(TaskHandle_t _taskGUI,unsigned short usStackDepth=200, UBaseType_t uxPriority = configUSE_PORT_OPTIMISED_TASK_SELECTION, TaskHandle_t* taskKeyboard = NULL);
  static void fctKeyboard(void* arg);
protected:
  TaskHandle_t taskKeyboard;
  TaskHandle_t taskGUI;
  QueueHandle_t queue;
};

#endif
