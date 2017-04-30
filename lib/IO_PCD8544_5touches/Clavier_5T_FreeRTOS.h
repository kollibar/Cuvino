#ifndef __CLAVIER_5T_FREERTOS_H__
#define __CLAVIER_5T_FREERTOS_H__

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
#include <Clavier_5T.h>


class Clavier_5T_FreeRTOS : public Clavier_5T {
public:
  Clavier_5T_FreeRTOS(FonctionUpdate _fctUpdate);
  Clavier_5T_FreeRTOS();
  ~Clavier_5T_FreeRTOS();
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
