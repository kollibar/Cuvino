#ifndef __GEOMETRIE_H__
#define __GEOMETRIE_H__


class Point8_t{
public:
  signed char x;
  signed char y;
  Point8_t();
  Point8_t(signed char,signed char);
};

class Point16_t{
public:
  signed int x;
  signed int y;
  Point16_t();
  Point16_t(signed char,signed char);
  Point16_t(signed int,signed int);
  Point16_t(Point8_t&);
};

struct Point32_t{
  signed long x;
  signed long y;
};

#endif
