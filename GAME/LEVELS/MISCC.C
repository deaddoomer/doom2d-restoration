#include "glob.h"
#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include "vga.h"
#include "view.h"
#include "memory.h"
#include "error.h"
#include "misc.h"

char **Z_makelist(void) {
  char **p;

  if(!(p=malloc(16004))) ERR_fatal("Not enough memory");
  *p=NULL;
  return p;
}

void Z_freelist(char **p) {
  char **s;

  for(s=p;*s;++s) free(*s);
  free(p);
}

void Z_drawmanspr(int x,int y,void *p,char d,byte color) {
  if(d) V_manspr2(x-w_x+100,y-w_y+50+w_o,p,color);
    else V_manspr(x-w_x+100,y-w_y+50+w_o,p,color);
}

void Z_drawspr(int x,int y,void *p,char d) {
  if(d) V_spr2(x-w_x+100,y-w_y+50+w_o,p);
    else V_spr(x-w_x+100,y-w_y+50+w_o,p);
}

void Z_dot(int x,int y,byte c) {
  V_dot(x-w_x+100,y-w_y+50+w_o,c);
}
