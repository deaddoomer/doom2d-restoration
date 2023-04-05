#include <stdio.h>
#include <malloc.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <direct.h>
#include <dos.h>
#include <sound.h>
#include <snddrv.h>
#include <keyb.h>
#include <vga.h>
#include <averr.h>
#include <harderr.h>
#include <process.h>

enum{END,BYTE,WORD,DWORD,SWITCH};

char _fast=0;

char bright[256];

extern int doom_sfnt,doom_bfnt;

#define TSNDS 10
snd_t *testsnd[TSNDS];
snd_t *ipath_snd,*igo_snd,*iend_snd;

//------------------------------------------------------------------//

extern void flame_sprf(void);

#define MAXFLM 1000

static struct{int x,y,xv,yv;short s;} flm[MAXFLM];

extern vgaimg flame01,flame02,flame03,flame04;
extern vgaimg flame05,flame06,flame07,flame08;

static vgaimg *fspr[8]={
  &flame01,&flame02,&flame03,&flame04,
  &flame05,&flame06,&flame07,&flame08
};

void init_flame(void) {
  int i;

  for(i=0;i<MAXFLM;++i) flm[i].s=-1;
}

void new_flame(int ax,int ay,int axv,int ayv) {
  int i;

  for(i=0;i<MAXFLM;++i) if(flm[i].s==-1) {
    flm[i].x=ax;flm[i].y=ay;flm[i].xv=axv;flm[i].yv=ayv;flm[i].s=0;
    return;
  }
}

void add_flame(int x,int y,int xv,int yv,int d) {
  int i,m;

//  fxv+=xv<<2;fyv+=yv<<2;
  if(_fast) return;
  m=max(abs(xv),abs(yv));
  if(!m) m=1;
  for(i=0;i<m;i+=3) {
    new_flame((x<<3)+(xv<<3)*i/m+(rand()%(d*2+1)-d),
      (y<<3)+(yv<<3)*i/m+(rand()%(d*2+1)-d),
      rand()%33-16,-rand()%20);
  }
}

void flame_act(void) {
  int i;

  for(i=0;i<MAXFLM;++i) if(flm[i].s>=0) {
    if(flm[i].xv) flm[i].xv+=(flm[i].xv>0)?-1:1;
    if(flm[i].yv) flm[i].yv+=(flm[i].yv>0)?-1:1;
    flm[i].x+=flm[i].xv;
    flm[i].y+=flm[i].yv;
    if(++flm[i].s>=8) flm[i].s=-1;
  }
}

void flame_draw(void) {
  int i;

  for(i=0;i<MAXFLM;++i) if(flm[i].s>=0) {
    V_sprf(flm[i].x>>3,flm[i].y>>3,fspr[flm[i].s],flame_sprf);
  }
}

void V_cursor(int x,int y) {
  new_flame((x<<3)+rand()%17-8,(y<<3)+3*8+rand()%17-8,rand()%17-8,-rand()%15-5);
}

//------------------------------------------------------------------//

#define MAXFMOV 100

static struct{int x1,y1,x2,y2,t,d;} fmov[MAXFMOV];

void init_fmov(void) {
  int i;

  for(i=0;i<MAXFMOV;++i) fmov[i].t=0;
}

void new_fmov(int x1,int y1,int x2,int y2,int t,int d) {
  int i;

  for(i=0;i<MAXFMOV;++i) if(!fmov[i].t) {
    fmov[i].t=t;fmov[i].t=d;
    fmov[i].x1=x1;fmov[i].y1=y1;
    fmov[i].x2=x2;fmov[i].y2=y2;
    return;
  }
}

void fmov_act(void) {
  int x,y,i;

  for(i=0;i<MAXFMOV;++i) if(fmov[i].t>0) {
    x=fmov[i].x1;y=fmov[i].y1;
    fmov[i].x1=(fmov[i].x1-fmov[i].x2)*(fmov[i].t-1)/fmov[i].t+fmov[i].x2;
    fmov[i].y1=(fmov[i].y1-fmov[i].y2)*(fmov[i].t-1)/fmov[i].t+fmov[i].y2;
    add_flame(x,y,fmov[i].x1-x,fmov[i].y1-y,fmov[i].d);
    --fmov[i].t;
  }
}

