#include "TempDS3231.h"


// ça c'est un peu dégueu ... normalement la librairie Wire s'initialise avec un Wire.begin
// mais si celui-ci a été fait ailleurs (nottamment dans Rtc_makusa) ... ça va faire merder.
// Je ne sais pas comment faire pour le vérifier... modifier la librairie Wire??

signed int getTemperatureDS3231(){
  Wire.beginTransmission(ADDRESSE_DS3231);
  Wire.write(DS3231_CMD_TEMP);
  Wire.endTransmission();

  Wire.requestFrom(ADDRESSE_DS3231, 2);
  uint8_t ent = Wire.read();
  uint8_t frac = Wire.read();

  return ((ent << 4) | (frac >> 4));
}


bool addrIsDS3231(unsigned char addr[8]) {
  if ( addr[0] == 'D' && addr[1] == 'S' && addr[2] == '3' && addr[3] == '2' && addr[4] == '3' && addr[5] == '1' && addr[6] == 0 ) return true;
  else return false;
}
