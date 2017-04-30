#ifndef __ERREURS_H__
#define __ERREURS_H__

#include <Arduino.h>

#define NB_ERREURS_MAX  5

typedef void(*FonctionErreur)(void);

struct Erreur{
  unsigned char err;
  unsigned int date;
  unsigned char param;
};


class PileErreur{
protected:
  unsigned char moduleAlloue;
  Erreur pile[NB_ERREURS_MAX];
  unsigned char nb;
  FonctionErreur fctErreur;
public:
  PileErreur();
  PileErreur(FonctionErreur fctErr);
  void videPile();
  void addErreur(unsigned char module,unsigned char erreur,unsigned char paramErreur);
  bool delErreur(unsigned char i);
  unsigned char getNumModuleSuiv();
  unsigned char getNb() const{
    return nb;
  }
  const Erreur* getErreur(unsigned char i) const{
    return (const Erreur*)&pile[i];
  }
  unsigned char getModuleErreur(unsigned char i) const;
  unsigned char getCodeErreur(unsigned char i) const;
  unsigned long getDateErreur(unsigned char i) const;
  unsigned char getParamErreur(unsigned char i) const;
};

#endif