static char vputs_flm=0;

void vputs(char *s) {
  int w,h;

  if(vputs_flm) {
    w=V_strlen(s);
    h=(vf_font==&doom_sfnt)?4:8;
    if(rand()&1) new_fmov(vf_x,vf_y+h,vf_x+w,vf_y+h,5,h);
    else new_fmov(vf_x+w,vf_y+h,vf_x,vf_y+h,5,h);
  }
  V_puts(s);
}

//------------------------------------------------------------------//

enum{
  NONE,MAIN,SOUND,SCARD,SPORT,SDMA,SIRQ,SFREQ,RKPL,RKEYS,
  TESTYN,SNDYN,TESTSND,
  IDIR,INST,IDONE,
  MAX_MNU
};

int mnu=NONE,swmnu=SCARD,need_exit=0,redraw=0,def_k=0;
int cur[MAX_MNU];
int total[MAX_MNU]={0,3,6,SDRV__END,0,0,0,10,2,9,2,2,0,0,0,0};

extern vgapal doompal;
extern vgaimg cursor;
extern short *snd_par;

unsigned char plk[2][9]={
  {75,77,72,80,0x9D,0xB8,73,71,54},
  {31,33,18,32,16,30,19,17,15}
};

char newscr[64008];

char cfile[256]="DEF.CFG";
char ddir[256]="C:\\CMRTKA";
char sdir[256]="";

int snd_card=0;

typedef struct{
  char *n;
  char t;
  void *p;
}cfg_t;

cfg_t cfg[]={
  {"sound_card",DWORD,&snd_card},
  {"sound_rate",WORD,&sfreq},
  {"sound_port",WORD,&snd_port},
  {"sound_dma",WORD,&snd_dma},
  {"sound_irq",WORD,&snd_irq},
  {"pl1_left",BYTE,&plk[0][0]},
  {"pl1_right",BYTE,&plk[0][1]},
  {"pl1_up",BYTE,&plk[0][2]},
  {"pl1_down",BYTE,&plk[0][3]},
  {"pl1_jump",BYTE,&plk[0][4]},
  {"pl1_fire",BYTE,&plk[0][5]},
  {"pl1_next",BYTE,&plk[0][6]},
  {"pl1_prev",BYTE,&plk[0][7]},
  {"pl1_use",BYTE,&plk[0][8]},
  {"pl2_left",BYTE,&plk[1][0]},
  {"pl2_right",BYTE,&plk[1][1]},
  {"pl2_up",BYTE,&plk[1][2]},
  {"pl2_down",BYTE,&plk[1][3]},
  {"pl2_jump",BYTE,&plk[1][4]},
  {"pl2_fire",BYTE,&plk[1][5]},
  {"pl2_next",BYTE,&plk[1][6]},
  {"pl2_prev",BYTE,&plk[1][7]},
  {"pl2_use",BYTE,&plk[1][8]},
  {NULL,END,NULL}
};

void close_all(void) {
  V_done();K_done();S_done();
}

void error(int z,int t,int n,char *s1,char *s2) {
  char *m;

  close_all();
  printf(av_ez_msg[z],s1,s2);
  if(t==ET_STD) m=strerror(n); else m=av_err_msg[n];
  printf(":\n  %s\n",m);
  exit(1);
}

static char prbuf[500];

void V_printf(char *s,...) {
  va_list ap;

  va_start(ap,s);
  vsprintf(prbuf,s,ap);
  va_end(ap);
  vf_font=&doom_sfnt;
  vputs(prbuf);
}

void V_prhdr(int y,char *s) {
  vf_font=&doom_bfnt;
  vf_x=160-V_strlen(s)/2;vf_y=y;
  vputs(s);
  vf_font=&doom_sfnt;
}

#define gotoxy(x,y) {vf_x=(x);vf_y=(y);}

