#include <stdio.h>
#include <dos.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include "..\averr.h"
#include "..\fsound.h"
#include "..\osnddrv.h"

char *S_getinfo(void);

extern void *snd_drv;

char *snd_name="none";

void set_snd_drv(short n) {
  snd_drv=snd_drv_tab[(n>=SDRV__END)?0:n];
  snd_name=S_getinfo();
}

snd *load_dmi(char *fn) {
  int h;
  unsigned l;
  snd *p;

  if((h=open(fn,O_RDONLY|O_BINARY))==-1)
    {error(EZ_LOADSND,ET_STD,errno,fn,NULL);return NULL;}
  if(!(p=malloc((l=filelength(h)))))
    {error(EZ_LOADSND,ET_STD,ENOMEM,fn,NULL);return NULL;}
  read(h,p,l);
  close(h);
  return p;
}

snd *load_snd(char *fn,unsigned r,unsigned ls,unsigned ll) {
  int h;
  unsigned l,i;
  snd *p;
  char *s;

  if((h=open(fn,O_RDONLY|O_BINARY))==-1)
    {error(EZ_LOADSND,ET_STD,errno,fn,NULL);return NULL;}
  if(!(p=malloc((l=filelength(h))+8)))
    {error(EZ_LOADSND,ET_STD,ENOMEM,fn,NULL);return NULL;}
  read(h,p+1,l);
  close(h);
  for(s=(char *)(p+1),i=0;i<l;++i,++s) *s^=0x80;
  p->len=l;p->rate=r;
  p->lstart=ls;
  if((p->llen=ll)==1) {p->llen=l;p->lstart=0;}
  return p;
}
