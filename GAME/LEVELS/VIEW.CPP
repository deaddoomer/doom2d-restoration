#include "glob.h"
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include "vga.h"
#include "gui.h"
#include "edit.h"
#include "memory.h"
#include "files.h"
#include "error.h"
#include "view.h"
#include "misc.h"
#include "..\map.h"

extern "C" {
extern map_block_t blk;
extern char musname[8];
extern byte clrmap[][256];
void V_remap_rect(short,short,short,short,byte *);
}

#define WD 200
#define HT 98
#define MAXX (FLDW*CELW-WD/2)
#define MAXY (FLDH*CELH-HT/2)

int min(int a,int b) {
  return (a<b)?a:b;
}

extern "C" {
int w_o=0,w_x=FLDW/2,w_y=FLDH/2;
int skytype=1;
byte w_drawb=1,w_drawt=0,w_drawf=1,w_drawth=1,w_drawsw=1;
void *walp[256],*horiz=NULL;
int walf[256];
byte fldb[FLDH][FLDW];
byte fldf[FLDH][FLDW];
byte fld[FLDH][FLDW];
wall_t wal[255];
}

int edx1,edy1,edx2,edy2,edt=0;

extern "C" void W_load_old(int h) {
  int i;

  memset(musname,0,8);
  skytype=1;
  for(i=0;i<255;++i) {
    read(h,wal+i,sizeof(wall_t));
    if(!wal[i].n[0]) break;
  }
  read(h,fldb,FLDW*FLDH);
  read(h,fld,FLDW*FLDH);
  read(h,fldf,FLDW*FLDH);
  TH_load_old(h);
}

static void unpack(void *buf,int len,void *obuf) {
  byte *p,*o;
  int l,n;

  for(p=(byte*)buf,o=(byte*)obuf,l=len;l;++p,--l) if(*p==255) {
    n=*((word*)(++p));memset(o,*(p+=2),n);o+=n;l-=3;
  }else *(o++)=*p;
}

static void pack(byte *ptr,int len) {
  int s,e,n;

  for(s=0;s<len;) {
    for(e=s+1;e<len && ptr[e]==ptr[s];++e);
    n=e-s;
    if(n>4) {
      F_write_blk("\xFF",1);
      F_write_blk(&n,2);
      F_write_blk(ptr+s,1);
    }else F_write_blk(ptr+s,n);
    s=e;
  }
}

extern "C" int W_load(int h) {
  int i;
  void *p,*buf;

  switch(blk.t) {
	case MB_WALLNAMES:
	  for(i=0;i<255 && blk.sz>0;++i,blk.sz-=sizeof(wall_t))
		read(h,wal+i,sizeof(wall_t));
	  return 1;
	case MB_BACK:  p=fldb;goto unp;
	case MB_WTYPE: p=fld;goto unp;
	case MB_FRONT: p=fldf;
	unp: switch(blk.st) {
	    case 0: read(h,p,FLDW*FLDH);break;
	    case 1:
	      read(h,buf=malloc(blk.sz),blk.sz);
	      unpack(buf,blk.sz,p);free(buf);break;
	    default: return 0;
	  }return 1;
	case MB_MUSIC:
	  read(h,musname,8);
	  return 1;
	case MB_SKY:
	  skytype=0;read(h,&skytype,2);
	  return 1;
  }return 0;
}

extern "C" void W_save(void) {
  int i;

  F_start_blk(MB_MUSIC,0);
  F_write_blk(musname,8);
  F_end_blk();
  F_start_blk(MB_SKY,0);
  F_write_blk(&skytype,2);
  F_end_blk();
  F_start_blk(MB_WALLNAMES,0);
  for(i=0;i<255 && wal[i].n[0];++i)
	F_write_blk(wal+i,sizeof(wall_t));
  F_end_blk();
  F_start_blk(MB_BACK,1);
  pack((byte*)fldb,FLDW*FLDH);
  F_end_blk();
  F_start_blk(MB_WTYPE,1);
  pack((byte*)fld,FLDW*FLDH);
  F_end_blk();
  F_start_blk(MB_FRONT,1);
  pack((byte*)fldf,FLDW*FLDH);
  F_end_blk();
}

