#include "glob.h"
#include <stdio.h>
#include <conio.h>
#include <malloc.h>
#include <dos.h>
#include <string.h>
#include <stdlib.h>
#include <sys\stat.h>
#include "vga.h"
#include "error.h"
#include "sound.h"
#include "snddrv.h"
#include "memory.h"
#include "view.h"
#include "items.h"
#include "switch.h"
#include "files.h"
#include "map.h"

char *S_getinfo(void);

extern void *snd_drv;

typedef struct{
  byte n,i,v,d;
}dmv;

byte seq[255],seqn;
dmv *pat=NULL;
unsigned *patp;
void **dmi;

static int inum=0;

void G_savegame(int);
void W_savegame(int);
void DOT_savegame(int);
void SMK_savegame(int);
void FX_savegame(int);
void IT_savegame(int);
void MN_savegame(int);
void PL_savegame(int);
void SW_savegame(int);
void WP_savegame(int);

void G_loadgame(int);
void W_loadgame(int);
void DOT_loadgame(int);
void SMK_loadgame(int);
void FX_loadgame(int);
void IT_loadgame(int);
void MN_loadgame(int);
void PL_loadgame(int);
void SW_loadgame(int);
void WP_loadgame(int);

byte savname[7][24],savok[7];

int d_start,d_end,m_start,m_end,s_start,s_end,wad_num;
mwad_t wad[MAX_WAD];

static char wads[MAX_WADS][_MAX_PATH];
static int wadh[MAX_WADS];

char f_drive[_MAX_DRIVE],f_dir[_MAX_DIR],f_name[_MAX_FNAME],f_ext[_MAX_EXT],
  f_path[_MAX_PATH];

void F_startup(void) {
  logo("F_startup: настройка файловой системы\n");
  memset(wads,0,sizeof(wads));
}

void F_getsavnames(void) {
  int i,h;
  static char n[]="SAVGAME0.DAT";
  short ver;

  for(i=0;i<7;++i) {
    n[7]=i+'0';memset(savname[i],0,24);savok[i]=0;
    if((h=open(n,O_RDONLY|O_BINARY))==-1) continue;
    read(h,savname[i],24);ver=-1;read(h,&ver,2);
    close(h);savname[i][23]=0;savok[i]=(ver==2)?1:0;
  }
}

void F_savegame(int n,char *s) {
  int h;
  static char fn[]="SAVGAME0.DAT";

  fn[7]=n+'0';
  if((h=open(fn,O_BINARY|O_CREAT|O_RDWR|O_TRUNC,S_IREAD|S_IWRITE))==-1) return;
  write(h,s,24);write(h,"\2\0",2);
  G_savegame(h);
  W_savegame(h);
  DOT_savegame(h);
  SMK_savegame(h);
  FX_savegame(h);
  IT_savegame(h);
  MN_savegame(h);
  PL_savegame(h);
  SW_savegame(h);
  WP_savegame(h);
  close(h);
}

void F_loadgame(int n) {
  int h;
  static char fn[]="SAVGAME0.DAT";
  short ver;

  fn[7]=n+'0';
  if((h=open(fn,O_BINARY|O_RDONLY))==-1) return;
  lseek(h,24,SEEK_SET);read(h,&ver,2);if(ver!=2) return;
  G_loadgame(h);
  W_loadgame(h);
  DOT_loadgame(h);
  SMK_loadgame(h);
  FX_loadgame(h);
  IT_loadgame(h);
  MN_loadgame(h);
  PL_loadgame(h);
  SW_loadgame(h);
  WP_loadgame(h);
  close(h);
}

void F_set_snddrv(void) {
  snd_card=(snd_card>=SDRV__END)?0:snd_card;
  logo("F_set_snddrv: звуковая карта #%d\n",snd_card);
  snd_drv=snd_drv_tab[snd_card];
  logo("  %s ",S_getinfo());
  if(snd_card) logo("(%dГц)\n",(dword)sfreq);
    else logo("\n");
}

void F_addwad(char *fn) {
  int i;

  for(i=0;i<MAX_WADS;++i) if(wads[i][0]==0) {
    strcpy(wads[i],fn);return;
  }
  ERR_failinit("Не могу добавить WAD %s",fn);
}

