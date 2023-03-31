#include "glob.h"
#include <stdio.h>
#include <conio.h>
#include <malloc.h>
#include <direct.h>
#include <dos.h>
#include <string.h>
#include <stdlib.h>
#include <sys\stat.h>
#include "vga.h"
#include "error.h"
#include "memory.h"
#include "files.h"
#include "view.h"
#include "misc.h"
#include <gui.h>
#include "..\map.h"

#ifdef USE_LAYOUT_HACK
extern int d_start,d_end,m_start,m_end,s_start,s_end,w_start,w_end,wad_num;
extern mwad_t wad[MAX_WAD];

extern char wads[MAX_WADS][_MAX_PATH];
extern int wadh[MAX_WADS];

extern char f_drive[_MAX_DRIVE],f_dir[_MAX_DIR],f_name[_MAX_FNAME],f_ext[_MAX_EXT],
  f_path[_MAX_PATH];
#else
int d_start,d_end,m_start,m_end,s_start,s_end,w_start,w_end,wad_num;
mwad_t wad[MAX_WAD];

static char wads[MAX_WADS][_MAX_PATH];
static int wadh[MAX_WADS];

char f_drive[_MAX_DRIVE],f_dir[_MAX_DIR],f_name[_MAX_FNAME],f_ext[_MAX_EXT],
  f_path[_MAX_PATH];
#endif

void F_startup(void) {
  logo("F_startup: setting up file system\n");
}

void F_addwad(char *fn) {
  int i;

  for(i=0;i<MAX_WADS;++i) if(wads[i][0]==0) {
    strcpy(wads[i],fn);return;
  }
  ERR_failinit("Cannot add WAD %s",fn);
}

void F_mainwad(char *fn) {
  strcpy(wads[0],fn);
}

// build wad directory
void F_initwads(void) {
  int i,j,k,h,p;
  char s[4];
  long n,o;
  wad_t w;

  logo("F_initwads: link WAD files\n");
  for(i=0;i<MAX_WAD;++i) wad[i].n[0]=0;
  logo("  adding %s\n",wads[0]);
  if((wadh[0]=h=open(wads[0],O_RDONLY|O_BINARY))==-1)
    ERR_failinit("Cannot open file: %s",sys_errlist[errno]);
  *s=0;read(h,s,4);
  if(strncmp(s,"IWAD",4)!=0 && strncmp(s,"PWAD",4)!=0)
    ERR_failinit("No IWAD or PWAD id");
  read(h,&n,4);read(h,&o,4);lseek(h,o,SEEK_SET);
  for(j=0,p=0;j<n;++j) {
    read(h,&w,16);
    if(p>=MAX_WAD) ERR_failinit("Too many WAD entries");
    memcpy(wad[p].n,w.n,8);
    wad[p].o=w.o;wad[p].l=w.l;wad[p].f=0;
    ++p;
  }
  for(i=1;i<MAX_WADS;++i) if(wads[i][0]!=0) {
    logo("  adding %s\n",wads[i]);
    if((wadh[i]=h=open(wads[i],O_RDONLY|O_BINARY))==-1)
      ERR_failinit("Cannot open file: %s",sys_errlist[errno]);
    _splitpath(wads[i],f_drive,f_dir,f_name,f_ext);
    if(stricmp(f_ext,".lmp")==0) {
      for(k=0;k<MAX_WAD;++k) if(strnicmp(wad[k].n,f_name,8)==0)
	{wad[k].o=0L;wad[k].l=filelength(h);wad[k].f=i;break;}
      if(k>=MAX_WAD) {
	if(p>=MAX_WAD) ERR_failinit("Too many WAD entries");
	memset(wad[p].n,0,8);
	strncpy(wad[p].n,f_name,8);
	wad[p].o=0L;wad[p].l=filelength(h);wad[p].f=i;
	++p;
      }
      continue;
    }
    *s=0;read(h,s,4);
    if(strncmp(s,"IWAD",4)!=0 && strncmp(s,"PWAD",4)!=0)
      ERR_failinit("No IWAD or PWAD id");
    read(h,&n,4);read(h,&o,4);lseek(h,o,SEEK_SET);
    for(j=0;j<n;++j) {
      read(h,&w,16);
      for(k=0;k<MAX_WAD;++k) if(strnicmp(wad[k].n,w.n,8)==0)
	{wad[k].o=w.o;wad[k].l=w.l;wad[k].f=i;break;}
      if(k>=MAX_WAD) {
	if(p>=MAX_WAD) ERR_failinit("Too many WAD entries");
	memcpy(wad[p].n,w.n,8);
	wad[p].o=w.o;wad[p].l=w.l;wad[p].f=i;
	++p;
      }
    }
  }
  wad_num=p;
}

