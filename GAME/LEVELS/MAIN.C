#include "glob.h"
#include <stdio.h>
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
#include "averr.h"
#include "vga.h"
#include "files.h"
#include "view.h"
#include "misc.h"

extern char f_drive[],f_dir[],f_name[],f_ext[];

void textmode(void);
int wherex(void);
int wherey(void);
void gotoxy(int,int);
void putcn(char,char,int);
void cputstr(char *);
void cputch(char);

int gamma=0;

char std_pal[256][3],mainpal[256][3];
byte clrmap[256*11];

void logo(const char *s,...) {
  va_list ap;
  int x,y;

  va_start(ap,s);
  vprintf(s,ap);
  va_end(ap);
  fflush(stdout);
  x=wherex();y=wherey();
  gotoxy(1,1);putcn(' ',0x1F,80);
  gotoxy(25,1);cputstr("Редактор уровней   Версия 1.32");
  gotoxy(x,y);
}

void setgamma(int g) {
  int t;
  static const byte gam[5][64]={
	#include "..\gamma.dat"
  };

  if(g>4) g=4;
  if(g<0) g=0;
  gamma=g;
  for(t=0;t<256;++t) {
	std_pal[t][0]=gam[gamma][mainpal[t][0]];
	std_pal[t][1]=gam[gamma][mainpal[t][1]];
	std_pal[t][2]=gam[gamma][mainpal[t][2]];
  }
  VP_setall(std_pal);
}

void error(int z,int t,int n,char *s1,char *s2) {
  char *m;

  if(t==ET_STD) m=strerror(n); else m=av_err_msg[n];
  ERR_fatal("%s",m);
}

void edit(void);

int main() {
  textmode();gotoxy(1,2);
  randomize();
  F_addwad("CMRTKA.WAD");
  CFG_args();
  F_startup();
  CFG_load();
  F_initwads();
  M_startup();
  F_allocres();
  TH_alloc();
  W_init();W_allocwalls();
  F_loadres(F_getresid("PLAYPAL"),mainpal,0,768);
  F_loadres(F_getresid("COLORMAP"),clrmap,0,256*11);
  edit();
  ERR_quit();
  return 0;
}