extern "C" void W_adjust(void) {
  if(w_x<WD/2) w_x=WD/2;
  if(w_y<HT/2) w_y=HT/2;
  if(w_x>MAXX) w_x=MAXX;
  if(w_y>MAXY) w_y=MAXY;
}

extern "C" void W_draw(void) {
  static int x,y,w,h;

  V_clr(0,200,0,1,0x6C);V_clr(0,200,99,1,0x6C);
  x=scrx;y=scry;w=scrw;h=scrh;
  if(scrx>=WD) return;
  if(scry>=HT+1) return;
  if(scrx+scrw>WD) scrw=WD-scrx;
  if(scry==0) {scry=1;if(!(--scrh)) return;}
  if(scry+scrh>HT+1) scrh=HT+1-scry;
  if(!scrw || !scrh) return;
  V_pic(127-(word)(w_x-WD/2)*56U/(word)(MAXX-WD/2),
	w_o+123-(word)(w_y-HT/2)*28U/(word)(MAXY-HT/2),(vgaimg*)horiz);
  if(w_drawb) Z_drawfld((byte *)fldb);
  if(w_drawth) TH_draw();
  if(w_drawf) Z_drawfld((byte *)fldf);
  if(w_drawt) Z_drawfldt((byte *)fld);
  if(w_drawsw) SW_draw();
  if(edt) V_remap_rect(min(edx1,edx2)*8-w_x+100,abs(edx2-edx1)*8+8,
					   min(edy1,edy2)*8-w_y+50,abs(edy2-edy1)*8+8,clrmap[6]);
  scrx=x;scry=y;scrw=w;scrh=h;
}

extern "C" void W_init(void) {
  int i;

  for(i=0;i<256;++i) walp[i]=NULL;
  memset(wal,0,sizeof(wal));
  memset(fldb,0,FLDW*FLDH);
  memset(fld,0,FLDW*FLDH);
  memset(fldf,0,FLDW*FLDH);
  TH_init();
  SW_init();
}

extern "C" void W_addwall(char *n) {
  int i;

  for(i=0;i<254 && wal[i].n[0];++i);
  if(i>=254) return;
  if(memicmp(n,"_WATER_",7)==0) walp[i+1]=(void*)((int)n[7]-'0'+1);
  else if((walp[i+1]=M_lock(F_findres(n)))==NULL) return;
  memset(wal[i].n,0,8);
  strncpy(wal[i].n,n,8);
  if((dword)walp[i+1]<16) return;
  wal[i].t=0;walf[i+1]=0;
  if(message(MB_YN,"Есть ли в текстуре %.8s прозрачные места?",n)==C_YES)
	{wal[i].t=1;walf[i+1]=1;}
}

extern "C" void W_drawwl(int x,int y,byte t) {
  if((dword)walp[t]<16) return;
  if(walf[t]&1) V_spr(x,y,(vgaimg*)walp[t]);
  else V_pic(x,y,(vgaimg*)walp[t]);
}

extern "C" void W_setsky(void) {
  char s[8];

  strcpy(s,"RSKY*");s[4]=skytype+'0';
  M_unlock(horiz);
  horiz=M_lock(F_getresid(s));
}

extern "C" void W_allocwalls(void) {
  int i;

  W_setsky();
  for(i=0;i<256;++i) walp[i]=NULL;
  for(i=0;i<255 && wal[i].n[0];++i) {
	if(memicmp(wal[i].n,"_WATER_",7)==0)
	  {walp[i+1]=(void*)((int)wal[i].n[7]-'0'+1);continue;}
    walp[i+1]=M_lock(F_getresid(wal[i].n));
    walf[i+1]=wal[i].t;
  }
}
