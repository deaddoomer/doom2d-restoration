#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "..\averr.h"

void readstrz(int,char *,int);

typedef struct{
  unsigned short w,h;
  short sx,sy;
}vgaimg;

short load_pal(char *fn,void *pal) {
  int h;
  char s[7];

  if((h=open(fn,O_BINARY|O_RDONLY))==-1)
    {error(EZ_LOADVGA,ET_STD,errno,"palette",fn);return 0;}
  read(h,s,7);
  if(memcmp(s,"VGAED2",7)!=0)
    {error(EZ_LOADVGA,ET_AVLIB,EN_BADFORMAT,"palette",fn);close(h);return 0;}
  read(h,pal,768);
  close(h);
  return 1;
}

void *load_vga(char *fn,char *in) {
  int h;
  vgaimg v;
  char *p;
  char s[41];
  unsigned sz;

  if((h=open(fn,O_BINARY|O_RDONLY))==-1)
    {error(EZ_LOADVGA,ET_STD,errno,in,fn);return NULL;}
  read(h,s,7);
  if(memcmp(s,"VGAED2",7)!=0)
    {close(h);error(EZ_LOADVGA,ET_AVLIB,EN_BADFORMAT,in,fn);return NULL;}
  lseek(h,768,SEEK_CUR);
  for(;;) {
    readstrz(h,s,40);
    if(*s==0) break;
    if(read(h,&v,sizeof(vgaimg))!=sizeof(vgaimg))
      {close(h);error(EZ_LOADVGA,ET_AVLIB,EN_BADFORMAT,in,fn);return NULL;}
    sz=(unsigned)v.w*v.h;
    if(stricmp(s,in)==0) {
      if(!(p=malloc(sz+sizeof(vgaimg))))
        {close(h);error(EZ_LOADVGA,ET_STD,ENOMEM,in,fn);return NULL;}
      memcpy(p,&v,sizeof(vgaimg));
      read(h,p+sizeof(vgaimg),sz);
      close(h);
      return p;
    }else lseek(h,sz,SEEK_CUR);
  }close(h);
  error(EZ_LOADVGA,ET_AVLIB,EN_RESNOTFOUND,in,fn);
  return NULL;
}
