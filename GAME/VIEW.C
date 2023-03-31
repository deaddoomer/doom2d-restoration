#include "glob.h"
#include <string.h>
#include <malloc.h>
#include "vga.h"
#include "memory.h"
#include "files.h"
#include "error.h"
#include "view.h"
#include "dots.h"
#include "smoke.h"
#include "weapons.h"
#include "items.h"
#include "switch.h"
#include "fx.h"
#include "player.h"
#include "monster.h"
#include "misc.h"
#include "map.h"

#define ANIT 5

#define WD 200
#define HT 98
#define MAXX (FLDW*CELW-WD/2)
#define MAXY (FLDH*CELH-HT/2)

extern map_block_t blk;

extern byte clrmap[256*12];
void V_remap_rect(int,int,int,int,byte *);

byte w_horiz=ON;
static void *horiz=NULL;
int w_o,w_x,w_y,sky_type=1;
void *walp[256];
dword walf[256];
int walh[256];
byte walswp[256];
byte walani[256];
int anih[ANIT][5];
byte anic[ANIT];
byte fldb[FLDH][FLDW];
byte fldf[FLDH][FLDW];
byte fld[FLDH][FLDW];

extern int lt_time,lt_type,lt_side,lt_ypos;
extern void *ltn[2][2];

static void getname(int n,char *s) {
  if(walh[n]==-1) {memset(s,0,8);return;}
  if(walh[n]==-2) {
    memcpy(s,"_WATER_",8);s[7]=(byte)walp[n]-1+'0';
    return;
  }
  F_getresname(s,walh[n]&0x7FFF);
}

static short getani(char *n) {
  if(strnicmp(n,"WALL22_1",8)==0) return 1;
  if(strnicmp(n,"WALL58_1",8)==0) return 2;
  if(strnicmp(n,"W73A_1",8)==0) return 3;
  if(strnicmp(n,"RP2_1",8)==0) return 4;
  return 0;
}

void W_savegame(int h) {
  char s[8];
  int i;

  write(h,&sky_type,4);
  for(i=1;i<256;++i) {
    getname(i,s);write(h,s,8);
  }
  write(h,walf,sizeof(walf));
  write(h,walswp,sizeof(walswp));
  write(h,fldb,FLDW*FLDH);
  write(h,fld,FLDW*FLDH);
  write(h,fldf,FLDW*FLDH);
}

void W_loadgame(int h) {
  char s[8];
  int i;

  read(h,&sky_type,4);
  for(i=1;i<256;++i) {
    walani[i]=0;
    read(h,s,8);if(!s[0]) {walh[i]=-1;walp[i]=NULL;continue;}
    walani[i]=getani(s);
    if(memicmp(s,"_WATER_",7)==0) {
      walh[i]=-2;walp[i]=(void*)(s[7]-'0'+1);
    }else walp[i]=M_lock(walh[i]=F_getresid(s));
  }
  read(h,walf,sizeof(walf));
  for(i=1;i<256;++i) if(walf[i]&1) walh[i]|=0x8000;
  read(h,walswp,sizeof(walswp));
  read(h,fldb,FLDW*FLDH);
  read(h,fld,FLDW*FLDH);
  read(h,fldf,FLDW*FLDH);
  strcpy(s,"RSKY1");s[4]=sky_type+'0';
  M_unlock(horiz);
  horiz=M_lock(F_getresid(s));
}

void W_adjust(void) {
  if(w_x<WD/2) w_x=WD/2;
  if(w_y<HT/2) w_y=HT/2;
  if(w_x>MAXX) w_x=MAXX;
  if(w_y>MAXY) w_y=MAXY;
}

void W_draw(void) {
  W_adjust();
  V_setrect(0,WD,w_o+1,HT);
  if(w_horiz) {
    V_pic(127-(word)(w_x-WD/2)*56U/(word)(MAXX-WD/2),
	  w_o+123-(word)(w_y-HT/2)*28U/(word)(MAXY-HT/2),horiz);
    if(sky_type==2) if(lt_time<0) {
      if(!lt_side) V_spr(0,w_o+lt_ypos,ltn[lt_type][(lt_time<-5)?0:1]);
      else V_spr2(WD-1,w_o+lt_ypos,ltn[lt_type][(lt_time<-5)?0:1]);
    }
  }else V_clr(0,WD,w_o+1,HT,0x97);
/*  x=w_x-WD/2;y=w_y-HT/2;
  sx=x/CELW;xo=-(x%CELW)+1;
  sy=y/CELH;yo=-(y%CELH)+w_o+1;
*/
  Z_drawfld((byte *)fldb);
/*  for(y=1-MAXTXH;y<HT/CELH+2;++y) if(y+sy>0 && y+sy<FLDH)
    for(x=1-MAXTXW;x<WD/CELW+2;++x) if(x+sx>0 && x+sx<FLDW)
    if((c=fldb[sy+y][sx+x])!=0) {
      if(!(p=M_lock(walh[c]))) ERR_fatal("W_draw: not enough memory");
      V_drawwall(x*CELW+xo,y*CELH+yo,wal[c].sx,wal[c].sy,wal[c].w*CELW,wal[c].h*CELH,p);
      M_unlock(walh[c]);
    }
*/
  DOT_draw();
  IT_draw();
  PL_draw(&pl1);
  if(_2pl) PL_draw(&pl2);
  MN_draw();
  WP_draw();
  SMK_draw();
  FX_draw();
  Z_drawfld((byte *)fldf);
  if(sky_type==2)
    if(lt_time==-4 || lt_time==-2)
      V_remap_rect(0,WD,w_o+1,HT,clrmap+256*11);
}

