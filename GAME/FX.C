#include "glob.h"
#include <io.h>
//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vga.h"
#include "error.h"
#include "view.h"
#include "fx.h"
#include "misc.h"

enum{NONE,TFOG,IFOG,BUBL};

typedef struct{
  int x,y;
  char t,s;
}fx_t;

static void *spr[15],*bsnd[2];
static char sprd[15];
static fx_t fx[MAXFX];

void FX_savegame(int h) {
  int i,n;

  for(i=n=0;i<MAXFX;++i) if(fx[i].t) ++n;
  write(h,&n,4);
//  printf("FX: %d\n",n);
  for(i=0;i<MAXFX;++i) if(fx[i].t) write(h,&fx[i],sizeof(fx_t));
}

void FX_loadgame(int h) {
  int n;

  read(h,&n,4);
//  printf("FX: %d\n",n);
  read(h,fx,n*sizeof(fx_t));
}

void FX_alloc(void) {
  int i;

//  logo("  effects");
  for(i=0;i<10;++i) spr[i]=Z_getspr("TFOG",i,0,sprd+i);
  for(;i<15;++i) spr[i]=Z_getspr("IFOG",i-10,0,sprd+i);
  bsnd[0]=Z_getsnd("BUBL1");
  bsnd[1]=Z_getsnd("BUBL2");
}

void FX_init(void) {
  int i;

  for(i=0;i<MAXFX;++i) fx[i].t=0;
}

void FX_act(void) {
  int i;
  byte b;

  for(i=0;i<MAXFX;++i) switch(fx[i].t) {
    case TFOG:
      if(++fx[i].s>=20) fx[i].t=0;
      break;
    case IFOG:
      if(++fx[i].s>=10) fx[i].t=0;
      break;
    case BUBL:
      fx[i].y-=fx[i].s+3;
      if((b=fld[fx[i].y>>3][fx[i].x>>3]) < 5 || b>7) fx[i].t=0;
      break;
  }
}

void FX_draw(void) {
  int i,s;

  for(i=0;i<MAXFX;++i) {
    s=-1;
    switch(fx[i].t) {
      case TFOG: s=fx[i].s/2;break;
	  case IFOG: s=fx[i].s/2+10;break;
	  case BUBL:
		V_dot(fx[i].x-w_x+100,fx[i].y-w_y+50+w_o,0xC0+fx[i].s);
		continue;
    }
    if(s>=0) Z_drawspr(fx[i].x,fx[i].y,spr[s],sprd[s]);
  }
}

void FX_tfog(int x,int y) {
  int i;

  for(i=0;i<MAXFX;++i) if(!fx[i].t) {
	fx[i].t=TFOG;fx[i].s=0;
	fx[i].x=x;fx[i].y=y;
	return;
  }
}

void FX_ifog(int x,int y) {
  int i;

  for(i=0;i<MAXFX;++i) if(!fx[i].t) {
    fx[i].t=IFOG;fx[i].s=0;
    fx[i].x=x;fx[i].y=y;
    return;
  }
}

void FX_bubble(int x,int y,int n) {
  int i;

  Z_sound(bsnd[rand()&1],128);
  for(i=0;i<MAXFX;++i) {
    if(!fx[i].t) {
	fx[i].t=BUBL;fx[i].s=rand()&1;
	fx[i].x=x+random(5)-2;
	fx[i].y=y+random(5)-2;
        if(--n==0) return;
    }
  }
}
