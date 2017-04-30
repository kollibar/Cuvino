#include "Clavier_3T_analog_FreeRTOS.h"


Clavier_3T_Analog_FreeRTOS::Clavier_3T_Analog_FreeRTOS(FonctionUpdate _fctUpdate):Clavier_3T_Analog(_fctUpdate){
  taskKeyboard=NULL;
}
Clavier_3T_Analog_FreeRTOS::Clavier_3T_Analog_FreeRTOS():Clavier_3T_Analog(){
  taskKeyboard=NULL;
}
Clavier_3T_Analog_FreeRTOS::~Clavier_3T_Analog_FreeRTOS(){
  vTaskDelete( NULL );
}

uint8_t Clavier_3T_Analog_FreeRTOS::getTouche(int16_t attenteTouche){
  // attend qu'une touche soit pressé et renvoi le code de la touche
  // temps d'attente avant de quitter la routine. -2 attentes infini, >=0 attente en seconde, -1 attente que la LED de l'écran soit eteinte
  if( attenteTouche == 0 ) return getToucheAct();

  if( taskKeyboard==NULL) return Clavier_3T_Analog::getTouche(attenteTouche); // si task FreeRTOS non démarée alors prends la versions sans !

  unsigned int attente;
  if( attenteTouche > 0) attente=attenteTouche;
  else attente=0xFFFF;

  unsigned char touche;

  do{
    if( ! xQueueReceive(queue, &touche, 1000 / portTICK_PERIOD_MS) ){
      touche=TIMEOUT_DECOMPTE;
    } else{
      if( attenteTouche!=2 || touche!=TIMEOUT_LED) break;
    }
    attente--;
  } while( attenteTouche<0 || attente!=0);
  #ifdef DEBUG
  Serial.print("receive touche:");
  Serial.println(touche,BIN);
  #endif
  return touche;
}

BaseType_t Clavier_3T_Analog_FreeRTOS::startTask(TaskHandle_t _taskGUI,unsigned short usStackDepth, UBaseType_t uxPriority, TaskHandle_t* _taskKeyboard){
  BaseType_t retour;
  if( taskKeyboard ==NULL){
    retour = xTaskCreate( Clavier_3T_Analog_FreeRTOS::fctKeyboard,
                            (const portCHAR *)"kb_3Ta",
                            usStackDepth,
                            (void *)this,
                            uxPriority,
                            &taskKeyboard
                          );
    queue = xQueueCreate(1, sizeof(char));
    if( _taskKeyboard !=NULL) *_taskKeyboard=taskKeyboard;
    dateDerniereTouche=0;
  }

  taskGUI=_taskGUI;

  return retour;
}

void Clavier_3T_Analog_FreeRTOS::fctKeyboard(void* arg){
  Clavier_3T_Analog_FreeRTOS* clavier=(Clavier_3T_Analog_FreeRTOS*)arg;
  for(;;){
    uint32_t date;
    unsigned char touche;
    for(;;){
      touche=clavier->getToucheAct();
      vTaskDelay(25 / portTICK_PERIOD_MS); // 50 ms
      if( clavier->getToucheAct() == touche) break;
    }

    date=(uint32_t)xTaskGetTickCount();
    if( touche !=0){
      if( touche != clavier->derniereTouche || (date - clavier->delaiTouche)>clavier->dateDerniereTouche ){
        #ifdef DEBUG
        Serial.print("lastTouche:");
        Serial.print(clavier->derniereTouche,BIN);
        Serial.print(" date:");
        Serial.print(date);
        Serial.print(" dateLastTouche:");
        Serial.print(clavier->dateDerniereTouche);
        Serial.print(" delaiLastTouche:");
        Serial.println(clavier->delaiTouche);
        Serial.print("send touche:");
        Serial.println(touche,BIN);
        #endif

        if( ! xQueueSend(clavier->queue, &touche, 100 / portTICK_PERIOD_MS)){
          xQueueReceive(clavier->queue, &touche, 0);
        }

        clavier->dateDerniereTouche = date;
        clavier->derniereTouche=touche;

        if( touche==clavier->derniereTouche) clavier->delaiTouche=DELAI_REDUIT / portTICK_PERIOD_MS;
        else clavier->delaiTouche=DELAI_STANDARD / portTICK_PERIOD_MS;
      }
    }else{
      clavier->delaiTouche=DELAI_STANDARD / portTICK_PERIOD_MS;
      clavier->derniereTouche=0;

      if( ((date - clavier->dateDerniereTouche) > (60000 / portTICK_PERIOD_MS)) && clavier->etatEcran()) {
        clavier->onOffEcran(false);
        touche=TIMEOUT_LED;
        xQueueSend(clavier->queue, &touche, 100 / portTICK_PERIOD_MS);
      }
    }
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

uint8_t Clavier_3T_Analog_FreeRTOS::getToucheAct(bool valDerTouche) {
  // renvoi la valeur de la touche actuellement pressé
  if( taskKeyboard==NULL) Clavier_3T_Analog::getToucheAct(valDerTouche); // si la task FreeRTOS n'est pas encore active -> fonctionnement normal

  int16_t val = 0;
  int16_t val2;
  uint8_t touche;
  for (;;) {
    do {
      val2 = val;
      //fctUpdate();

      if ( (PIN_CLAVIER & ( 1 << BIT_CLAVIER )) == 0) return 0; // si la lecture numérique retourne 0 (càd Tension <2,5V) -> pas de touche
      val = analogRead(BROCHE_CLAVIER); // lecture analogique

    } while (val != val2);

    onOffEcran(true);
//#ifdef PORT_LED_ECRAN
//    PORT_LED_ECRAN |= (1 << BIT_LED_ECRAN); // on ralume l'écran
//#endif

    touche = 0;
    if ( val > 500) {
      if ( val < 580 ) touche = BT_HAUT;
      else if ( val < 650 ) touche = BT_BAS;
      else if ( val < 720 ) touche = BT_SELECT;
      else if ( val < 765 ) touche = BT_HAUT | BT_BAS;
      else if ( val < 794 ) touche = BT_HAUT | BT_SELECT;
      else if ( val < 824 ) touche = BT_BAS | BT_SELECT;
      else if ( val < 900 ) touche = BT_BAS | BT_HAUT | BT_SELECT;
    }

    if (touche != 0) {
      return touche;
    }
  }
}
