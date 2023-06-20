#ifndef __COO_MACROS_H__
#define __COO_MACROS_H__

#include <inttypes.h>
#include "COO_types.h"


#define COO_SUBINDEX_NULL 0xFF
#define COO_SUBINDEX_INDEXONLY 0xFE

#define COO_BITS_TYPE_DATA    0b00001111
#define COO_DATA_PROGMEM      0b00001000
#define COO_DATA_VAR          0b00000001
#define COO_DATA_FCT          0b00000010
#define COO_DATA_EEPROM       0b00000100

#define COO_PARAM_SYSTEM      0b0000

#define COO_WRITABLE_BY_CAN            0b00100000
#define COO_RO                         0b10000000
#define COO_WO                         0b01000000
#define COO_RW                         0b00000000
#define COO_CONFIG                     0b11000000 // les variables de confg sont forcément readable et writable => RO & WO = Config
#define COO_CONFIG_VAL_DEFAUT_PROGMEM  (COO_CONFIG | COO_DATA_PROGMEM)
#define COO_STORAGE_EEPROM_VOLATILE             0b00010000

#define COO_BITS_READ_WRITE    0b11000000

#define COO_DESCR_SUBINDEX0(nb_index)   {COO_UNSIGNED8, COO_DATA_INCLUS, nb_index}
#define COO_DESCR_INT8(param)   {COO_INTEGER8, (uint8_t)param, sizeof(int8_t)}
#define COO_DESCR_INT16(param)  {COO_INTEGER16, (uint8_t)param, sizeof(int16_t)}
#define COO_DESCR_INT32(param)  {COO_INTEGER32, (uint8_t)param, sizeof(int32_t)}

#define COO_DESCR_UINT8(param)  {COO_UNSIGNED8, (uint8_t)param, sizeof(uint8_t)}
#define COO_DESCR_UINT16(param)  {COO_UNSIGNED16, (uint8_t)param, sizeof(uint16_t)}
#define COO_DESCR_UINT32(param)  {COO_UNSIGNED32, (uint8_t)param, sizeof(uint32_t)}

#endif