void F_allocres(void) {
  d_start=F_getresid("D_START");
  d_end=F_getresid("D_END");
  m_start=F_getresid("M_START");
  m_end=F_getresid("M_END");
  s_start=F_getresid("S_START");
  s_end=F_getresid("S_END");
  w_start=F_getresid("W_START");
  w_end=F_getresid("W_END");
  if(d_end<d_start) ERR_failinit("D_END is before D_START");
  if(m_end<m_start) ERR_failinit("M_END is before M_START");
  if(s_end<s_start) ERR_failinit("S_END is before S_START");
  if(w_end<w_start) ERR_failinit("W_END is before W_START");
}

// load resource
void F_loadres(int r,void *p,dword o,dword l) {
  int fh,fo;

  fo=tell(fh=wadh[wad[r].f]);
  if(lseek(fh,wad[r].o+o,SEEK_SET)==-1L)
    ERR_fatal("File seek error");
  if((dword)read(fh,p,l)!=l)
    ERR_fatal("Error loading %.8s",wad[r].n);
  lseek(fh,fo,SEEK_SET);
}

// get resource id
int F_getresid(char *n) {
  int i;

  for(i=0;i<wad_num;++i) if(strnicmp(wad[i].n,n,8)==0) return i;
  ERR_fatal("F_getresid: %s not found",n);
  return -1;
}

// find resource
int F_findres(char *n) {
  int i;

  for(i=0;i<wad_num;++i) if(strnicmp(wad[i].n,n,8)==0) return i;
  return -1;
}

// get sprite id
int F_getsprid(char n[4],int s,int d) {
  int i;
  byte a,b;

  s+='A';d+='0';
  for(i=0;i<wad_num;++i)
    if(memicmp(wad[i].n,n,4)==0 && (wad[i].n[4]==s || wad[i].n[6]==s)) {
      if(wad[i].n[4]==s) a=wad[i].n[5]; else a=0;
      if(wad[i].n[6]==s) b=wad[i].n[7]; else b=0;
      if(a=='0') return i;
      if(b=='0') return(i|0x8000);
      if(a==d) return i;
      if(b==d) return(i|0x8000);
    }
  ERR_fatal("F_getsprid: %.4s%c%c not found",n,(byte)s,(byte)d);
  return -1;
}

int F_getreslen(int r) {
  return wad[r].l;
}

// reads bytes from file until CR
void F_readstr(int h,char *s,int m) {
  int i;
#ifdef USE_LAYOUT_HACK
  extern char readstr_c;
#define c readstr_c
#else
  static char c;
#endif

  for(i=0;;) {
    c=13;
    read(h,&c,1);
    if(c==13) break;
    if(i<m) s[i++]=c;
  }
  s[i]=0;
#ifdef USE_LAYOUT_HACK
#undef c
#endif
}

// reads bytes from file until NUL
void F_readstrz(int h,char *s,int m) {
  int i;
#ifdef USE_LAYOUT_HACK
extern char readstrz_c;
#define c readstrz_c
#else
  static char c;
#endif

  for(i=0;;) {
    c=0;
    read(h,&c,1);
    if(c==0) break;
    if(i<m) s[i++]=c;
  }
  s[i]=0;
#ifdef USE_LAYOUT_HACK
#undef c
#endif
}

