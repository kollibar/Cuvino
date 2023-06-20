#include "Timer16.h"



/** Change le mode du timer, pour tous les Event !
 * càd la durée d'un tick.
 * Attention, les ticks des event actuels ne sont pas modifié,
 * Ne pas appeller qd des Event sont actifs !
 * 
 * @param uint8_t	mode
 * 
 * @return code d'erreur de la fonction
 * 					si 0: OK !
*/
int8_t Timer16::setMode(uint8_t mode){
	return Event16::setMode(mode);
}

/**
 * Récupère le mode actuel
 * 
 * @return uint8_t mode
*/
uint8_t Timer16::getMode(){
	return Event16::getMode();
}

/**
 * Récupère la periode d'appel du timer pour ne pas rater d'Event
 * 
 * @return	uint16_t	periode en milliseconde
*/
uint16_t Timer16::getPeriodeTimer_ms(){
	return Event16::getPeriodeTimer_ms();
}

Timer16::Timer16(void){}
/** Paramètre l'event 'id' dans la liste _events
 * fct interne 
 * aucune vérification
 * @param int8_t	id		id de lévènement dans la liste
 * @param int8_t	type d'évènement
 * 					FCT_VIDE		fonction de callback sans argument
 *					FCT_UINT8		fonction de callback à un argument uint8_t
 *					FCT_INT8		fonction de callback à un argument int8_t
 *					FCT_UINT16		fonction de callback à un argument uint16_t
 *					FCT_INT16		fonction de callback à un argument int16_t
 *					FCT_POINTEUR	fonction de callback à un argument pointeur (void*)
 * @param uint32_t period		periode de retour de l'évènement en ms
 * @param	void(*)(void) 	callback	fct à appeller à chaque occurence de l'évènement (éventuellement transtypé)
 * @param	void*	arg		paramètre à passer à la fonctiond de callback (éventuellement transtypé)
 * @param int16_t	repeatCount		nb de répétition de l'évènement
 * 							si = -1 => répétition infinie de l'évènement
 * 							si == 0 => erreur
 * @return id de l'évènement. Si retour < 0, erreur lors de la création de l'évènement.
*/
int8_t Timer16::setEvent(int8_t id, int8_t type, uint32_t period, void (*callback)(void), void* arg, int16_t repeatCount, bool reserved){
	if( id >= MAX_NUMBER_OF_EVENTS ) return ERREUR_ID_OUT_OF_RANGE;
	if( id < 0 ) return id;

	_events[id].set(type, period, callback, arg, repeatCount, reserved);
	return id;
}
  
  /**
   * Paramètre un évènement avec une fonction callback sans paramètres, pour 'repeatCount' répétition de l'évènement
   * @param uint32_t period		periode de retour de l'évènement en ms
   * @param	void(*)(void) 	callback	fct à appeller à chaque occurence de l'évènement
   * @param int16_t	repeatCount		nb de répétition de l'évènement
   * 							si = -1 => répétition infinie de l'évènement
   * @return id de l'évènement. Si retour < 0, erreur lors de la cration de l'évènement.
  */
int8_t Timer16::every(uint32_t period, void (*callback)(void), int repeatCount){
	int8_t id = findFreeEventIndex();
	if (id < 0 ) return id;

	return setEvent(id, FCT_VIDE, period, callback, NULL, repeatCount);
}
/**
   * Paramètre un évènement avec une fonction callback sans paramètres, pour répétition à l'infini
   * 
   * @param uint32_t period		periode de retour de l'évènement en ms
   * @param	void(*)(void) 	callback	fct à appeller à chaque occurence de l'évènement
   * 
   * @return id de l'évènement. Si retour < 0, erreur lors de la cration de l'évènement.
  */
int8_t Timer16::every(uint32_t period, void (*callback)(void)){
	return every(period, callback, -1); // - means forever
}
/**
   * Paramètre un évènement avec une fonction callback sans paramètres, pour 1 seule occurence de l'évènement
   * 
   * @param uint32_t period		periode de retour de l'évènement en ms
   * @param	void(*)(void) 	callback	fct à appeller à chaque occurence de l'évènement
   * 
   * @return id de l'évènement. Si retour < 0, erreur lors de la cration de l'évènement.
  */
int8_t Timer16::after(uint32_t period, void (*callback)(void))
{
	return every(period, callback, 1);
}

