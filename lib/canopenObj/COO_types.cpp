#include "COO_types.h"

COO_type_t COO_typeOf(bool& x){return COO_BOOLEAN;};
COO_type_t COO_typeOf(int8_t& x){return COO_INTEGER8;};
COO_type_t COO_typeOf(int16_t& x){return COO_INTEGER16;};
COO_type_t COO_typeOf(int32_t& x){return COO_INTEGER32;};
COO_type_t COO_typeOf(int64_t& x){return COO_INTEGER64;};
COO_type_t COO_typeOf(uint8_t& x){return COO_UNSIGNED8;};
COO_type_t COO_typeOf(uint16_t& x){return COO_UNSIGNED16;};
COO_type_t COO_typeOf(uint32_t& x){return COO_UNSIGNED32;};
COO_type_t COO_typeOf(uint64_t& x){return COO_UNSIGNED64;};
COO_type_t COO_typeOf(float& x){return COO_REAL32;};
COO_type_t COO_typeOf(char* x){return COO_OCTET_STRING;};
//COO_type_t COO_typeOf(byte* x){return COO_DOMAIN;};
COO_type_t COO_typeOf(char& x){return COO_OCTET_STRING;};
//COO_type_t COO_typeOf(byte x){return COO_DOMAIN;};

template<typename T>
uint8_t COO_isNumeric(T x){
  COO_type_t t=COO_typeOf(x);
  return ( ( t >= 0x2 && t <= 0x8 ) || t == 0x15 || t == 0x1B);
};
template<typename T>
uint8_t COO_isNumericNotFloat(T x){
  COO_type_t t=COO_typeOf(x);
  return ( ( t >= 0x2 && t <= 0x7 ) || t == 0x15 || t == 0x1B);
};
template<typename T>
uint8_t COO_isFloat(T x){
  return ( COO_typeOf(x) == COO_REAL32 );
};
template<typename T>
uint8_t COO_isNumericSignedNotFloat(T x){
  COO_type_t t=COO_typeOf(x);
  return ( ( t >= 0x2 && t <= 0x4 ) || t == 0x15 );
};
template<typename T>
uint8_t COO_isNumericUnsignedNotFloat(T x){
  COO_type_t t=COO_typeOf(x);
  return ( ( t >= 0x5 && t <= 0x7 ) || t == 0x1B );
};