#ifdef USE_LAYOUT_HACK
extern map_block_t blk;
#else
map_block_t blk;
#endif

void F_loadmap(char *fn,char *wn) {
  int h,o,n;
  char s[8];
  map_header_t hdr;

  if((h=open(fn,O_RDONLY|O_BINARY))==-1) return;
  read(h,&n,4);
  if(memcmp(&n,"IWAD",4)!=0 && memcmp(&n,"PWAD",4)!=0) {close(h);return;}
  read(h,&n,4);read(h,&o,4);
  lseek(h,o,SEEK_SET);
  for(;n;--n) {
    read(h,&o,4);
    lseek(h,4,SEEK_CUR);
    read(h,s,8);
    if(strnicmp(s,wn,8)==0) break;
  }
  if(!n) {close(h);return;}
  lseek(h,o,SEEK_SET);
  read(h,&hdr,sizeof(hdr));
  if(memcmp(hdr.id,"Doom2D\x1A",8)==0) {
	for(;;) {
	  read(h,&blk,sizeof(blk));
	  if(blk.t==MB_END) break;
	  if(blk.t==MB_COMMENT)
		{lseek(h,blk.sz,SEEK_CUR);continue;}
	  if(!W_load(h))
	  if(!TH_load(h))
	  if(!SW_load(h))
		lseek(h,blk.sz,SEEK_CUR);
	}
  }else {lseek(h,o,SEEK_SET);W_load_old(h);}
  close(h);
  W_allocwalls();
}

#ifdef USE_LAYOUT_HACK
extern int savh;
extern int savo;
#else
static int savh;
static int savo;
#endif
static char tmp_file[128]="$TMPWAD$.$$$";

int F_savemap(char *fn,char *wn) {
  static map_header_t hdr={"Doom2D\x1A",LAST_MAP_VER};
  int h,o,n,c,l;
  wad_t *w;
  void *buf;
#ifdef USE_LAYOUT_HACK
  extern wad_t we;
#else
  static wad_t we;
#endif

  w=buf=NULL;
  rename(fn,tmp_file);
  h=open(tmp_file,O_BINARY|O_RDONLY);
  if((savh=open(fn,O_BINARY|O_CREAT|O_TRUNC|O_RDWR,S_IREAD|S_IWRITE))==-1) goto err;
  write(savh,"PWAD",12);
  if(h!=-1) {
    read(h,&n,4);read(h,&n,4);read(h,&o,4);
    if(!(buf=malloc(32000))) goto err;
  }else n=0;
  if(!(w=malloc((++n)*16))) goto err;
  memset(w[0].n,0,8);strncpy(w[0].n,wn,8);
  w[0].o=12;c=1;
  write(savh,&hdr,sizeof(hdr));
  W_save();
  TH_save();
  SW_save();
  F_start_blk(MB_END,0);
  F_end_blk();
  w[0].l=tell(savh)-12;
  if(h!=-1) {
    for(--n;n;--n,o+=16) {
      lseek(h,o,SEEK_SET);read(h,&we,16);
      if(strnicmp(we.n,wn,8)==0) continue;
      lseek(h,we.o,SEEK_SET);
      memcpy(w[c].n,we.n,8);
      w[c].o=tell(savh);
      for(w[c].l=0;w[c].l<we.l;) {
        l=min(we.l-w[c].l,32000);
        read(h,buf,l);write(savh,buf,l);
        w[c].l+=l;
      }
      ++c;
    }
  }
  o=tell(savh);
  write(savh,w,c*16);
  lseek(savh,4,SEEK_SET);
  write(savh,&c,4);write(savh,&o,4);
  close(savh);
  if(h!=-1) {close(h);remove(tmp_file);}
  if(w) free(w);
  if(buf) free(buf);
  return 1;
err:
  close(savh);remove(fn);
  if(h!=-1) {close(h);rename(tmp_file,fn);}
  if(w) free(w);
  if(buf) free(buf);
  return 0;
}

