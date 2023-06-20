#include "fct_diverses.h"


char* strcpy_P(char* buffer, const char* chaine, bool addMaj, char i) {
  char c = pgm_read_byte(chaine + i);
  if ( addMaj && c >= 97 and c <= 122) c -= 32;
  char j = 0;
  buffer[j] = c;
  ++j;
  ++i;
  do {
    c = pgm_read_byte(chaine + i);
    buffer[j] = c;
    ++i;
    ++j;
  } while (c != 0);
  return buffer;
}
