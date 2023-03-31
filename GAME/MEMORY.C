#include "glob.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "error.h"
#include "files.h"
#include "memory.h"

dword dpmi_memavl(void);

extern int d_start,d_end;

extern mwad_t wad[];

static byte m_active=FALSE;

static void *resp[MAX_WAD];
static short resl[MAX_WAD];

void M_startup(void) {
  if(m_active) return;
  logo("M_startup: настройка памяти\n");
  memset(resp,0,sizeof(resp));
  memset(resl,0,sizeof(resl));
  logo("  свободно DPMI-памяти: %uK\n",dpmi_memavl()>>10);
  m_active=TRUE;
}

void M_shutdown(void) {
//  FILE *h;
//  int i;

  if(!m_active) return;
  m_active=FALSE;
/*
  if(!(h=fopen("res_use.txt","wt"))) return;
  for(i=0;i<MAX_WAD;++i) if(resp[i]) {
    fprintf(h,"%.8s\n",wad[i].n);
  }
  fclose(h);
*/
}

static void allocres(int h) {
  int *p,s;

  if(h>d_start && h<d_end) s=1; else s=0;
  if(!(p=malloc(wad[h].l+4+s*8)))
    ERR_fatal("M_lock: не хватает памяти");
  *p=h;
  ++p;
  resp[h]=p;
  if(s) {
    p[0]=p[1]=p[2]=p[3]=0;
    F_loadres(h,p,0,2);
    F_loadres(h,p+1,2,2);
    F_loadres(h,p+2,4,2);
    F_loadres(h,p+3,6,2);
    F_loadres(h,p+4,8,wad[h].l-8);
  }else F_loadres(h,p,0,wad[h].l);
}

void *M_lock(int h) {
  if(h==-1 || h==0xFFFF) return NULL;
  h&=-1-0x8000;
  if(h>=MAX_WAD) ERR_fatal("M_lock: странный номер ресурса");
  if(!resl[h]) if(!resp[h]) allocres(h);
  ++resl[h];
  return resp[h];
}

void M_unlock(void *p) {
  int h;

  if(!p) return;
  h=((int*)p)[-1];
  if(h>=MAX_WAD) ERR_fatal("M_unlock: странный номер ресурса");
  if(!resl[h]) return;
  --resl[h];
}
