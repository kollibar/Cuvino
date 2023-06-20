#include "Event16.h"

uint8_t Event16::mode = EVENT_MODE_128MS;

Event16::Event16(void)
{
	this->count = 0;
}

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
int8_t Event16::setMode(uint8_t mode){
	if( mode < EVENT_MODE_128MS || mode > 15 ) return ERRREUR_MODE_NON_SUPPORTE;

	Event16::mode = mode;
	return ERREUR_OK;
}

/**
 * Récupère le mode actuel
 * 
 * @return uint8_t mode
*/
uint8_t Event16::getMode(){
	return Event16::mode;
}

/**
 * Récupère la periode d'appel du timer pour ne pas rater d'Event
 * 
 * @return	uint16_t	periode en milliseconde
*/
uint16_t Event16::getPeriodeTimer_ms(){
	return ((uint16_t)1<<Event16::mode)-1;
}

/** Renvoi la date actuelle en tick
 * 
 * @return	uint16_t 	date actuelle en tick
*/
uint16_t Event16::now16(){
	#ifdef TIMER16_USE_FREERTOS
	TickType_t freertosTick = xTaskGetTickCount();
	return freertosTick * portTICK_PERIOD_MS;
	#else
	return Event16::date32toTick16(millis());
	#endif
}

/** convertie une date en milliseconde vers des tick
 * 
 * @param	uint32_t date en milliseconde
 * 
 * @return	uint16_t date en tick
*/
uint16_t Event16::date32toTick16(uint32_t date){
	return (uint16_t)(date >> Event16::mode);
}

/** convertie une date en tick vers des milliseconde
 * 
 * @param	uint16_t date en tick
 * 
 * @return	uint32_t date en milliseconde
*/
uint32_t Event16::tick16toDate32(uint16_t tick){
	return ((uint32_t)tick)<<Event16::mode;
}

/**
 * Concatene 2 byte pour créer un int afin de le stocker
*/
uint16_t Event16::concatenation(uint8_t val1, uint8_t val2){
	return ((uint16_t)val1) & (((uint16_t)val2) << 8);
}

/**
 * Déconcatène un int pour récupérer le byte 1
*/
uint8_t Event16::deconcatenationVal1(uint16_t val){
	return ((uint8_t)(val & 0xFF));
}
/**
 * Déconcatène un int pour récupérer le byte 2
*/
uint8_t Event16::deconcatenationVal2(uint16_t val){
	return ((uint8_t)(val >> 8));
}

/**
 * Paramètre un évènement
 * 
 * @param	int8_t	type d'évènement
 * 					FCT_VIDE		fonction de callback sans argument
 *					FCT_UINT8		fonction de callback à un argument uint8_t
 *					FCT_INT8		fonction de callback à un argument int8_t
 *					FCT_UINT16		fonction de callback à un argument uint16_t
 *					FCT_INT16		fonction de callback à un argument int16_t
 *					FCT_POINTEUR	fonction de callback à un argument pointeur (void*)
 * @param	uint32_t period		periode de retour de l'évènement en ms
 * @param	void(*)(void) 	callback	fct à appeller à chaque occurence de l'évènement (éventuellement transtypé)
 * @param	void*	arg		paramètre à passer à la fonctiond de callback (éventuellement transtypé)
 * @param	int16_t	repeatCount		nb de répétition de l'évènement
 * 							si = -1 => répétition infinie de l'évènement
 * 							si == 0 => erreur
 * @param	bool	reserved	l'évènement est réservé (defaut : false)
 * 
 * @return int8_t	code erreur : 0 si OK,
 * 								  ERREUR_RESERVED_EVENT si tentative de paramétrage d'un évènement réservé sans que le paramètre 'reserve' soit = true,
*/
int8_t Event16::set(uint8_t type, uint32_t period_ms, void (*callback)(void), void* arg, uint16_t repeatCount, bool reserved){
	if( this->isReserved() && ! reserved ) return ERREUR_RESERVED_EVENT;
	if( repeatCount > ~MASQ_PARAM_16 ) return ERREUR_MAX_COUNT;
	if( type > MASQ_FCT_4 ) return ERREUR_TYPE_NON_SUPPORTE;

	this->setCount(repeatCount);
	this->setEventType(type);

	this->period_tick = Event16::date32toTick16(period_ms);
	this->callback = callback;
	this->arg = arg;
	this->lastEventTime_tick = Event16::now16();

	return ERREUR_OK;
}

