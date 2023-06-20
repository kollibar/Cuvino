#ifndef __CANOPENOBJ_H__
#define __CANOPENOBJ_H__

#include <inttypes.h>
#include "COO_macros.h"
#include "COO_types.h"

typedef uint8_t subindex_t;
typedef uint16_t index_t;

typedef uint8_t COO_type_t;
typedef uint8_t COO_param_t;
typedef uint8_t COO_size_t;

typedef struct{
    COO_type_t format;
    COO_param_t param;
    union{
        COO_size_t taille;
        uint8_t valeur;
    };
} co_descr_obj;

typedef uint8_t co_erreur_t;

#define CANOPEN_OBJ_ERREUR_OK               0
#define CANOPEN_OBJ_ERREUR_INDEX_INCONNU    1
#define CANOPEN_OBJ_ERREUR_SUBINDEX_INCONNU 2
#define CANOPEN_OBJ_ERREUR_VARIABLE_NON_SAUVE_EEPROM    3
#define CANOPEN_OBJ_ERREUR_VARIABLE_LECTURE_SEULE       4
#define CANOPEN_OBJ_ERREUR_VARIABLE_ECRITURE_SEULE      5


#endif

