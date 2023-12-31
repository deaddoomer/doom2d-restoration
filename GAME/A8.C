#include "glob.h"
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <vga.h>
#include <keyb.h>
#include "error.h"
#include "files.h"
#include "sound.h"

#define WAIT_SZ 400000

extern byte gamcor[5][64];

extern char cd_path[];

extern int gamma;
void setgamma(int);

extern void *cd_scr;

static byte skipping=0,credits=0;

#define A8_ID 0xA8

enum{
  A8C_ENDFR,A8C_PAL,A8C_CLRSCR,A8C_DRAW,
  A8C_FILL,A8C_DRAW2C,A8C_DRAW2
};

typedef struct{
  unsigned char id,ver;
  short width,height,frames;
  long maxfsize;
  long f1size;
}a8_hdr_t;

typedef unsigned char uchar;

#define SQ 4

static int sqw,sqh;

static int norm_gamma;

static unsigned char *frp,sqc[2][50][80];
static int frame;
static a8_hdr_t ah;
static snd_t *strk;
static int strk_ch;

static signed char *unpack(char *d,signed char *p,int l) {
  for(;l>0;) if(*p>0) {
    memcpy(d,p+1,*p);d+=*p;l-=*p;p+=*p+1;
  }else if(*p<0) {
    memset(d,p[1],-*p);d+=-*p;l-=-*p;p+=2;
  }else return p+1;
  return p;
}

static unsigned char *draw(signed char *p) {
  int x,y,sy,yc,n;

  for(y=0;y<sqh;) if(*p>0) {
    for(yc=*p++;yc;--yc,++y) for(x=0;x<sqw;) if(*p>0) {
      n=(*p++)*SQ;
      for(sy=0;sy<SQ;++sy) {
        p=unpack(scra+(y*SQ+sy)*320+x*SQ,p,n);
      }
      x+=n/SQ;
    }else x+=-*p++;
  }else y+=-*p++;
  return p;
}

static unsigned char *fill(signed char *p) {
  int x,y,yc,n;

  for(y=0;y<sqh;) if(*p>0) {
    for(yc=*p++;yc;--yc,++y) for(x=0;x<sqw;) if(*p>0) {
      for(n=*p++;n;--n,++p,++x)
        V_clr(x*SQ,SQ,y*SQ,SQ,*p);
    }else x+=-*p++;
  }else y+=-*p++;
  return p;
}

static unsigned char *draw2c(signed char *p) {
  int x,y,sx,sy,yc,n;
  unsigned short w;

  for(y=0;y<sqh;) if(*p>0) {
    for(yc=*p++;yc;--yc,++y) for(x=0;x<sqw;) if(*p>0) {
      for(n=*p++;n;--n,++x) {
        sqc[0][y][x]=*p++;
        sqc[1][y][x]=*p++;
        w=*(unsigned short*)p;p+=2;
        for(sy=0;sy<SQ;++sy)
          for(sx=0;sx<SQ;++sx,w>>=1)
            scra[(y*SQ+sy)*320+x*SQ+sx]=sqc[w&1][y][x];
      }
    }else x+=-*p++;
  }else y+=-*p++;
  return p;
}

static unsigned char *draw2(signed char *p) {
  int x,y,sx,sy,yc,n;
  unsigned short w;

  for(y=0;y<sqh;) if(*p>0) {
    for(yc=*p++;yc;--yc,++y) for(x=0;x<sqw;) if(*p>0) {
      for(n=*p++;n;--n,++x) {
        w=*(unsigned short*)p;p+=2;
        for(sy=0;sy<SQ;++sy)
          for(sx=0;sx<SQ;++sx,w>>=1)
            scra[(y*SQ+sy)*320+x*SQ+sx]=sqc[w&1][y][x];
      }
    }else x+=-*p++;
  }else y+=-*p++;
  return p;
}

//---------------------------------------------------------------//

static int fh,fsz,fdptr;
static char *fdata;

//static void f_read(void *p,int l) {
//  if(fdata) {
//    memcpy(p,fdata+fdptr,l);fdptr+=l;
//  }else read(fh,p,l);
//}

//static void f_skip(int n) {
//  lseek(fh,n,SEEK_CUR);
//}

static void f_close(void) {
  if(fdata) {
    free(fdata);fdata=NULL;
  }else if(fh!=-1) {close(fh);fh=-1;}
}

static char end_clr=1;

void A8_close(void) {
  if(strk) if(strk_ch) S_stop(strk_ch);
  f_close();
  if(frp) {free(frp);frp=NULL;}
  if(strk) {free(strk);strk=NULL;}
  if(end_clr) {
    VP_fill(0,0,0);
    V_clr(0,320,0,200,0);V_copytoscr(0,320,0,200);
    setgamma(norm_gamma);
  }
}