void save_cfg(void) {
  FILE *oh;
  cfg_t *c;

  if(!(oh=fopen(cfile,"wt"))) return;
    fprintf(oh,";Файл конфигурации\n\n");
    fprintf(oh,"cd_path=%s\n",sdir);
    fprintf(oh,"gamma=1\n");
    fprintf(oh,"sound_volume=128\n");
    fprintf(oh,"music_volume=128\n");
    fprintf(oh,"sound_interp=off\n");
  for(c=cfg;c->t;++c) switch(c->t) {
    case BYTE:
      fprintf(oh,"%s=%u\n",c->n,*(unsigned char*)c->p);
      break;
    case SWITCH:
      fprintf(oh,"%s=%s\n",c->n,(*(char*)c->p)?"on":"off");
      break;
    case WORD:
      fprintf(oh,"%s=%u\n",c->n,*(unsigned short*)c->p);
      break;
    case DWORD:
      fprintf(oh,"%s=%u\n",c->n,*(unsigned*)c->p);
      break;
  }
  fclose(oh);
}

void prmenu(int x,int y,int c,...) {
  char **ap;

  if(c>=0) V_cursor(x-10,y+c*10);
  vf_font=(void*)&doom_sfnt;
  ap=(char**)(&c+1);
  while(*ap) {
    vf_x=x;vf_y=y;
    vputs(*ap);
    ++ap;
    y+=10;
  }
}

char *keyname(short k) {
  static char *nm[83]={
    "ESC","1","2","3","4","5","6","7","8","9","0","-","=","BACKSPACE","TAB",
    "Q","W","E","R","T","Y","U","I","O","P","[","]","ENTER","ЛЕВЫЙ CONTROL","A","S",
    "D","F","G","H","J","K","L",";","\"","\'","ЛЕВЫЙ SHIFT","\\","Z","X","C","V",
    "B","N","M","<",">","/","ПРАВЫЙ SHIFT","*","ЛЕВЫЙ ALT","ПРОБЕЛ","CAPS LOCK","F1","F2","F3","F4","F5",
    "F6","F7","F8","F9","F10","NUM LOCK","SCROLL LOCK","[7]","[8]","[9]","СЕРЫЙ -","[4]","[5]","[6]","СЕРЫЙ +","[1]",
    "[2]","[3]","[0]","[.]"
  };
  static char s[20];

  if(k>=1 && k<=0x53) return nm[k-1];
  if(k==0) return "???";
  if(k==0x57) return "F11";
  if(k==0x58) return "F12";
  switch(k) {
    case 0x9C: return "СЕРЫЙ ENTER";
    case 0x9D: return "ПРАВЫЙ CONTROL";
    case 0xB8: return "ПРАВЫЙ ALT";
    case 0xC7: return "HOME";
    case 0xC8: return "ВВЕРХ";
    case 0xC9: return "PAGE UP";
    case 0xCB: return "ВЛЕВО";
    case 0xCD: return "ВПРАВО";
    case 0xCF: return "END";
    case 0xD0: return "ВНИЗ";
    case 0xD1: return "PAGE DOWN";
    case 0xD2: return "INSERT";
    case 0xD3: return "DELETE";
  }
  sprintf(s,"КЛАВИША #%X",k);
  return s;
}

