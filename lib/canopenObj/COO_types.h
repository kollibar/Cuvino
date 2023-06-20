#ifndef __COO_TYPE_H__
#define __COO_TYPE_H__

#include <stdint.h>
#include <avr/pgmspace.h>

typedef uint8_t COO_size_t;
typedef uint8_t COO_param_t;
typedef uint8_t COO_type_t;


#define COO_NULL        0x00
#define COO_BOOLEAN     0x01
#define COO_INTEGER8    0x02
#define COO_INTEGER16   0x03
#define COO_INTEGER32   0x04
#define COO_UNSIGNED8   0x05
#define COO_UNSIGNED16  0x06
#define COO_UNSIGNED32  0x07
#define COO_REAL32      0x08
#define COO_VISIBLE_STRING  0x09
#define COO_OCTET_STRING    0x0A
#define COO_UNICODE_STRING  0x0B
#define COO_DOMAIN      0x0F
//#define COO_REAL64      0x11
#define COO_INTEGER64   0x15
#define COO_UNSIGNED64  0x1B

/* detection de type */
// Generic catch-all implementation.
COO_type_t COO_typeOf(bool& x);
COO_type_t COO_typeOf(int8_t& x);
COO_type_t COO_typeOf(int16_t& x);
COO_type_t COO_typeOf(int32_t& x);
COO_type_t COO_typeOf(int64_t& x);
COO_type_t COO_typeOf(uint8_t& x);
COO_type_t COO_typeOf(uint16_t& x);
COO_type_t COO_typeOf(uint32_t& x);
COO_type_t COO_typeOf(uint64_t& x);
COO_type_t COO_typeOf(float& x);
COO_type_t COO_typeOf(char* x);
//COO_type_t COO_typeOf(byte* x);
COO_type_t COO_typeOf(char& x);
//COO_type_t COO_typeOf(byte x);

template<typename T>
uint8_t COO_isNumeric(T x);
template<typename T>
uint8_t COO_isNumericNotFloat(T x);
template<typename T>
uint8_t COO_isFloat(T x);
template<typename T>
uint8_t COO_isNumericSignedNotFloat(T x);
template<typename T>
uint8_t COO_isNumericUnsignedNotFloat(T x);


#endif