int A8_nextframe(void) {
  unsigned char *p;
  int i,j,k;
  static int len;

  if(credits) if(keys[0x33] && keys[0x34]) skipping=1;
  if(frame==-1) if(strk) strk_ch=S_play(strk,-1,1024,255);
    if(fdata) {
      len=*(int*)(fdata+fdptr);fdptr+=4;
    }else {len=0;read(fh,&len,4);}
    len-=4;
    if(len<=0) {
      A8_close();
      return 0;
    }
    if(fdata) {
      p=fdata+fdptr;fdptr+=len;
    }else {read(fh,frp,len);p=frp;}
    for(;*p;) switch(*p++) {
      case A8C_PAL:
        i=*p++;j=*p++;if(!j) j=256;
        for(k=0;k<j*3;++k) p[k]=gamcor[3][p[k]];
        VP_set(p,i,j);
        p+=j*3;
        break;
      case A8C_CLRSCR:
        V_clr(0,ah.width,0,ah.height,*p++);
        break;
      case A8C_DRAW:
        p=draw(p);
        break;
      case A8C_FILL:
        p=fill(p);
        break;
      case A8C_DRAW2C:
        p=draw2c(p);
        break;
      case A8C_DRAW2:
        p=draw2(p);
        break;
      default:
        ERR_fatal("Плохой блок в файле A8");
    }
    ++frame;
  return 1;
}

static char wscr;

static void wait_scr(int s) {
  if(!end_clr) return;
  if(s<WAIT_SZ) return;
  F_freemus();
  V_setrect(0,320,0,200);
  V_clr(0,320,0,200,0);
  V_copytoscr(0,320,0,200);
  V_pic(0,0,(void*)((char*)cd_scr+768));
  VP_setall(cd_scr);
  V_copytoscr(0,320,0,200);
  wscr=1;
}

static void blank_scr(void) {
  VP_fill(0,0,0);
  V_setrect(0,320,0,200);
  V_clr(0,320,0,200,0);
  V_copytoscr(0,320,0,200);
}

int A8_start(char *nm) {
  static char s[40];
  int sz,h;
  unsigned char *p;

  end_clr=1;
  if(stricmp(nm,"FINAL")==0 || stricmp(nm,"CREDITS")==0) end_clr=0;
  else if(stricmp(nm,"KONEC")==0) end_clr=0;
  credits=(stricmp(nm,"FINAL")==0)?1:0;
  if(stricmp(nm,"CREDITS")==0) if(skipping) return 0;
  wscr=0;
  strk=NULL;strk_ch=0;
  fdata=NULL;frp=NULL;
  if(snd_type!=ST_NONE) {
    sprintf(s,"%sA8\\%s.SND",cd_path,nm);
    if((h=open(s,O_BINARY|O_RDONLY))!=-1) {
      lseek(h,0,SEEK_END);sz=tell(h);lseek(h,0,SEEK_SET);
      if((strk=malloc(sz+sizeof(snd_t)))!=NULL) {
        wait_scr(sz);
        read(h,strk+1,sz);
        strk->rate=11000;
        strk->len=sz;
        strk->lstart=strk->llen=0;
        for(p=(unsigned char *)(strk+1);sz;--sz,++p) *p^=0x80;
      }
      close(h);
    }
  }
  sprintf(s,"%sA8\\%s.A8",cd_path,nm);
  if((fh=open(s,O_BINARY|O_RDONLY))==-1) {
//    if(strk) {free(strk);strk=NULL;}
//    return 0;
    ERR_fatal("Не могу открыть файл %s",s);
  }
  read(fh,&ah,sizeof(ah)-4);
  if(ah.id!=A8_ID || ah.ver!=0) ERR_fatal("Испорченный файл A8 %s",s);
  lseek(fh,0,SEEK_END);
  fsz=tell(fh)-sizeof(ah)+4;
  lseek(fh,sizeof(ah)-4,SEEK_SET);
  if((fdata=malloc(fsz))!=NULL) {
    wait_scr(fsz);
    read(fh,fdata,fsz);
    fdptr=0;
    close(fh);fh=-1;
  }else if(!(frp=malloc(ah.maxfsize))) {
    if(strk) {free(strk);strk=NULL;}
    if(!(frp=malloc(ah.maxfsize))) {
      close(fh);fh=-1;return 0;
    }
  }
  sqw=ah.width/SQ;sqh=ah.height/SQ;
  frame=-1;
  norm_gamma=gamma;
  if(wscr) blank_scr();
//  if(gamma<3) setgamma(3);
  return 1;
}