void drawmenu(int m) {
  short *p;
  int i;
  static char *kn[9]={
    "ВЛЕВО","ВПРАВО","ВВЕРХ","ВНИЗ","ПРЫЖОК","ОГОНЬ",
    "СЛЕД. ОРУЖИЕ","ПРЕД. ОРУЖИЕ","ОТКРЫВАТЬ"
  };

  V_setrect(0,320,0,200);
//  draw_back();
  V_clr(0,320,0,200,0);
  switch(m) {
    case MAIN:
      V_prhdr(35,"SETUP");
      prmenu(100,80,cur[m],"НАСТРОЙКА ЗВУКА","ВЫБОР КЛАВИШ","ВЫХОД В DOS",NULL);
      break;
    case SOUND:
      V_prhdr(35,"НАСТРОЙКА ЗВУКА");
      gotoxy(100,80);
      if(snd_card) V_printf("КАРТА %s",strupr(snd_name));
        else vputs("ЗВУКОВОЙ КАРТЫ НЕТ");
      p=snd_par;
      gotoxy(100,90);vputs("ПОРТ ");
      if(*p) V_printf("%X",snd_port); else vputs("НЕ НУЖЕН");
      p+=*p+1;
      gotoxy(100,100);vputs("DMA ");
      if(*p) V_printf("%u",snd_dma); else vputs("НЕ НУЖНО");
      p+=*p+1;
      gotoxy(100,110);vputs("IRQ ");
      if(*p) V_printf("%u",snd_irq); else vputs("НЕ НУЖНО");
      gotoxy(100,120);V_printf("ЧАСТОТА %u ГЦ",sfreq);
      gotoxy(100,130);vputs((snd_card)?"ПРОВЕРКА":"УСТАНОВКА");
      V_cursor(100-10,80+cur[m]*10);
      break;
    case TESTSND:
      V_prhdr(90,"ПРОВЕРКА ЗВУКА");
      break;
    case TESTYN:
      V_prhdr(35,"НЕТ ЗВУКОВОЙ КАРТЫ");
      gotoxy(90,60);vputs("ПРОДОЛЖИТЬ ПРОВЕРКУ ?");
      gotoxy(10,80);vputs("(ЭТО МОЖЕТ ПРИВЕСТИ К ЗАВИСАНИЮ КОМПЬЮТЕРА)");
      gotoxy(150,100);vputs("НЕТ");
      gotoxy(150,110);vputs("ДА");
      V_cursor(150-10,100+cur[m]*10);
      break;
    case SNDYN:
      gotoxy(100,60);vputs("ВЫ СЛЫШАЛИ ЗВУК ?");
      gotoxy(150,100);vputs("ДА");
      gotoxy(150,110);vputs("НЕТ");
      V_cursor(150-10,100+cur[m]*10);
      break;
    case SCARD:
      V_prhdr(35,"ВЫБЕРИ ЗВУКОВУЮ КАРТУ");
      for(i=0;i<SDRV__END;++i) {
        set_snd_drv(i);
        gotoxy(100,80+i*10);
        vputs((i==0)?"БЕЗ ЗВУКА":strupr(snd_name));
      }
      V_cursor(100-10,80+cur[m]*10);
      set_snd_drv(snd_card);
      break;
    case SPORT:
      V_prhdr(35,"ВЫБЕРИ ПОРТ");
      p=snd_par+1;
      for(i=0;i<total[m];++i,++p) {
        gotoxy(130,80+i*10);
        V_printf("ПОРТ %X",*p);
      }
      V_cursor(130-10,80+cur[m]*10);
      break;
    case SDMA:
      V_prhdr(35,"ВЫБЕРИ DMA");
      p=snd_par;p+=*p+2;
      for(i=0;i<total[m];++i,++p) {
        gotoxy(140,80+i*10);
        V_printf("DMA %u",*p);
      }
      V_cursor(140-10,80+cur[m]*10);
      break;
    case SIRQ:
      V_prhdr(35,"ВЫБЕРИ IRQ");
      p=snd_par;p+=*p+1;p+=*p+2;
      for(i=0;i<total[m];++i,++p) {
        gotoxy(140,80+i*10);
        V_printf("IRQ %u",*p);
      }
      V_cursor(140-10,80+cur[m]*10);
      break;
    case SFREQ:
      V_prhdr(35,"ВЫБЕРИ КАЧЕСТВО ЗВУКА");
      prmenu(130,80,cur[m],
        "5000 ГЦ","6000 ГЦ","7000 ГЦ","8000 ГЦ","9000 ГЦ",
        "10000 ГЦ","11000 ГЦ","16000 ГЦ","22000 ГЦ","44000 ГЦ",NULL);
      break;
    case RKPL:
      V_prhdr(35,"ВЫБОР КЛАВИШ");
      prmenu(100,80,cur[m],"ПЕРВЫЙ ИГРОК","ВТОРОЙ ИГРОК",NULL);
      break;
    case RKEYS:
      V_prhdr(35,"ВЫБЕРИ КЛАВИШИ");
      for(i=0;i<9;++i) {
        gotoxy(90,80+i*10);vputs(kn[i]);
        gotoxy(180,80+i*10);vputs(keyname(plk[cur[RKPL]][i]));
      }
      V_cursor(90-10,80+cur[m]*10);
      break;
    case IDIR:
      V_prhdr(35,"КУДА УСТАНАВЛИВАТЬ");
      gotoxy(20,100);vputs(ddir);V_cursor(vf_x+8,vf_y);
      break;
    case INST:
      V_prhdr(35,"ИДЕТ УСТАНОВКА");
      gotoxy(90,70);vputs("F10 - ОТМЕНА УСТАНОВКИ");
      break;
    case IDONE:
      V_prhdr(90,"УСТАНОВКА ЗАВЕРШЕНА");
      break;
  }
}

