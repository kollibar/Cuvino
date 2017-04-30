#include "PrintPGM.h"

PrintPGM::PrintPGM():Print(){

}

size_t PrintPGM::printNdigits(uint8_t nb,uint8_t n){
  if(nb >= pow(10,n--)) return print(nb);
  size_t r=(size_t)n;
  while(nb<pow(10,n) && n!=0){
    --n;
    write('0');
  }
  if( nb != 0) print(nb);
  return r;
}

size_t PrintPGM::printNdigits(uint16_t nb,uint8_t n){
  if(nb >= pow(10,n--)) return print(nb);
  size_t r=(size_t)n;
  while(nb<pow(10,n) && n!=0){
    --n;
    write('0');
  }
  if( nb != 0) print(nb);
  return r;
}

size_t PrintPGM::printNdigits(uint32_t nb,uint8_t n){
  if(nb >= pow(10,n--)) return print(nb);
  size_t r=(size_t)n;
  while(nb<pow(10,n) && n!=0){
    --n;
    write('0');
  }
  if( nb != 0) print(nb);
  return r;
}

size_t PrintPGM::print2digits(uint8_t nb){
	if( nb<10) write('0');
	else write(nb/10+48);
	write(nb%10+48);
	return 2;
}

// Imprime un espace
size_t PrintPGM::printEspace(uint8_t nb){
	for(;nb!=0;--nb) write(' ');
  return (size_t)nb;
}
// Print une chaine PROGMEM
size_t PrintPGM::printPGM(const char* chaine,bool addMaj,uint8_t i){
  size_t n=(size_t)i;
	char c=pgm_read_byte(chaine+i);
	if( addMaj && c>=97 and c<=122) c-=32;
	while(c!=0){
		write(c);
		++i;
		c=pgm_read_byte(chaine+i);
	}
  return ((size_t)i-n);
}
size_t PrintPGM::printlnPGM(const char* chaine,bool addMaj,uint8_t i){
	size_t r=printPGM(chaine,addMaj,i)+1;
	write('\n');
  return r;
}


size_t PrintPGM::printDateTime(uint8_t D, uint8_t M, uint8_t Y, uint8_t h,uint8_t m, uint8_t s){
  size_t r=print2digits(D);
  r+=write('/');
  r+=print2digits(M);
  r+=write('/');
  r+=print2digits(Y);
  r+=write(' ');
  r+=print2digits(h);
  r+=write(':');
  r+=print2digits(m);
  if ( s != 255) {
    r+=write(':');
    r+=print2digits(s);
  }
  return r;
}

size_t PrintPGM::printTime(uint8_t h,uint8_t m, uint8_t s){
  size_t r=print2digits(h);
  r+=write(':');
  r+=print2digits(m);
  if ( s != 255) {
    r+=write(':');
    r+=print2digits(s);
  }
  return r;
}

size_t PrintPGM::printTemp16e(signed int temp,unsigned char n,unsigned char m){
  size_t r=0;
  signed int d=1000;
  // partie entière
  if( temp < 0) {
    r=print('-');
    temp=-temp;
  }
  r+=print(temp>>4);

  if( m>4 ) m=4; // m = nb max de digit après virgule
  if( n>m ) n=m; // n = nb min de digit après virgule

  if( m!=0 && (n!=0 || (temp&0b1111) !=0)){
    // partie décimale
    r+=print('.');
    temp=temp&0b1111;

    if(temp==0) r+=printNdigits((unsigned int)temp,n);
    else{
      temp=temp*625;
      while( m>0 ){
        print((char)(temp/d+48));
        --m;--n;
        temp-=(temp/d)*1000;
        if(n<=0 && temp==0) break;
      }
    }
  }
  r+=write((char)247);
  return r;
}
