#include "Widgets.h"

void _creeLigneMenuListeV1(uint8_t i, char* chaine, uint8_t taille, void* arg) {
  char* chaineCom = (char*)arg;
  strcpy(chaine, &chaineCom[i * taille]);
  return;
}

void _creeLigneMenuListeOuiNon(uint8_t i, char* chaine, uint8_t taille, void* arg) {
  switch (i) {
    case 0:
      //strcpy_P(chaine, TXT_OUI);
      strcpy(chaine,"OUI");
      break;
    case 1:
      //strcpy_P(chaine, TXT_NON);
      strcpy(chaine,"NON");
      break;
    default:
      break;
  }
}