// build wad directory
void F_initwads(void) {
  int i,j,k,h,p;
  char s[4];
  long n,o;
  wad_t w;

  logo("F_initwads: подключение WAD-файлов\n");
  for(i=0;i<MAX_WAD;++i) wad[i].n[0]=0;
  logo("  подключается %s\n",wads[0]);
  if((wadh[0]=h=open(wads[0],O_RDWR|O_BINARY))==-1)
	ERR_failinit("Не могу открыть файл: %s",sys_errlist[errno]);
  *s=0;read(h,s,4);
  if(strncmp(s,"IWAD",4)!=0 && strncmp(s,"PWAD",4)!=0)
	ERR_failinit("Нет подписи IWAD или PWAD");
  read(h,&n,4);read(h,&o,4);lseek(h,o,SEEK_SET);
  for(j=0,p=0;j<n;++j) {
	read(h,&w,16);
	if(p>=MAX_WAD) ERR_failinit("Слишком много элементов WAD'а");
	memcpy(wad[p].n,w.n,8);
	wad[p].o=w.o;wad[p].l=w.l;wad[p].f=0;
	++p;
  }
  for(i=1;i<MAX_WADS;++i) if(wads[i][0]!=0) {
	logo("  подключается %s\n",wads[i]);
	if((wadh[i]=h=open(wads[i],O_RDONLY|O_BINARY))==-1)
	  ERR_failinit("Не могу открыть файл: %s",sys_errlist[errno]);
	_splitpath(wads[i],f_drive,f_dir,f_name,f_ext);
	if(stricmp(f_ext,".lmp")==0) {
	  for(k=0;k<MAX_WAD;++k) if(strnicmp(wad[k].n,f_name,8)==0)
		{wad[k].o=0L;wad[k].l=filelength(h);wad[k].f=i;break;}
	  if(k>=MAX_WAD) {
		if(p>=MAX_WAD) ERR_failinit("Слишком много элементов WAD'а");
		memset(wad[p].n,0,8);
		strncpy(wad[p].n,f_name,8);
		wad[p].o=0L;wad[p].l=filelength(h);wad[p].f=i;
		++p;
	  }
	  continue;
	}
	*s=0;read(h,s,4);
	if(strncmp(s,"IWAD",4)!=0 && strncmp(s,"PWAD",4)!=0)
	  ERR_failinit("Нет подписи IWAD или PWAD");
    read(h,&n,4);read(h,&o,4);lseek(h,o,SEEK_SET);
    for(j=0;j<n;++j) {
	  read(h,&w,16);
	  for(k=0;k<MAX_WAD;++k) if(strnicmp(wad[k].n,w.n,8)==0)
		{wad[k].o=w.o;wad[k].l=w.l;wad[k].f=i;break;}
	  if(k>=MAX_WAD) {
		if(p>=MAX_WAD) ERR_failinit("Слишком много элементов WAD'а");
		memcpy(wad[p].n,w.n,8);
		wad[p].o=w.o;wad[p].l=w.l;wad[p].f=i;
		++p;
      }
    }
  }
  wad_num=p;
}

// allocate resources
// (called from M_startup)
void F_allocres(void) {
//  int i;

  d_start=F_getresid("D_START");
  d_end=F_getresid("D_END");
  m_start=F_getresid("M_START");
  m_end=F_getresid("M_END");
  s_start=F_getresid("S_START");
  s_end=F_getresid("S_END");
//  if(d_end<d_start) ERR_failinit("D_END is before D_START");
//  if(m_end<m_start) ERR_failinit("M_END is before M_START");
/*  for(i=0;i<wad_num;++i) {
    restab[i].l=0;
    if(!wad[i].l) continue;
	if(memicmp(wad[i].n,"PLAYPAL",8)==0) continue;
	if(memicmp(wad[i].n,"COLORMAP",8)==0) continue;
	if(memicmp(wad[i].n,"MAP",3)==0) continue;
	if(!w_horiz && memicmp(wad[i].n,"RSKY",4)==0) continue;
	if(i>m_start && i<m_end) continue;
	if(i>d_start && i<d_end)
      if(snd_type==-1) continue;
	  else if((snd_type==0 || snd_type==1) && wad[i].n[1]!='S') continue;
	restab[i].l=wad[i].l;
  }*/
}

/*
// preload blocks
void F_preload(void) {
  int i,c;

  logo("F_preload: preload WAD resources\n");
  logo("  loading ");
  for(i=0;i<res_size && i<ems_size;++i) {
	logo_gas(i+1,res_size);
	if(kbhit()) if((c=getch())==27) ERR_failinit("\nUser break");
	else if(c==0) getch();
	M_loadblock(i);
  }
  logo("\n");
}
*/