int8_t Timer16::every(uint32_t period, void (*callback)(void*),void* arg, int repeatCount)
{
	int8_t id = findFreeEventIndex();
	if (id < 0) return id;

	return setEvent(id, FCT_POINTEUR, period, (void (*)(void))callback,arg, repeatCount );
}

int8_t Timer16::every(uint32_t period, void (*callback)(void*),void* arg)
{
	return every(period, callback, arg,0); // - means forever
}

int8_t Timer16::after(uint32_t period, void (*callback)(void*),void* arg)
{
	return every(period, callback, arg,1);
}

int8_t Timer16::every(uint32_t period, void (*callback)(uint8_t),uint8_t val, int repeatCount)
{
	int8_t id = findFreeEventIndex();
	if (id < 0) return id;

	return setEvent(id, FCT_UINT8, period, (void (*)(void))callback, (void*)val, repeatCount);
}

int8_t Timer16::every(uint32_t period, void (*callback)(uint8_t),uint8_t val)
{
	return every(period, callback, val,0); // - means forever
}

int8_t Timer16::after(uint32_t period, void (*callback)(uint8_t),uint8_t val)
{
	return every(period, callback, val,1);
}

int8_t Timer16::every(uint32_t period, void (*callback)(uint16_t),uint16_t val, int repeatCount)
{
	int8_t id = findFreeEventIndex();
	if (id < 0) return id;

	return setEvent(id, FCT_UINT16, period, (void (*)(void))callback, (void*)val, repeatCount);

}

int8_t Timer16::every(uint32_t period, void (*callback)(uint16_t),uint16_t val)
{
	return every(period, callback, val,0); // - means forever
}

int8_t Timer16::after(uint32_t period, void (*callback)(uint16_t),uint16_t val)
{
	return every(period, callback, val,1);
}

uint8_t Timer16::getValue8(int8_t id){
	return _events[id].getValue8();
}
uint16_t Timer16::getValue16(int8_t id, bool force){
	return _events[id].getValue16(force);
}

void* Timer16::getPointeurArg(int8_t id){
	return _events[id].getPointeurArg();
}

/**
 * Arrete la répétition d'un évènement
 * 
 * @param uint8_t id	id du timer
*/
void Timer16::stop(int8_t id){
	if (id >= 0 && id < MAX_NUMBER_OF_EVENTS) {
		_events[id].stop();
	}
}

/**
 * Actualise tous les timers pour en vérifier l'activation de l'évènement
 * 
*/
void Timer16::update(void){
	this->update(Event16::now16());
}

/**
 * Actualise tous les timers pour en vérifier l'activation de l'évènement
 * (avec fourniture de la date actuelle issue de millis() )
 * 
 * @param uint32_t now	date actuelle (appel de la fonction millis())
*/
void Timer16::update(uint32_t now){
	this->update((uint16_t)(now >> Event16::getMode()));
}

/**
 * Actualise tous les timers pour en vérifier l'activation de l'évènement
 * (avec fourniture de la date actuelle en 'tick' de Event )
 * 
 * @param uint16_t now	date actuelle en tick de Event (par exemple fournie par Event16::now16())
*/
void Timer16::update(uint16_t now){
	for (int8_t i = 0; i < MAX_NUMBER_OF_EVENTS; i++){
		_events[i].update(now);
	}
}
/**
 * Cherche quel est la date du prochain évènement tout timer confondu
 * 
 * @return uint32_t	date du prochain évènement (en ms)
*/
uint32_t Timer16::next(){
	return this->next(Event16::now16());
}
/**
 * Cherche quel est la date du prochain évènement tout timer confondu
 * (avec fourniture de la date actuelle issue de millis() )
 * 
 * @param uint32_t now	date actuelle (appel de la fonction millis())
 * 
 * @return uint32_t	date du prochain évènement (en ms)
*/
uint32_t Timer16::next(uint32_t now){
	return this->next(Event16::date32toTick16(now));
}
/**
 * Cherche quel est la date du prochain évènement tout timer confondu
 * (avec fourniture de la date actuelle en 'tick' de Event )
 * 
 * @param uint16_t now	date actuelle en tick de Event (par exemple fournie par Event16::now16())
 * 
 * @return uint32_t	date du prochain évènement (en ms)
*/
uint32_t Timer16::next(uint16_t now){
	uint16_t r=0xFFFF;
	for (int8_t i = 0; i < MAX_NUMBER_OF_EVENTS; i++) {
		if ( _events[i].isActif() ) {
			if( _events[i].next(now) < r ) r=_events[i].next(now);
			if( r==0 ) break;
		}
	}
	return Event16::tick16toDate32(r);
}
/**
 * Trouve un Event vide et renvoi son id (<0 si erreur)
 * 
 * @return int8_t	 id du 1er event livre, <0 si erreur
*/
int8_t Timer16::findFreeEventIndex(void){
	for (int8_t i = 0; i < MAX_NUMBER_OF_EVENTS; i++) {
		if ( _events[i].isFreeEvent() ) {
			return i;
		}
	}
	return NO_TIMER_AVAILABLE;
}
/**
 * Réserve un évènement pour usage futur
 * 
 * @return int8_t id de l'évènement
*/
int8_t Timer16::reserve(){
	int8_t id = findFreeEventIndex();

	//si id < 0 => erreur, on ne fait rien
	if (id >= 0 ) _events[id].setReserved();

	return id;
}
/**
 * Libère un évènement réservé pour usage futur
 * 
 * @return int8_t id de l'évènement
*/
void Timer16::relache(int8_t id){
	_events[id].unsetReserved();
}

