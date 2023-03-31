#include "glob.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <dos.h>
#include "config.h"
#include "vga.h"
#include "error.h"
#include "files.h"
#include "memory.h"
#include "view.h"

enum{NONE,BYTE,WORD,DWORD,STRING,SW_ON,SW_OFF,FILES,MAINWAD};

extern int gamma;

void F_mainwad(char *);

typedef struct{
  char *par,*cfg;
  void *p;
  byte t,o;
}cfg_t;

static cfg_t cfg[]={
  {"file",NULL,NULL,FILES,0},
  {"mainwad","main_wad",NULL,MAINWAD,0},
  {"waitrr","wait_retrace",&vp_waitrr,SW_ON,0},
  {"nowaitrr",NULL,&vp_waitrr,SW_OFF,0},
  {"gamma","gamma",&gamma,WORD,0},
  {"config",NULL,cfg_file,STRING,0},
  {NULL,NULL,NONE,0}
};

char cfg_file[128]="EDITOR.CFG";

static char buf[256];

void CFG_args(void) {
  int j;
  dword n;
  char *s;

  logo("CFG_args: checking arguments\n");
  // HACK: -4 is hack to match original editor.exe layout
  for(s=strtok(getcmd(buf-4)," \r\n\t");s;s=strtok(NULL," \r\n\t")) {
next:
    if(*s=='/' || *s=='-') ++s;
    for(j=0;cfg[j].t;++j) if(cfg[j].par) if(stricmp(s,cfg[j].par)==0) {
	  switch(cfg[j].t) {
	case BYTE:
	  n=strtol(s=strtok(NULL," \r\n\t"),NULL,0);
	  *((byte *)cfg[j].p)=(byte)n;
	  break;
	case WORD:
	  n=strtol(s=strtok(NULL," \r\n\t"),NULL,0);
	  *((word *)cfg[j].p)=(word)n;
	  break;
	case DWORD:
	  n=strtol(s=strtok(NULL," \r\n\t"),NULL,0);
	  *((dword *)cfg[j].p)=n;
	  break;
	case STRING:
	  strcpy((char *)cfg[j].p,s=strtok(NULL," \r\n\t"));
	  break;
	case SW_ON:
	  *((byte *)cfg[j].p)=ON;
	  if(cfg[j+1].t==SW_OFF && cfg[j+1].p==cfg[j].p) cfg[j+1].o=1;
	  if(j>0) if(cfg[j-1].t==SW_OFF && cfg[j-1].p==cfg[j].p) cfg[j-1].o=1;
	  break;
	case SW_OFF:
	  *((byte *)cfg[j].p)=OFF;
	  if(cfg[j+1].t==SW_ON && cfg[j+1].p==cfg[j].p) cfg[j+1].o=1;
	  if(j>0) if(cfg[j-1].t==SW_ON && cfg[j-1].p==cfg[j].p) cfg[j-1].o=1;
	  break;
	case FILES:
	  for(s=strtok(NULL," \r\n\t");s;s=strtok(NULL," \r\n\t")) {
		if(*s=='/' || *s=='-') goto next;
		F_addwad(s);
	  }break;
	case MAINWAD:
	  F_mainwad(s=strtok(NULL," \r\n\t"));
	  break;
	default:
	  ERR_failinit("BUG: unknown type in cfg!");
	  }
	  cfg[j].o=1;break;
    }
  }
}

void CFG_load(void) {
  int j,h;
  dword n;
  char s[128];
  char *p1,*p2;

  logo("CFG_load: loading config from %s\n",cfg_file);
  if((h=open(cfg_file,O_RDONLY|O_BINARY))==-1) {
    perror("Cannot open file");return;
  }
  while(!eof(h)) {
    F_readstr(h,s,127);
	if(*s==';' || s[1]==';') continue; // comment
    if(!(p1=strtok(s,"\r\n\t =;"))) continue;
    if(!(p2=strtok(NULL,"\r\n\t =;"))) continue;
    for(j=0;cfg[j].t;++j) if(cfg[j].cfg && !cfg[j].o)
     if(stricmp(p1,cfg[j].cfg)==0) {
      switch(cfg[j].t) {
	case BYTE:
	  n=strtol(p2,NULL,0);
	  *((byte *)cfg[j].p)=(byte)n;
	  break;
	case WORD:
	  n=strtol(p2,NULL,0);
	  *((word *)cfg[j].p)=(word)n;
	  break;
	case DWORD:
	  n=strtol(p2,NULL,0);
	  *((dword *)cfg[j].p)=n;
	  break;
	case STRING:
	  strcpy((char *)cfg[j].p,p2);
	  break;
	case SW_ON:
	case SW_OFF:
	  if(stricmp(p2,"ON")==0) {*((byte *)cfg[j].p)=ON;break;}
	  if(stricmp(p2,"OFF")==0) {*((byte *)cfg[j].p)=OFF;break;}
	  *((byte *)cfg[j].p)=strtol(p2,NULL,0);
	  break;
	case FILES:
	  break;
	case MAINWAD:
	  F_mainwad(p2);
	  break;
	default:
	  ERR_failinit("BUG: unknown type in cfg!");
	  }
	  break;
    }
  }
  close(h);
}