void F_start_blk(int t,int st) {
  blk.t=t;blk.st=st;blk.sz=0L;
  savo=tell(savh);
  write(savh,&blk,sizeof(blk));
}

void F_end_blk(void) {
  int o;

  o=tell(savh);
  lseek(savh,savo,SEEK_SET);
  write(savh,&blk,sizeof(blk));
  lseek(savh,o,SEEK_SET);
}

void F_write_blk(void *p,dword l) {
  write(savh,p,l);
  blk.sz+=l;
}

static int strlistcmp(char **a,char **b) {
  return stricmp(*a,*b);
}

char **F_make_dmm_list(void) {
  char **p,s[9];
  int i,j;

  if(!(p=malloc(1000*4))) ERR_fatal("Not enough memory");
  s[8]=0;
  for(i=m_start+1,j=0;i<m_end;++i) if(wad[i].l>=8) {
	if(memicmp(wad[i].n,"DMI",3)==0) continue;
	F_loadres(i,s,0,4);
	if(memcmp(s,"DMM",4)==0) {
	  memcpy(s,wad[i].n,8);
	  p[j++]=strdup(s);
	}
  }p[j]=NULL;
  qsort(p,j,4,strlistcmp);
  return p;
}

char **F_make_wall_list(void) {
  char **p,s[9];
  int i;

  if(!(p=malloc((w_end-w_start)*4))) ERR_fatal("Not enough memory");
  s[8]=0;
  for(i=0;i+w_start+1<w_end;++i) {
	memcpy(s,wad[i+w_start+1].n,8);
	p[i]=strdup(s);
  }p[i]=NULL;
  return p;
}

char **F_make_map_list(char *fn) {
  char **p,s[9];
  int h,n,o,i;

  s[8]=0;
  p=Z_makelist();p[0]=NULL;
  if((h=open(fn,O_BINARY|O_RDONLY))==-1) return p;
  read(h,&n,4);
  if(memcmp(&n,"IWAD",4)!=0 && memcmp(&n,"PWAD",4)!=0) {
    message(MB_OK,"Файл %s - не WAD!",fn);
    close(h);return p;
  }
  read(h,&n,4);read(h,&o,4);
  lseek(h,o,SEEK_SET);
  for(i=0;n;--n) {
    lseek(h,8,SEEK_CUR);
    read(h,s,8);
    if(memicmp(s,"MAP",3)==0) {
      p[i++]=strdup(s);
    }
  }p[i]=NULL;
  qsort(p,i,4,strlistcmp);
  close(h);
  return p;
}

char **F_make_file_list(char *dir,char **mask) {
  char **p;
  int i,f;
  DIR *dirp;
  struct dirent *ff;

  p=Z_makelist();
  if(f_path[strlen(strcpy(f_path,dir))-1]!='\\') strcat(f_path,"\\");
  strcat(f_path,"*.*");
  for(i=0,dirp=opendir(f_path);ff=readdir(dirp);)
	if(ff->d_attr&_A_SUBDIR) {
	  if(strcmp(ff->d_name,".")==0) continue;
	  sprintf(f_dir,"<%s>",ff->d_name);
	  p[i++]=strdup(f_dir);
	}
  qsort(p,f=i,4,strlistcmp);
  for(;*mask;++mask) {
	if(f_path[strlen(strcpy(f_path,dir))-1]!='\\') strcat(f_path,"\\");
	strcat(f_path,*mask);
	for(dirp=opendir(f_path);ff=readdir(dirp);) if(!(ff->d_attr&_A_SUBDIR))
	  p[i++]=strdup(ff->d_name);
  }
  p[i]=NULL;
  qsort(p+f,i-f,4,strlistcmp);
  return p;
}