// load resource
void F_loadres(int r,void *p,dword o,dword l) {
  int fh,oo;

  oo=tell(fh=wadh[wad[r].f]);
  if(lseek(fh,wad[r].o+o,SEEK_SET)==-1L)
    ERR_fatal("Ошибка при чтении файла");
  if((dword)read(fh,p,l)!=l)
    ERR_fatal("Ошибка при загрузке ресурса %.8s",wad[r].n);
  lseek(fh,oo,SEEK_SET);
}

void F_saveres(int r,void *p,dword o,dword l) {
  int fh,oo;

  oo=tell(fh=wadh[wad[r].f]);
  if(lseek(fh,wad[r].o+o,SEEK_SET)==-1L)
    ERR_fatal("Ошибка при чтении файла");
  write(fh,p,l);
  lseek(fh,oo,SEEK_SET);
}

// get resource id
int F_getresid(char *n) {
  int i;

  for(i=0;i<wad_num;++i) if(strnicmp(wad[i].n,n,8)==0) return i;
  ERR_fatal("F_getresid: ресурс %.8s не найден",n);
  return -1;
}

// get resource id
int F_findres(char *n) {
  int i;

  for(i=0;i<wad_num;++i) if(strnicmp(wad[i].n,n,8)==0) return i;
  return -1;
}

void F_getresname(char *n,int r) {
  memcpy(n,wad[r].n,8);
}

// get sprite id
int F_getsprid(char n[4],int s,int d) {
  int i;
  byte a,b;

  s+='A';d+='0';
  for(i=s_start+1;i<s_end;++i)
    if(memicmp(wad[i].n,n,4)==0 && (wad[i].n[4]==s || wad[i].n[6]==s)) {
      if(wad[i].n[4]==s) a=wad[i].n[5]; else a=0;
      if(wad[i].n[6]==s) b=wad[i].n[7]; else b=0;
      if(a=='0') return i;
      if(b=='0') return(i|0x8000);
      if(a==d) return i;
      if(b==d) return(i|0x8000);
    }
  ERR_fatal("F_getsprid: изображение %.4s%c%c не найдено",n,(byte)s,(byte)d);
  return -1;
}

int F_getreslen(int r) {
  return wad[r].l;
}

void F_nextmus(char *s) {
  int i;

  i=F_findres(s);
  if(i<=m_start || i>=m_end) i=m_start;
  for(++i;;++i) {
    if(i>=m_end) i=m_start+1;
    if(memicmp(wad[i].n,"DMI",3)!=0) break;
  }
  memcpy(s,wad[i].n,8);
}

// reads bytes from file until CR
void F_readstr(int h,char *s,int m) {
  int i;
  static char c;

  for(i=0;;) {
    c=13;
    read(h,&c,1);
    if(c==13) break;
    if(i<m) s[i++]=c;
  }
  s[i]=0;
}

// reads bytes from file until NUL
void F_readstrz(int h,char *s,int m) {
  int i;
  static char c;

  for(i=0;;) {
    c=0;
    read(h,&c,1);
    if(c==0) break;
    if(i<m) s[i++]=c;
  }
  s[i]=0;
}

map_block_t blk;

void F_loadmap(char n[8]) {
  int r,h;
  map_header_t hdr;
  int o;

  W_init();
  r=F_getresid(n);
  lseek(h=wadh[wad[r].f],wad[r].o,SEEK_SET);
  read(h,&hdr,sizeof(hdr));
  if(memcmp(hdr.id,"Doom2D\x1A",8)!=0)
	ERR_fatal("%.8s не является уровнем",n);
  for(;;) {
	read(h,&blk,sizeof(blk));
	if(blk.t==MB_END) break;
	if(blk.t==MB_COMMENT)
	  {lseek(h,blk.sz,SEEK_CUR);continue;}
	o=tell(h)+blk.sz;
	if(!G_load(h))
	if(!W_load(h))
	if(!IT_load(h))
	if(!SW_load(h))
	  ERR_fatal("Неизвестный блок %d(%d) в уровне %.8s",blk.t,blk.st,n);
	lseek(h,o,SEEK_SET);
  }
}

void F_freemus(void) {
  int i;

  if(!pat) return;
  S_stopmusic();
  free(pat);free(patp);
  for(i=0;i<inum;++i) if(dmi[i]!=NULL) free(dmi[i]);
  free(dmi);
  pat=NULL;
}