/**
 * Change la fonction de callback et les arguments d'un evenement
 * 
 * @param	int8_t	type d'évènement
 * 					FCT_VIDE		fonction de callback sans argument
 *					FCT_UINT8		fonction de callback à un argument uint8_t
 *					FCT_INT8		fonction de callback à un argument int8_t
 *					FCT_UINT16		fonction de callback à un argument uint16_t
 *					FCT_INT16		fonction de callback à un argument int16_t
 *					FCT_POINTEUR	fonction de callback à un argument pointeur (void*)
 * @param	void(*)(void) 	callback	fct à appeller à chaque occurence de l'évènement (éventuellement transtypé)
 * @param	void*	arg		paramètre à passer à la fonctiond de callback (éventuellement transtypé)
 * 
 * @return int8_t	code erreur : 0 si OK,
 * 								  ERREUR_RESERVED_EVENT si tentative de paramétrage d'un évènement réservé sans que le paramètre 'reserve' soit = true,
*/
int8_t Event16::changeFct(uint8_t type, void (*callback)(void), void* arg){
	if( type > MASQ_FCT_4 ) return ERREUR_TYPE_NON_SUPPORTE;

	this->callback = callback;
	this->setEventType(type);
	this->arg=arg;

	return ERREUR_OK;
}


/**
 * Change le nombre de d'appel restant
 * 
 * @param	uint16_t	 repeatCount	 nouveau nb d'appel restant
 * 
 * @return code d'erreur de la fonction
 * 					si 0: OK !
*/
int8_t Event16::setCount(uint16_t repeatCount){
	if( repeatCount > ~MASQ_PARAM_16 ) return ERREUR_MAX_COUNT;

	this->count = ( this->count & MASQ_PARAM_16 ) | repeatCount;

	return ERREUR_OK;
}

/**
 * Retourne le nombre d'appel restant actuellement
 * 
 * @return uint16_t 	nb d'appel restant
*/
uint16_t Event16::getCount(){
	return (this->count & ~MASQ_PARAM_16);
}

/**
 * Renvoi la periode de l'Event
 * 
 * @return uint32_t	periode en millisecondes
*/
uint32_t Event16::getPeriod_ms(){
	return Event16::tick16toDate32(this->period_tick);
}

/**
 * Renvoi la date de dernier appel (en milliseconde)
 * 
 * @return uint32_t date de dernier appel en milliseconde
*/
uint32_t Event16::getLastEventTime_ms(){
	return Event16::tick16toDate32(this->lastEventTime_tick);
}
/**
 * check si un évènement est libre
 * 
 * @return bool true si libre, false sinon
*/
bool Event16::isFreeEvent(){
	return ( this->count & MASQ_FCT_16 == 0 );
}
/**
 * Marque un évènement commre réservé
*/
void Event16::setReserved(){
	this->count |= EVENT_RESERVED_16;
}
/**
 * Enlève le marquage 'réservé' d'un évènement
*/
void Event16::unsetReserved(){
	this->count &= ~EVENT_RESERVED_16;
}
/**
 * Check si un évènement est marqué comme réservé
 * 
 * @return bool true si réservé, false sinon
*/
bool Event16::isReserved(){
	return ( this->getEventType() & EVENT_RESERVED);
}

/**
 * Renvoi la valeur stocké pour la fonction de callback
 *
 * /!\ ne fonctionne QUE si la fonction donnée est un callback(uint8_t)
 * sinon return 0
 * @return uint8_t valeur
*/
uint8_t Event16::getValue8(){
	if( this->getFctType() == FCT_UINT8  ) return ((uint8_t)arg);
	else return 0;
}
/**
 * Renvoi la valeur stocké pour la fonction de callback
 *
 * si force  = false (defaut)
 * 		/!\ ne fonctionne QUE si la fonction donnée est un callback(uint16_t)
 * 		sinon return 0
 * 
 * si force  = true
 * 		/!\ renvoi toujours la valeur stocké, même si celle-ci est invalide, à vous de gérer
 * 
 * @return uint16_t valeur
*/
uint16_t Event16::getValue16(bool force){
	if( force || this->getFctType()  == FCT_UINT16  ) return ((uint16_t)arg);
	else return 0;
}

/**
 * Renvoi le pointeur stocké pour la fonction de callback
 *
 * /!\ ne fonctionne QUE si la fonction donnée est un callback(void*)
 * sinon return NULL
 * @return void* pointeur
*/
void* Event16::getPointeurArg(){
	if( this->getFctType()  == FCT_POINTEUR  ) return ((void*)arg);
	else return NULL;
}
/**
 * Arrete l'event
 * (ne supprime pas son éventuelle réservation)
*/
void Event16::stop(){
	this->count &= EVENT_RESERVED_16;
}

uint8_t Event16::getEventType(){
	return (uint8_t)(this->count>>12);
}
uint8_t Event16::getFctType(){
	return (this->getEventType() & MASQ_FCT_4);
}

bool Event16::setEventType(uint8_t type){
	if( type > MASQ_FCT_4 ) return false;
	this->count &= (~MASQ_FCT_16);
	this->count |= ((uint16_t)type << 12);
	return true;
}

