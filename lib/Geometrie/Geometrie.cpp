#include "Geometrie.h"

Point8_t::Point8_t(){
  this->x=0;this->y=0;
}

Point8_t::Point8_t(signed char _x,signed char _y){
  this->x=_x;this->y=_y;
}

Point16_t::Point16_t(){
  this->x=0;this->y=0;
}

Point16_t::Point16_t(signed char _x,signed char _y){
  this->x=_x;this->y=_y;
}

Point16_t::Point16_t(signed int _x,signed int _y){
  this->x=_x;this->y=_y;
}

Point16_t::Point16_t(Point8_t& p){
  this->x=p.x;
  this->y=p.y;
}
