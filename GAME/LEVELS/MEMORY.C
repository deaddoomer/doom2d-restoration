#include "glob.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "error.h"
#include "files.h"
#include "memory.h"

dword dpmi_memavl(void);

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
  if(!m_active) return;
  m_active=FALSE;
}

static void allocres(int h) {
  int *p;

  if(!(p=malloc(wad[h].l+4)))
    ERR_fatal("M_lock: не хватает памяти");
  *p=h;
  ++p;
  resp[h]=p;
  F_loadres(h,p,0,wad[h].l);
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
