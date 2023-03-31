#include "glob.h"
#include <stdlib.h>
#include <string.h>
#include <vga.h>
//#include <keyb.h>
#include "error.h"

enum{AB_END,AB_SCREEN,AB_UPDATE};

typedef struct{
  short t,st;
  unsigned len;
}anm_blk_t;

#define SQ 4

static byte cnum[256];

static void init_cpack(void) {
  int i;

  for(i=0;i<256;++i) cnum[i]=i;
}

static byte cpack(byte n) {
  byte c;

  c=cnum[n];
  if(n) memmove(cnum+1,cnum,n);
  cnum[0]=c;
  return c;
}

static byte *unpack(byte *d,byte *s,int l) {
  for(;l>0;--l,++d,++s) {
    if(*s==0) {memset(d,cpack(0),(dword)(*(++s))+1);d+=(dword)*s;l-=(dword)*s;}
    else *d=cpack(*s);
  }
  return s;
}

static byte *line(int y,byte *u) {
  int x,n,sy;

  for(x=0;x<scrw/SQ;)
    if(*u&0x80) {
      n=(*u&0x7F)+1;++u;
      for(sy=0;sy<SQ*320;sy+=320) {u=unpack(scra+y+sy+x*SQ,u,n*SQ);}
      x+=n;
    }else{x+=*u+1;++u;}
  return u;
}

static anm_blk_t *anm;

void ANM_start(void *p) {
  init_cpack();
  anm=p;
}

int ANM_play(void) {
  byte *u;
  int x,y;

//  while(!keys[0x39]);
//  while(keys[0x39]);
    if(anm->t==AB_END) return 0;
    switch(anm->t) {
      case AB_SCREEN:
        unpack(scra,(byte*)(anm+1),64000);
        break;
      case AB_UPDATE:
        for(u=(byte*)(anm+1),y=0;y<200/SQ;)
          if(*u&0x80) {
            for(x=(*u&0x7F)+1,++u;x;--x,++y)
              u=line(y*320*SQ,u);
          }else{y+=*u+1;++u;}
        break;
      default: ERR_fatal("bad ANM file");
    }
    anm=((anm_blk_t*)((byte*)anm+anm->len))+1;
  if(anm->t==AB_END) return 0;
  return 1;
}