/**
 * renvoi l'état d'activité de l'event
 * 
 * @return bool 	etat de l'event true=> actif, false non actif
*/
bool Event16::isActif(){
	return ( ( this->count & (MASQ_FCT_16) ) != 0);
}


bool Event16::update(void){
	if( this->isActif() ) return update(now16());
	return false;
}
bool Event16::update(uint32_t now){
	return update((uint16_t)(now >> Event16::mode));
}
bool Event16::update(uint16_t now){
	if( ! this->isActif() ) return false;

	if (now - lastEventTime_tick >= period_tick) {

		switch( this->getFctType() ){
			case FCT_VIDE:
				(*callback)();
				break;
			case FCT_UINT8:
				(*(void (*)(uint8_t))callback)((uint8_t)arg);
				break;
			case FCT_POINTEUR:
				(*(void (*)(void*))callback)(arg);
				break;
			case FCT_2_BYTE:
				(*(void (*)(uint8_t, uint8_t))callback)(Event16::deconcatenationVal1((uint16_t) arg) , Event16::deconcatenationVal2((uint16_t)arg));
				break;
			case FCT_UINT16:
				(*(void (*)(uint16_t))callback)((uint16_t)arg);
				break;
		}
		
		lastEventTime_tick = now;

		if( (count & ~MASQ_PARAM_16) > 0 ){
			--count;
			count = ((count & ~MASQ_PARAM_16) - 1) | (count & MASQ_PARAM_16);
			if(count & ~MASQ_PARAM_16 == 0)  {
				this->stop();
			}
		}

		return true;
	}
	return false;
}



/** 
 * Retourn la date du prochain appel de l'évènement
 * si le retour est de 0, besoin d'un update rapidement !
 * 
 * @return uint16_t date du prochain appel (en 'tick')
 *					 Si pas de prochain appel (évènt terminé ou non paramétré) renvoi 0xFFFF
*/
uint16_t Event16::next(void){
	return this->next(Event16::now16());
}
/** 
 * Retourn la date du prochain appel de l'évènement
 * si le retour est de 0, besoin d'un update rapidement !
 * (avec fourniture de la date actuelle en tick)
 * 
 * @param	uint16_t 	date actuelle en tick
 * 
 * @return uint16_t date du prochain appel (en 'tick')
 *					 Si pas de prochain appel (évènt terminé ou non paramétré) renvoi 0xFFFF
*/
uint16_t Event16::next(uint16_t now){
	if( ! this->isActif() ) return 0xFFFF;
	if( now - lastEventTime_tick >= period_tick ) return 0;
	else return (lastEventTime_tick + period_tick - now);
}
/** 
 * Retourn la date du prochain appel de l'évènement
 * si le retour est de 0, besoin d'un update rapidement !
 * (avec fourniture de la date actuelle en milliseconde depuis initialiation
 * 
 * @param	uint32_t 	date actuelle en milliseconde (issus de millis())
 * 
 * @return uint16_t date du prochain appel (en 'tick')
 *					 Si pas de prochain appel (évènt terminé ou non paramétré) renvoi 0xFFFF
*/
uint16_t Event16::next(uint32_t now){
	return this->next((uint16_t)(now >> Event16::mode));
}

/** 
 * Retourn la date du prochain appel de l'évènement
 * si le retour est de 0, besoin d'un update rapidement !
 * 
 * @return uint32_t date du prochain appel (en millisecondes)
 *					 Si pas de prochain appel (évènt terminé ou non paramétré) renvoi 0xFFFFFFFF
*/
uint32_t Event16::next_ms(void){
	return this->next_ms(Event16::now16());
}

/** 
 * Retourn la date du prochain appel de l'évènement
 * si le retour est de 0, besoin d'un update rapidement !
 * (avec fourniture de la date actuelle en milliseconde depuis initialiation
 * 
 * @param	uint32_t 	date actuelle en milliseconde (issus de millis())
 * 
 * @return uint32_t date du prochain appel (en millisecondes)
 *					 Si pas de prochain appel (évènt terminé ou non paramétré) renvoi 0xFFFFFFFF
*/

uint32_t Event16::next_ms(uint32_t now){
	return this->next_ms(Event16::date32toTick16(now));
}
/** 
 * Retourn la date du prochain appel de l'évènement
 * si le retour est de 0, besoin d'un update rapidement !
 * (avec fourniture de la date actuelle en tick)
 * 
 * @param	uint16_t 	date actuelle en tick
 * 
 * @return uint32_t date du prochain appel (en millisecondes)
 *					 Si pas de prochain appel (évènt terminé ou non paramétré) renvoi 0xFFFFFFFF
*/
uint32_t Event16::next_ms(uint16_t now){
	if( ! this->isActif() ) return 0xFFFFFFFF;
	return Event16::tick16toDate32(this->next(now));
}