int8_t Timer16::setEventReserve(int8_t id, uint32_t period, void(*callback)(), int repeatCount){
	if( ! _events[id].isReserved() ) return ERREUR_RESERVED_EVENT;

	return setEvent(id, FCT_VIDE, period, callback, NULL, repeatCount, true );
}
int8_t Timer16::setEventReserve(int8_t id, uint32_t period, void(*callback)(void*), void* arg, int repeatCount){
	if( ! _events[id].isReserved() ) return ERREUR_RESERVED_EVENT;

	return setEvent(id, FCT_POINTEUR, period, (void (*)(void))callback, arg, repeatCount , true);
}
int8_t Timer16::setEventReserve(int8_t id, uint32_t period, void(*callback)(uint8_t), uint8_t val, int repeatCount){
	if( ! _events[id].isReserved() ) return ERREUR_RESERVED_EVENT;

	return setEvent(id, FCT_UINT8, period, (void (*)(void))callback, (void*)val, repeatCount , true);
}
int8_t Timer16::setEventReserve(int8_t id, uint32_t period, void(*callback)(uint16_t), uint16_t val, int repeatCount){
	if( ! _events[id].isReserved() ) return ERREUR_RESERVED_EVENT;

	return setEvent(id, FCT_UINT16, period, (void (*)(void))callback, (void*)val, repeatCount , true);
}
int8_t Timer16::setEventReserve(int8_t id, uint32_t period, void(*callback)(uint8_t, uint8_t), uint8_t val1, uint8_t val2, int repeatCount){
	if( ! _events[id].isReserved() ) return ERREUR_RESERVED_EVENT;

	return setEvent(id, FCT_UINT8, period, (void (*)(void))callback, (void*)Event16::concatenation(val1,val2), repeatCount , true);
}

int8_t Timer16::changeFct(int8_t id, void(*callback)()){
	if( id < 0 || id >= MAX_NUMBER_OF_EVENTS) return ERREUR_ID_OUT_OF_RANGE;

	return _events[id].changeFct(FCT_VIDE, callback, NULL);
}
int8_t Timer16::changeFct(int8_t id, void(*callback)(void*), void* arg){
	if( id < 0 || id >= MAX_NUMBER_OF_EVENTS) return ERREUR_ID_OUT_OF_RANGE;

	return _events[id].changeFct(FCT_POINTEUR, (void(*)(void))callback, arg);
}
int8_t Timer16::changeFct(int8_t id, void(*callback)(uint8_t), uint8_t val){
	if( id < 0 || id >= MAX_NUMBER_OF_EVENTS) return ERREUR_ID_OUT_OF_RANGE;

	return _events[id].changeFct(FCT_UINT8, (void(*)(void))callback, (void*)val);
}
int8_t Timer16::changeFct(int8_t id, void(*callback)(uint16_t), uint16_t val){
	if( id < 0 || id >= MAX_NUMBER_OF_EVENTS) return ERREUR_ID_OUT_OF_RANGE;

	return _events[id].changeFct(FCT_UINT16, (void(*)(void))callback, (void*)val);
}
int8_t Timer16::changeFct(int8_t id, void(*callback)(uint8_t, uint8_t), uint8_t val1, uint8_t val2){
	if( id < 0 || id >= MAX_NUMBER_OF_EVENTS) return ERREUR_ID_OUT_OF_RANGE;

	return _events[id].changeFct(FCT_2_BYTE, (void(*)(void))callback, (void*)Event16::concatenation(val1,val2));
}