//----------------------------------------------------------------------//

void main_loop(void) {
    fmov_act();
    flame_act();
      V_setscr(scrbuf);
      drawmenu(mnu);
        V_setscr(newscr+8);
        memset(newscr+8,0,64000);
        flame_draw();
        V_setscr(scrbuf);
        V_spr(0,0,(vgaimg*)newscr);
      V_setscr(NULL);
      memcpy(scra,scrbuf,64000);
      redraw=0;
}

void setmnu(int m) {
  int t;

  if(m==IDIR && mnu==SNDYN) {
    T_done();S_init();
    if(snd_type!=ST_NONE) if(snd_card!=3 && snd_card!=4)
      if(load_dmm("install.dmm")) S_startmusic();
    if(snd_type!=ST_NONE) S_play(ipath_snd,7,1024,255);
  }
  vputs_flm=1;
  V_setscr(scrbuf);drawmenu(mnu);drawmenu(m);
  vputs_flm=0;
  for(t=0;t<6;++t) {
    timer=0;
    main_loop();
    while(timer<0xFFFF);
  }
  mnu=m;
  V_setscr(NULL);
}

static int ed_k=0;

void menu_kp(short k) {
  short *p;
  static unsigned short ftab[10]={
    5000,6000,7000,8000,9000,10000,11000,16000,22000,44000
  };
  static unsigned char kch[128]={
     0 , 0 ,'1','2','3','4','5','6','7','8','9','0','-','=', 0 , 0 ,
    'Q','W','E','R','T','Y','U','I','O','P','[',']', 0 , 0 ,'A','S',
    'D','F','G','H','J','K','L',';','\'','`', 0 ,'\\','Z','X','C','V',
    'B','N','M',',','.','/', 0 ,'*', 0 ,' ', 0 , 0 , 0 , 0 , 0 , 0 ,
     0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,'-', 0 , 0 , 0 ,'+', 0 ,
     0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
     0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
     0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
  };
  static unsigned char kch_sh[128]={
     0 , 0 ,'!','@','#','$','%','^','&','*','(',')','_','+', 0 , 0 ,
    'Q','W','E','R','T','Y','U','I','O','P','{','}', 0 , 0 ,'A','S',
    'D','F','G','H','J','K','L',':','\"','~', 0 ,'|','Z','X','C','V',
    'B','N','M','<','>','?', 0 ,'*', 0 ,' ', 0 , 0 , 0 , 0 , 0 , 0 ,
     0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,'-', 0 , 0 , 0 ,'+', 0 ,
     0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
     0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
     0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
  };

  if(mnu==IDIR) {
    switch(k) {
      case 1:
        swmnu=SOUND;
        S_stopmusic();free_music();
        S_done();T_init();
        break;
      case 0x0E: ed_k=-1;break;
      case 0x1C: case 0x9C: swmnu=INST;break;
      default:
        if(keys[0x2A] || keys[0x36]) ed_k=kch_sh[k&0x7F];
        else ed_k=kch[k&0x7F];
        break;
    }return;
  }
  switch(k) {
    case 0x44:
      if(mnu==INST) need_exit=1;
      break;
    case 1:
      switch(mnu) {
        case MAIN: need_exit=1;break;
        case SOUND: need_exit=1;break;
        case IDIR: swmnu=SOUND;S_done();T_init();break;
        case SCARD: swmnu=SOUND;break;
        case SPORT: swmnu=SOUND;break;
        case SDMA: swmnu=SOUND;break;
        case SIRQ: swmnu=SOUND;break;
        case SFREQ: swmnu=SOUND;break;
        case RKPL: swmnu=MAIN;break;
        case RKEYS: swmnu=RKPL;break;
      }break;
    case 0x48: case 0xC8:
      if(--cur[mnu]<0) cur[mnu]=total[mnu]-1;
      redraw=1;break;
    case 0x50: case 0xD0: case 0x4C:
      if(++cur[mnu]>=total[mnu]) cur[mnu]=0;
      redraw=1;break;
    case 0x1C: case 0x39: case 0x9C:
      switch(mnu) {
        case MAIN:
          switch(cur[mnu]) {
            case 0: swmnu=SOUND;break;
            case 1: swmnu=RKPL;break;
            case 2: need_exit=1;break;
          }break;
        case SOUND:
          switch(cur[mnu]) {
            case 0: swmnu=SCARD;break;
            case 1: if(total[SPORT]>1) swmnu=SPORT; break;
            case 2: if(total[SDMA]>1) swmnu=SDMA; break;
            case 3: if(total[SIRQ]>1) swmnu=SIRQ; break;
            case 4: swmnu=SFREQ;break;
            case 5:
              if(!S_detect()) {swmnu=TESTYN;cur[TESTYN]=0;break;}
              swmnu=TESTSND;break;
          }break;
        case TESTYN:
          swmnu=(cur[mnu])?TESTSND:SOUND;break;
        case SNDYN:
          swmnu=SOUND;break;
        case SCARD:
          set_snd_drv(snd_card=cur[mnu]);
          p=snd_par;
          if(total[SPORT]=*p) snd_port=p[1]; p+=*p+1;
          if(total[SDMA]=*p) snd_dma=p[1]; p+=*p+1;
          if(total[SIRQ]=*p) snd_irq=p[1];
          cur[SPORT]=cur[SDMA]=cur[SIRQ]=0;
          swmnu=SOUND;break;
        case SPORT:
          p=snd_par;
          snd_port=p[cur[mnu]+1];
          swmnu=SOUND;break;
        case SDMA:
          p=snd_par;p+=*p+1;
          snd_dma=p[cur[mnu]+1];
          swmnu=SOUND;break;
        case SIRQ:
          p=snd_par;p+=*p+1;p+=*p+1;
          snd_irq=p[cur[mnu]+1];
          swmnu=SOUND;break;
        case SFREQ:
          sfreq=ftab[cur[mnu]];
          swmnu=SOUND;break;
        case RKPL: swmnu=RKEYS;break;
        case RKEYS:
          plk[cur[RKPL]][cur[RKEYS]]=0;def_k=1;
          V_setscr(scrbuf);drawmenu(mnu);V_setscr(NULL);
          memcpy(scra,scrbuf,64000);
          break;
        case IDIR: need_exit=1;break;
      }break;
  }
}