void W_init(void) {
  int i,j;
  static char *anm[ANIT-1][5]={
    {"WALL22_1","WALL23_1","WALL23_2",NULL,NULL},
    {"WALL58_1","WALL58_2","WALL58_3",NULL,NULL},
    {"W73A_1","W73A_2",NULL,NULL,NULL},
    {"RP2_1","RP2_2","RP2_3","RP2_4",NULL}
  };

  for(i=1;i<ANIT;++i) {
    for(j=0;anm[i-1][j];++j)
      anih[i][j]=F_getresid(anm[i-1][j]);
    for(;j<5;++j) anih[i][j]=-1;
  }
  memset(anic,0,sizeof(anic));
  DOT_init();
  SMK_init();
  FX_init();
  WP_init();
  IT_init();
  SW_init();
  PL_init();
  MN_init();
  M_unlock(horiz);
  horiz=M_lock(F_getresid("RSKY1"));
}

void W_act(void) {
  int i,a;

  if(g_time%3!=0) return;
  for(i=1;i<256;++i) if((a=walani[i])!=0) {
    if(anih[a][++anic[a]]==-1) anic[a]=0;
    walp[i]=M_lock(anih[a][anic[a]]);
  }
}

static void unpack(void *buf,int len,void *obuf) {
  byte *p,*o;
  int l,n;

  for(p=(byte*)buf,o=(byte*)obuf,l=len;l;++p,--l) if(*p==255) {
    n=*((word*)(++p));memset(o,*(p+=2),n);o+=n;l-=3;
  }else *(o++)=*p;
}

int W_load(int h) {
  int i,j,k,g;
  static wall_t w;
  void *p,*buf;

  switch(blk.t) {
	case MB_WALLNAMES:
	  for(i=0;i<256;++i) {walh[i]=-1;walswp[i]=i;walani[i]=0;}
	  for(i=1;i<256 && blk.sz>0;++i,blk.sz-=sizeof(w)) {
		read(h,&w,sizeof(w));
		if(memicmp(w.n,"_WATER_",7)==0)
		  {walp[i]=(void*)(w.n[7]-'0'+1);walh[i]=-2;continue;}
		walp[i]=M_lock(walh[i]=F_getresid(w.n));
		if(w.n[0]=='S' && w.n[1]=='W' && w.n[4]=='_') walswp[i]=0;
		walf[i]=(w.t)?1:0;if(w.t) walh[i]|=0x8000;
		if(memicmp(w.n,"VTRAP01",8)==0) walf[i]|=2;
		walani[i]=getani(w.n);
	  }
	  for(j=i,i=1;i<256;++i) if(walswp[i]==0) {
		if(j>=256) break;
		F_getresname(w.n,walh[i]&0x7FFF);
		w.n[5]^=1;
		g=F_getresid(w.n)|(walh[i]&0x8000);
		for(k=1;k<256;++k) if(walh[k]==g) break;
		if(k>=256) {
		  walh[k=j++]=g;walp[k]=M_lock(g);
		  walf[k]=(g&0x8000)?1:0;
		}
		walswp[i]=k;walswp[k]=i;
	  }
	  return 1;
	case MB_BACK:  p=fldb;goto unp;
	case MB_WTYPE: p=fld;goto unp;
	case MB_FRONT: p=fldf;
	unp: switch(blk.st) {
	    case 0: read(h,p,FLDW*FLDH);break;
	    case 1:
	      if(!(buf=malloc(blk.sz)))
	        ERR_fatal("Не хватает памяти");
	      read(h,buf,blk.sz);
	      unpack(buf,blk.sz,p);free(buf);break;
	    default: return 0;
	  }return 1;
	case MB_SKY:
	  sky_type=0;read(h,&sky_type,2);
	  strcpy(w.n,"RSKY1");w.n[4]=sky_type+'0';
	  M_unlock(horiz);
	  horiz=M_lock(F_getresid(w.n));
	  return 1;
  }return 0;
}
