#ifndef __TERMINALWINDOW_H__
#define __TERMINALWINDOW_H__

#include <IO_PCD8544_5T.h>
#include <Adafruit_GFX_v1.0.2_NB8bits.h>

template<typename T> class TerminalWindow;

template<typename T>
class TerminalWindow: public PrintPGM{
public:
  TerminalWindow(T& _display);
  void start(int8_t _x,int8_t _y,int8_t _w,int8_t _h,uint8_t _color, uint8_t _bg, uint8_t _textSize);
  void stop();
  size_t write(uint8_t);
  void setColor(uint8_t _color);
  void setColor(uint8_t _color,uint8_t _bg);
  void clear();
protected:
  T* display;
  int8_t x_deb,y_deb,x_end,y_end,pos_x,pos_y;
  uint8_t color,bg, textSize;
};

template<typename T>
TerminalWindow<T>::TerminalWindow(T& _display){
  display=&_display;
}

template <class T>
void TerminalWindow<T>::start(int8_t _x,int8_t _y,int8_t _w,int8_t _h,uint8_t _color, uint8_t _bg, uint8_t _textSize){
  if( _y<0 || _x<0 || _x>=display->width() || _y>=display->height() ) x_deb=-1;
  else{
    x_deb=_x; pos_x=_x;
    y_deb=_y; pos_y=_y;
    x_end=x_deb+_w;
    y_end=y_deb+_h;
    if( x_end > display->width() ) x_end=display->width();
    if( y_end > display->height() ) y_end=display->height();
    color=_color;
    bg=_bg;
    textSize=_textSize;
  }
}

template <class T>
void TerminalWindow<T>::stop(){
  clear();
  x_deb=-1;
}
template <class T>
void TerminalWindow<T>::clear(){
  display->fillRect(x_deb,y_deb,x_end-x_deb,y_end-x_deb,bg);
  pos_x=x_deb;
  pos_y=y_deb;
}
template <class T>
void TerminalWindow<T>::setColor(uint8_t _color){
  color=_color;
}
template <class T>
void TerminalWindow<T>::setColor(uint8_t _color,uint8_t _bg){
  color=_color;
  bg=_bg;
}
template <class T>
size_t TerminalWindow<T>::write(uint8_t c){
  if( c==0) {
    display->display();
    return 0;
  }else{
    if( x_deb>= 0){
      if (pos_x > (x_end - ((textSize==0)?4:(textSize*6)))) {
        pos_y += ((textSize==0)?6:(textSize*8));
        pos_x = 0;
        int8_t y_c=((textSize==0)?6:(textSize*8));
        if( pos_y > (y_end - y_c)){

          for(int8_t y=y_deb+y_c;y<y_end;++y){
            for(int8_t x=x_deb;x<x_end;++x) display->drawPixel(x,y-y_c,display->getPixel(x,y));
          }
          pos_y-=y_c;
          display->fillRect(x_deb,pos_y,x_end-x_deb,y_end-pos_y,bg);
        }
      }
      display->drawChar(pos_x,pos_y,c,color,bg,textSize);
      pos_x += ((textSize==0)?4:(textSize*6));
    }
    return 1;
  }
}

#endif