volatile unsigned char defk_key;

void defk_kp(short k) {
  defk_key=k;
}

int i_progr=0,i_file=0;
FILE *ifh=NULL,*iofh=NULL;

int IP_TOTAL=5000000;

enum{I_GO,I_OK};

#define IFILES 11

static char *i_fname[IFILES]={
  "START.DAT","SETUP.DAT","INSTALL.DAT","README.DOC","DOS4GW.EXE",
  "EDITOR.DAT","EDITOR.CFG",
  "MEGADM.WAD","MEGADM.BAT",
  "SUPERDM.WAD","SUPERDM.BAT"
};
static char *i_ofname[IFILES]={
  "START.EXE","SETUP.EXE","CMRTKA.WAD","README.DOC","DOS4GW.EXE",
  "EDITOR.EXE","EDITOR.CFG",
  "MEGADM.WAD","MEGADM.BAT",
  "SUPERDM.WAD","SUPERDM.BAT"
};

int fsize(char *fn) {
  FILE *h;
  int sz;

  if(!(h=fopen(fn,"rb"))) return 0;
  fseek(h,0,SEEK_END);
  sz=ftell(h);
  fclose(h);
  return sz;
}

void inst_reset(void) {
  int i,n;

  i_progr=0;i_file=0;
  ifh=iofh=NULL;
  for(n=0,IP_TOTAL=1;n<IFILES;++n) {
    if((i=fsize(i_fname[n]))<=0) {
      V_done();K_done();S_done();
      printf("\nНе могу открыть файл %s\n",i_fname[n]);
      exit(1);
    }
    IP_TOTAL+=i;
  }
}

