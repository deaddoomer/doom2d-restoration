#include <stdio.h>
#include <dos.h>
#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include "..\averr.h"
#include "..\sound.h"
#include "..\snddrv.h"

typedef struct{
  unsigned char n,i,v,d;
}dmv;

unsigned char seq[255],seqn;
dmv *pat=NULL;
unsigned *patp;
void **dmi;

static int inum=0;

char *S_getinfo(void);

extern void *snd_drv;

char *snd_name="none";

void set_snd_drv(short n) {
  snd_drv=snd_drv_tab[(n>=SDRV__END)?0:n];
  snd_name=S_getinfo();
}

snd_t *load_dmi(char *fn) {
  int h;
  unsigned l;
  snd_t *p;

  if((h=open(fn,O_RDONLY|O_BINARY))==-1)
    {error(EZ_LOADSND,ET_STD,errno,fn,NULL);return NULL;}
  if(!(p=malloc((l=filelength(h))+8)))
    {error(EZ_LOADSND,ET_STD,ENOMEM,fn,NULL);return NULL;}
  p->len=p->rate=p->lstart=p->llen=0;
  read(h,((int*)p),2);
  read(h,((int*)p)+1,2);
  read(h,((int*)p)+2,2);
  read(h,((int*)p)+3,2);
  read(h,p+1,l-8);
  close(h);
  return p;
}

snd_t *load_snd(char *fn,unsigned r,unsigned ls,unsigned ll) {
  int h;
  unsigned l,i;
  snd_t *p;
  char *s;

  if((h=open(fn,O_RDONLY|O_BINARY))==-1)
    {error(EZ_LOADSND,ET_STD,errno,fn,NULL);return NULL;}
  if(!(p=malloc((l=filelength(h))+16)))
    {error(EZ_LOADSND,ET_STD,ENOMEM,fn,NULL);return NULL;}
  read(h,p+1,l);
  close(h);
  for(s=(char *)(p+1),i=0;i<l;++i,++s) *s^=0x80;
  p->len=l;p->rate=r;
  p->lstart=ls;
  if((p->llen=ll)==1) {p->llen=l;p->lstart=0;}
  return p;
}

void free_music(void) {
  int i;

  S_stopmusic();
  if(!pat) return;
  free(pat);free(patp);
  for(i=0;i<inum;++i) if(dmi[i]!=NULL) free(dmi[i]);
  free(dmi);
}

short load_dmm(char *fn) {
  int r,h;
  unsigned i,j;
  struct{
    char id[4];
    unsigned char ver,pat;
    unsigned short psz;
  }d;
  struct{char t;char n[13];short r;}di;

  free_music();
  if((h=open(fn,O_RDONLY|O_BINARY))==-1) return 0;
  read(h,&d,sizeof(d));
  if(memcmp(d.id,"DMM",4)!=0) {close(h);return 0;}
  if(!(pat=malloc(d.psz<<2))) {close(h);return 0;}
  read(h,pat,d.psz<<2);
  read(h,&seqn,1);if(seqn) read(h,seq,seqn);
  inum=0;read(h,&inum,1);
  if(!(dmi=malloc(inum*4)))
    {free(pat);pat=NULL;close(h);return 0;}
  if(!(patp=malloc((unsigned)d.pat*32)))
    {free(pat);free(dmi);pat=NULL;close(h);return 0;}
  for(i=0;i<inum;++i) dmi[i]=NULL;
  for(i=0;i<inum;++i) {
    read(h,&di,16);
    if(!di.n[0]) continue;
//    if((r=open(di.n,O_RDONLY|O_BINARY))==-1) {free_music();return 0;}
//    if(!(dmi[i]=malloc(j=filelength(r)))) {close(r);free_music();return 0;}
//    read(r,dmi[i],j);close(r);
    if(!(dmi[i]=load_dmi(di.n))) {free_music();close(h);return 0;}
  }
  for(i=r=0,j=(unsigned)d.pat<<3;i<j;++i) {
    patp[i]=r<<2;
    while(pat[r++].v!=0x80);
  }
  close(h);
  return 1;
}
