#include "glob.h"
#include <stdio.h>
#include <process.h>
#include <stdarg.h>
#include <conio.h>
#include <time.h>
#include <dos.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "error.h"
#include "config.h"
#include "memory.h"
#include "keyb.h"
#include "sound.h"
#include "vga.h"
#include "files.h"
#include "view.h"
#include "menu.h"
#include "player.h"
#include "misc.h"
#include <harderr.h>

void F_saveres(int r,void *p,int o,int l);

#define RUNT 12

extern char f_drive[],f_dir[],f_name[],f_ext[];

void close_all(void);

dword dpmi_memavl(void);

void K_slow(void);
void K_fast(void);

void textmode(void);
int wherex(void);
int wherey(void);
void gotoxy(int,int);
void putcn(char,char,int);
void cputstr(char *);
void cputch(char);

int _cpu=3;

int gamma=0;

byte fastdraw=0;

static struct{
  dword o;
  byte d[128];
}rom[3];

struct{
  word run;
  word e;
  byte rom[256];
}chk;

char main_pal[256][3],std_pal[256][3];
byte clrmap[256*11];
int snd_card=0;

void logo(const char *s,...) {
  va_list ap;
  int x,y;

  va_start(ap,s);
  vprintf(s,ap);
  va_end(ap);
  fflush(stdout);
  x=wherex();y=wherey();
  gotoxy(1,1);putcn(' ',0x4F,80);
#ifdef DEMO
  gotoxy(34,1);cputstr("DOOM2D V1.30 *demo*");
#else
  gotoxy(34,1);cputstr("DOOM2D V1.30");
#endif
  gotoxy(x,y);
}

void logo_gas(int cur,int all) {
  int x,y,n,i;

  x=wherex();y=wherey();
  n=(78-x)*cur/all+x;
  cputch('[');
  for(i=x;i<n;++i) cputch('.');
  for(;i<78;++i) cputch(' ');
  cputch(']');
  gotoxy(x,y);
}

/*
static void __interrupt __far dbzfunc(void) {
  ERR_fatal("Divide overflow");
}
*/

static const byte gamcor[5][64]={
  #include "gamma.dat"
};

void setgamma(int g) {
  int t;

  if(g>4) g=4;
  if(g<0) g=0;
  gamma=g;
  for(t=0;t<256;++t) {
	std_pal[t][0]=gamcor[gamma][main_pal[t][0]];
	std_pal[t][1]=gamcor[gamma][main_pal[t][1]];
	std_pal[t][2]=gamcor[gamma][main_pal[t][2]];
  }
  VP_setall(std_pal);
}

void randomize(void);

word equp(void);
#pragma aux equp= "int 0x11" value [ax]

int main() {
  int i;

  pl1.ku=0x48;pl1.kd=0x50;pl1.kl=0x4B;pl1.kr=0x4D;pl1.kf=0xB8;pl1.kj=0x9D;
  pl1.kwl=0x47;pl1.kwr=0x49;pl1.kp=0x36;
  pl1.id=-1;
  pl2.ku=0x11;pl2.kd=0x1F;pl2.kl=0x1E;pl2.kr=0x20;pl2.kf=0x3A;pl2.kj=0x0F;
  pl2.kwl=0x10;pl2.kwr=0x12;pl2.kp=0x2A;
  pl2.id=-2;
  textmode();gotoxy(1,2);
//  _dos_setvect(0,dbzfunc);
  randomize();
  F_startup();
  F_addwad("Doom2D.wad");
  CFG_args();
  CFG_load();
  F_initwads();
  F_set_snddrv();
  M_startup();
  F_allocres();
  F_loadres(F_getresid("PLAYPAL"),main_pal,0,768);
  F_loadres(F_getresid("COLORMAP"),clrmap,0,256*11);
  F_loadres(F_getresid("COLORMAP"),rom,256*11,132*3);
  F_loadres(F_getresid("COLORMAP"),&chk,256*11+132*3,sizeof(chk));
{
    int flg=1;
    for(i=0;i<3;++i) if(rom[i].o)
      if(memcmp((void*)rom[i].o,rom[i].d,128)!=0)
        flg=0;
    if((equp()&0xCEC1)!=chk.e) flg=0;
    if(memcmp((void*)0xFFF00,chk.rom,256)!=0)
      flg=0;
  if(flg) {
    chk.run=0;
    F_saveres(F_getresid("COLORMAP"),&chk,256*11+132*3,2);
  }else if(!chk.run) chk.run=RUNT;
  if(chk.run>1) {
    --chk.run;
    F_saveres(F_getresid("COLORMAP"),&chk,256*11+132*3,2);
  }
}
//logo("*******************************************************************\n");
//logo("**                                                               **\n");
//logo("**                          DEMO-ВЕРСИЯ                          **\n");
//logo("**                                                               **\n");
//logo("**                     ВРЕМЯ ИГРЫ ОГРАНИЧЕНО                     **\n");
//logo("**                                                               **\n");
//logo("*******************************************************************\n");
  G_init();
  logo("  free DPMI memory: %uK\n",dpmi_memavl()>>10);
  logo("K_init: setting up keyboard\n");
  K_slow();K_init();
  logo("T_init: setting up timer\n");
  T_init();
  logo("S_init: setting up sound\n");
  S_init();
  GM_init();
  logo("V_init: setting up video\n");
  if(V_init()!=0) ERR_failinit("Unable to set VGA video mode");
  setgamma(gamma);
  V_setscr(scrbuf);
  for(;;) {
    timer=0;
    G_act();
    if(fastdraw) G_act();
    G_draw();
    if(fastdraw) while(timer<0x1FFFE); else while(timer<0xFFFF);
  }
}