#define BSZ 5000

int install(void) {
  int s;
  static char buf[BSZ];

  if(ifh) {
    s=fread(buf,1,BSZ,ifh);
    if(s<=0) {
      fclose(ifh);fclose(iofh);
      ifh=iofh=NULL;
      ++i_file;
      return I_GO;
    }
    fwrite(buf,s,1,iofh);
    i_progr+=s;
  }else{
    if(i_file>=IFILES) return I_OK;
    if(!(ifh=fopen(i_fname[i_file],"rb"))) {
      V_done();K_done();S_done();
      printf("\nНе могу открыть файл %s\n",i_fname[i_file]);
      exit(1);
    }
    sprintf(buf,"%s\\%s",ddir,i_ofname[i_file]);
    if(!(iofh=fopen(buf,"wb"))) {
      V_done();K_done();S_done();
      printf("\nНе могу создать файл %s\n",buf);
      exit(1);
    }
  }
  return I_GO;
}

void load_res(void) {
  int i;
  static char s[40];

  for(i=0;i<TSNDS;++i) {
    sprintf(s,"TEST%02d.DMI",i+1);
    if(!(testsnd[i]=load_dmi(s)))
      {printf("Не могу открыть файл %s\n",s);exit(1);}
  }
  if(!(ipath_snd=load_dmi("INSTPATH.DMI")))
    {puts("Не могу открыть файл INSTPATH.DMI");exit(1);}
  if(!(igo_snd=load_dmi("INSTGO.DMI")))
    {puts("Не могу открыть файл INSTGO.DMI");exit(1);}
  if(!(iend_snd=load_dmi("INSTEND.DMI")))
    {puts("Не могу открыть файл INSTEND.DMI");exit(1);}
}

int harderr_handler(int f,int d,int e) {
  if(!keys[0x44]) return HARDERR_RETRY;
  close_all();return HARDERR_ABORT;
}

void chk_blaster(void) {
  char *p,c;
  unsigned v;

  if(!(p=getenv("BLASTER"))) return;
  snd_card=5;snd_port=0x220;snd_irq=7;snd_dma=1;
  swmnu=SOUND;
  for(;*p;) {
    if((*p>='A' && *p<='Z') || (*p>='a' && *p<='z')) {
      c=*p;if(c>='a' && c<='z') c=c-'a'+'A';
      v=strtoul(p+1,&p,(c=='I' || c=='D')?10:16);
      switch(c) {
        case 'A': snd_port=v;break;
        case 'I': snd_irq=v;break;
        case 'D': snd_dma=v;break;
      }
    }else ++p;
  }
}

void build_dir(char *d) {
  static char s[256],dir[256];
  char *p;

  strcpy(s,d);
  dir[0]=0;if(s[0]=='\\') strcpy(dir,"\\");
  for(p=strtok(s,"\\");p;p=strtok(NULL,"\\")) {
    strcat(dir,p);
    if(strchr(p,':')) {strcat(dir,"\\");continue;}
    mkdir(dir);
    strcat(dir,"\\");
  }
}

