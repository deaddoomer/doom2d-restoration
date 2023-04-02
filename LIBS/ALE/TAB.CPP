#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <malloc.h>
#include <tabsort.h>
#include <str.h>
#include "dll.h"

DLLEXPORT String String::operator +(String s) {
  String ns(*this);
  if(!ns.used) {
    if(!ns.resize(1)) return ns;
    ns[0]=0;
  }
  int l=strlen(s.ptr);
  ns.insert(used-1,l,s.ptr);
  return ns;
}

DLLEXPORT String& String::operator +=(String s) {
  if(!used) {
    if(!resize(1)) return *this;
    ptr[0]=0;
  }
  int l=strlen(s.ptr);
  insert(used-1,l,s.ptr);
  return *this;
}

DLLEXPORT int String::printf(int sz,char *s,...) {
  if(!resize(sz)) return -1;
  va_list ap;
  va_start(ap,s);
  int c=_vbprintf(ptr,sz,s,ap);
  va_end(ap);
  used=c+1;
  shrink();
  return c;
}

DLLEXPORT String::String(int sz,char *s,...) {
  if(!resize(sz)) return;
  va_list ap;
  va_start(ap,s);
  int c=_vbprintf(ptr,sz,s,ap);
  va_end(ap);
  used=c+1;
  shrink();
}

DLLEXPORT int TABinsert(void*&ptr,int &total,int &used,int at,int n,int sz,void *p,int STEP) {
  if(STEP<=0) STEP=1;
  if(at>used) at=used;
  int nn=used+n;
  if(nn>total) {
    if(total+STEP>=nn) nn=total+STEP;
    void *nt=malloc(nn*sz);
    if(!nt) return -1;
    if(at) memcpy(nt,ptr,at*sz);
    if(p) memcpy((char*)nt+at*sz,p,n*sz);
    if(used-at) memcpy((char*)nt+(at+n)*sz,(char*)ptr+at*sz,(used-at)*sz);
    if(ptr) free(ptr);
    ptr=nt;
    total=nn;used+=n;
  }else{
    if(used-at) memmove((char*)ptr+(at+n)*sz,(char*)ptr+at*sz,(used-at)*sz);
    if(p) memcpy((char*)ptr+at*sz,p,n*sz);
    used+=n;
  }
  return at;
}

DLLEXPORT int TABSORTEDinsert(void*&ptr,int &total,int &used,int sz,void *e,int STEP,
int (*sf) (const void*,const void*)) {
  if(!ptr || used<=0 || !sf || !e)
    return TABinsert(ptr,total,used,used,1,sz,e,STEP);
  int a=0,b=used-1;
  char *p=(char*)ptr;
  if(sf(e,p)<=0) return TABinsert(ptr,total,used,0,1,sz,e,STEP);
  if(sf(p+(used-1)*sz,e)<=0) return TABinsert(ptr,total,used,used,1,sz,e,STEP);
  while(a<b) {
    int c=(a+b)/2;
    if(c==a) return TABinsert(ptr,total,used,a+1,1,sz,e,STEP);
    int v=sf(e,p+c*sz);
    if(v==0) return TABinsert(ptr,total,used,c,1,sz,e,STEP);
    else if(v<0) b=c; else a=c;
  }
  return TABinsert(ptr,total,used,a+1,1,sz,e,STEP);
}

static int __sort_w;
static int (*__sort_f) (const void*,const void*);
static void *__sort_x,*__sort_y;

static void __sort(char *a,int n) {
  int i=0,j=n-1;
  memcpy(__sort_x,a+n/2*__sort_w,__sort_w);
  do{
    while(__sort_f(a+i*__sort_w,__sort_x) < 0) ++i;
    while(__sort_f(__sort_x,a+j*__sort_w) < 0) --j;
    if(i<=j) {
      if(i!=j) {
	memcpy(__sort_y,a+i*__sort_w,__sort_w);
	memcpy(a+i*__sort_w,a+j*__sort_w,__sort_w);
	memcpy(a+j*__sort_w,__sort_y,__sort_w);
      }
      ++i;--j;
    }
  }while(i<=j);
  if(j>0) __sort(a,j+1);
  if(i<n-1) __sort(a+i*__sort_w,n-i);
}

extern "C" DLLEXPORT void qusort(void *p,int n,int w,int (*f) (const void*,const void*)) {
  if(!p || n<2 || !w || !f) return;
  __sort_w=w;__sort_f=f;
  if(!(__sort_x=malloc(w*2))) return;
  __sort_y=(char*)__sort_x+w;
  __sort((char*)p,n);
  free(__sort_x);
}
