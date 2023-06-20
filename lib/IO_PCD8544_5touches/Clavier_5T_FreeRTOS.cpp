#include "Clavier_5T_FreeRTOS.h"


Clavier_5T_FreeRTOS::Clavier_5T_FreeRTOS(FonctionUpdate _fctUpdate):Clavier_5T(_fctUpdate){
  taskKeyboard=NULL;
}
Clavier_5T_FreeRTOS::Clavier_5T_FreeRTOS():Clavier_5T(){
  taskKeyboard=NULL;
}
Clavier_5T_FreeRTOS::~Clavier_5T_FreeRTOS(){
  vTaskDelete( NULL );
}

uint8_t Clavier_5T_FreeRTOS::getTouche(int16_t attenteTouche){
  // attend qu'une touche soit pressé et renvoi le code de la touche
  // temps d'attente avant de quitter la routine. -2 attentes infini, >=0 attente en seconde, -1 attente que la LED de l'écran soit eteinte
  if( attenteTouche == 0 ) return getToucheAct();

  if( taskKeyboard==NULL) return Clavier_5T::getTouche(attenteTouche); // si task FreeRTOS non démarée alors prends la versions sans !

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

BaseType_t Clavier_5T_FreeRTOS::startTask(TaskHandle_t _taskGUI,unsigned short usStackDepth, UBaseType_t uxPriority, TaskHandle_t* _taskKeyboard){
  BaseType_t retour;
  if( taskKeyboard ==NULL){
    retour = xTaskCreate( Clavier_5T_FreeRTOS::fctKeyboard,
                            (const char *)"kb_5T",
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

void Clavier_5T_FreeRTOS::fctKeyboard(void* arg){
  Clavier_5T_FreeRTOS* clavier=(Clavier_5T_FreeRTOS*)arg;
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

uint8_t Clavier_5T_FreeRTOS::getToucheAct(bool valDerTouche) {
  // renvoi la valeur de la touche actuellement pressé
  if( taskKeyboard==NULL) Clavier_5T::getToucheAct(valDerTouche); // si la task FreeRTOS n'est pas encore active -> fonctionnement normal

  uint8_t touche;

  if ( (PIN_CLAVIER & ( MASQUE_CLAVIER )) == MASQUE_CLAVIER ) return 0; // si aucune touche

  onOffEcran(true);

  touche = 0;

  if( (PIN_CLAVIER & ( 1 << BIT_CLAVIER_BAS)) == 0 ) touche = touche | BT_BAS;
  if( (PIN_CLAVIER & ( 1 << BIT_CLAVIER_HAUT)) == 0 ) touche = touche | BT_HAUT;
  if( (PIN_CLAVIER & ( 1 << BIT_CLAVIER_GAUCHE)) == 0 ) touche = touche | BT_ESC;
  if( (PIN_CLAVIER & ( 1 << BIT_CLAVIER_DROITE)) == 0 ) touche = touche | BT_SELECT;
  if( (PIN_CLAVIER & ( 1 << BIT_CLAVIER_MENU)) == 0 ) touche = touche | BT_MENU;

  return touche;
}