int main(int ac,char *av[]) {
  short *p;
  int i,j;
  static int cnt=0;

  sfreq=11000;
  snd_vol=128;
  mus_vol=128;
  for(i=0;i<256;++i)
    bright[i]=VP_brightness(doompal[i].r,doompal[i].g,doompal[i].b)/2;
  load_res();
  if(ac>=2) {
    if(stricmp(av[1],"-fast")==0) {
      _fast=1;
      if(ac>=3) strcpy(ddir,av[2]);
    }else{
      strcpy(ddir,av[1]);
      if(ac>=3) if(stricmp(av[2],"-fast")==0) _fast=1;
    }
  }
  if(ac>=1) {
    strcpy(sdir,av[0]);
    for(i=strlen(sdir)-1;i>=0;--i) {
      if(sdir[i]=='\\' || sdir[i]==':') break;
      sdir[i]=0;
    }
  }
  strupr(ddir);
  K_init();
  harderr_inst(harderr_handler);
  ((short*)newscr)[0]=320;
  ((short*)newscr)[1]=200;
  memset(newscr+4,0,4);
  memset(cur,0,sizeof(cur));
  chk_blaster();
  set_snd_drv(snd_card);
  p=snd_par;
  p+=(total[SPORT]=*p)+1;
  p+=(total[SDMA]=*p)+1;
  total[SIRQ]=*p;
  V_init();VP_setall(doompal);
  vf_font=(void*)&doom_sfnt;vf_color=0;vf_step=-1;
  K_setkeyproc(menu_kp);T_init();
  init_flame();
  init_fmov();
  while(!need_exit) {
    timer=0;
    if(ed_k) {
      i=strlen(ddir);
      if(ed_k<0) {
        if(i) ddir[i-1]=0;
      }else{
        if(i<40) {ddir[i]=ed_k;ddir[i+1]=0;}
      }
      ed_k=0;
    }
    main_loop();
    if(def_k) {
      plk[cur[RKPL]][cur[RKEYS]]=0;
      defk_key=0;K_setkeyproc(defk_kp);
      while(!defk_key);
      K_setkeyproc(menu_kp);
      plk[cur[RKPL]][cur[RKEYS]]=defk_key;
      def_k=0;redraw=1;
    }
    if(mnu==INST) switch(install()) {
      case I_GO:
        j=240*i_progr/IP_TOTAL+40;
        if(!_fast) {
          for(i=40;i<=j;i+=5)
            new_flame(i<<3,140*8,rand()%17-8,-rand()%40-10);
        }else V_clr(40,j-39,140,5,0xD0);
        break;
      case I_OK:
        swmnu=IDONE;cnt=40;
        if(snd_type!=ST_NONE) S_play(iend_snd,7,1024,255);
        break;
    }
    if(mnu==IDONE) if(--cnt<=0) need_exit=1;
    if(swmnu) {
      if(mnu==SNDYN && cur[SNDYN]==0) swmnu=IDIR;
      if(swmnu==TESTSND && snd_card==0) swmnu=IDIR;
      if(swmnu==INST) {
        if(!ddir[0]) swmnu=IDIR; else {
          if(ddir[i=strlen(ddir)-1]=='\\') ddir[i]=0;
          build_dir(ddir);
          inst_reset();
          if(snd_type!=ST_NONE) S_play(igo_snd,7,1024,255);
        }
      }
      setmnu(swmnu);
      if(swmnu==TESTSND) {
        T_done();S_init();
        if(snd_type!=ST_NONE) S_play(testsnd[rand()%TSNDS],8,1024,255);
        for(i=0;i<27;++i) {
          timer=0;
          main_loop();
          while(timer<0xFFFF);
        }
        S_done();T_init();swmnu=SNDYN;cur[SNDYN]=1;
      }else swmnu=0;
    }
    if(mnu!=INST) while(timer<0xFFFF);
  }
  V_done();K_done();S_done();
  if(mnu==IDONE) {
    sprintf(cfile,"%s\\DEFAULT.CFG",ddir);
    save_cfg();
    if(ddir[1]==':') _dos_setdrive(ddir[0]-'A'+1,(unsigned*)&j);
    chdir(ddir);
    if(spawnl(P_WAIT,"START.EXE","START.EXE",NULL)==-1)
      puts("\nЗапустите START.EXE, чтобы начать игру.");
  }
  return 0;
}
