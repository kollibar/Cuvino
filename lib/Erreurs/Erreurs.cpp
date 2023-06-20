#include <Erreurs.h>

PileErreur::PileErreur(){
  moduleAlloue=0;
  fctErreur=NULL;
  videPile();
}

PileErreur::PileErreur(FonctionErreur fctErr){
  moduleAlloue=0;
  fctErreur=fctErr;
  videPile();
}


unsigned char PileErreur::getNumModuleSuiv(){
  ++moduleAlloue;
  return moduleAlloue;
}

void PileErreur::videPile(){
  for(unsigned char i=0;i<NB_ERREURS_MAX;++i) {
    pile[i].param=0;
    pile[i].date=0;
    pile[i].err=0;
  }
  nb=0;
}

void PileErreur::addErreur(unsigned char module,unsigned char erreur,unsigned char paramErreur){
  if( fctErreur != NULL) fctErreur();

  if( nb >= NB_ERREURS_MAX-1 ) return; // complet -> aucun nouvel enregistrement possible
  if( nb >= NB_ERREURS_MAX-2 || (nb >= NB_ERREURS_MAX-3 && (module >= 8 || erreur >= 32))){ // complet mais reste 1 place (cas d'une erreur simple) ou reste 2 places (cas d'une erreur double) => on enregistre une erreur de dépassement de capacité de la pile d'erreur
    pile[nb].err=8;
    pile[nb].date=millis()/60000; // date en minute
    nb++;
    if(module >= 8 || erreur >= 32){
      pile[nb].err=8;
      pile[nb].date=millis()/60000; // date en minute
      nb++;
    }
  }
  else{
    if( module < 8 && erreur < 32){ // erreur simple (càd module + erreur peut être enregistré sur 1 octets): module < 8 et erreur < 32
      pile[nb].err=module|(erreur<<3);
      pile[nb].date=millis()/60000; // date en minute
      pile[nb].param=paramErreur;
      nb++;
    } else{ // numero d'erreur ou de module trop grand !! => erreur double (enregistré sur 2 blocs erreurs)
      pile[nb].err=module|0b10000000;
      pile[nb].date=millis()/60000; // date en minute
      pile[nb].param=paramErreur;
      nb++;
      pile[nb].err=erreur;
      pile[nb].date=pile[nb-1].date; // date en minute
      pile[nb].param=paramErreur;
      nb++;
    }
  }
}
unsigned char PileErreur::getModuleErreur(unsigned char i) const{
  if( (pile[i].err&0b10000000)==0){ // erreur simple
    if( i==0 || (pile[i-1].err&0b10000000)==0){
      return pile[i].err&0b111;
    } else return 0xFF;
  } else{ // erreur sur 2 blocs
    return pile[i].err&0b01111111;
  }
}
unsigned char PileErreur::getCodeErreur(unsigned char i) const{
  if( (pile[i].err&0b10000000)==0){ // erreur simple
    if( i==0 || (pile[i-1].err&0b10000000)==0 ){
      return pile[i].err>>3;
    } else return 0xFF;
  } else{ // erreur sur 2 blocs
    if( i<NB_ERREURS_MAX-2) return pile[i+1].err; // ce n'est pas la dernière erreur
    else return 0xFF; // erreur enregistrée comme sur 2 blocs mais dernière erreur => erreur!
  }
}
unsigned long PileErreur::getDateErreur(unsigned char i) const{
  return pile[i].date*60000;
}
unsigned char PileErreur::getParamErreur(unsigned char i) const{
  return pile[i].param;
}

bool PileErreur::delErreur(unsigned char i){
  if( i > nb) return false;
  while(i <nb-1 ){
    pile[i].err=pile[i+1].err;
    pile[i].date=pile[i+1].date;
    pile[i].param=pile[i+1].param;
    i++;
  }
  nb--;
  return true;
}