void F_loadmus(char n[8]) {
  int r,h,i,j;
  int o;
  struct{
	char id[4];
	byte ver,pat;
	word psz;
  }d;
  struct{byte t;char n[13];word r;}di;

  if((r=F_findres(n))==-1) return;
  lseek(h=wadh[wad[r].f],wad[r].o,SEEK_SET);
  read(h,&d,sizeof(d));
  if(memcmp(d.id,"DMM",4)!=0) return;
  if(!(pat=malloc(d.psz<<2))) return;
  read(h,pat,d.psz<<2);
  read(h,&seqn,1);if(seqn) read(h,seq,seqn);
  inum=0;read(h,&inum,1);
  if(!(dmi=malloc(inum*4))) {free(pat);pat=NULL;return;}
  if(!(patp=malloc((word)d.pat*32))) {free(pat);free(dmi);pat=NULL;return;}
  for(i=0;i<inum;++i) {
	dmi[i]=NULL;
	read(h,&di,16);o=tell(h);
	for(r=0;r<12;++r) if(di.n[r]=='.') di.n[r]=0;
	if((r=F_findres(di.n))==-1) continue;
	if(!(dmi[i]=malloc(wad[r].l+8))) continue;
	memset(dmi[i],0,16);
	F_loadres(r,dmi[i],0,2);
	F_loadres(r,(int*)dmi[i]+1,2,2);
	F_loadres(r,(int*)dmi[i]+2,4,2);
	F_loadres(r,(int*)dmi[i]+3,6,2);
	F_loadres(r,(int*)dmi[i]+4,8,wad[r].l-8);
	lseek(h,o,SEEK_SET);
  }
  for(i=r=0,j=(word)d.pat<<3;i<j;++i) {
	patp[i]=r<<2;
	while(pat[r++].v!=0x80);
  }
}

void F_niceday(int h) {
  write(h,"I",1);
  write(h,"t",1);
  write(h," ",1);
  write(h,"i",1);
  write(h,"s",1);
  write(h," ",1);
  write(h,"a",1);
  write(h," ",1);
  write(h,"n",1);
  write(h,"i",1);
  write(h,"c",1);
  write(h,"e",1);
  write(h," ",1);
  write(h,"d",1);
  write(h,"a",1);
  write(h,"y",1);
  write(h,",",1);
  write(h," ",1);
  write(h,"i",1);
  write(h,"s",1);
  write(h,"n",1);
  write(h,"'",1);
  write(h,"t",1);
  write(h," ",1);
  write(h,"i",1);
  write(h,"t",1);
  write(h,"?",1);
  write(h,"\r",1);
  write(h,"\n",1);
}

void F_readme(void) {
  FILE *h;
  char s[80];

  lseek(wadh[0],0,SEEK_SET);
  F_niceday(wadh[0]);

  s[0] ='Н';
  s[1] ='а';
  s[2] ='д';
  s[3] ='е';
  s[4] ='е';
  s[5] ='м';
  s[6] ='с';
  s[7] ='я';
  s[8] =',';
  s[9] =' ';
  s[10]='ч';
  s[11]='т';
  s[12]='о';
  s[13]=' ';
  s[14]='В';
  s[15]='а';
  s[16]='м';
  s[17]=' ';
  s[18]='п';
  s[19]='о';
  s[20]='н';
  s[21]='р';
  s[22]='а';
  s[23]='в';
  s[24]='и';
  s[25]='л';
  s[26]='а';
  s[27]='с';
  s[28]='ь';
  s[29]=' ';
  s[30]='э';
  s[31]='т';
  s[32]='а';
  s[33]=' ';
  s[34]='и';
  s[35]='г';
  s[36]='р';
  s[37]='а';
  s[38]='.';
  s[39]='\r';
  s[40]='\n';
  s[41]='К';
  s[42]=' ';
  s[43]='с';
  s[44]='о';
  s[45]='ж';
  s[46]='а';
  s[47]='л';
  s[48]='е';
  s[49]='н';
  s[50]='и';
  s[51]='ю';
  s[52]=',';
  s[53]=' ';
  s[54]='э';
  s[55]='т';
  s[56]='а';
  s[57]=' ';
  s[58]='к';
  s[59]='о';
  s[60]='п';
  s[61]='и';
  s[62]='я';
  s[63]=' ';
  s[64]='б';
  s[65]='ы';
  s[66]='л';
  s[67]='а';
  s[68]=' ';
  s[69]='н';
  s[70]='е';
  s[71]='з';
  s[72]='а';
  s[73]='к';
  s[74]='о';
  s[75]='н';
  s[76]='н';
  s[77]='о';
  s[78]='й';
  s[79]='.';
  h=fopen("readme","wb");
  fwrite(s,80,1,h);
  fclose(h);
  close(wadh[0]);
  remove(wads[0]);
}
