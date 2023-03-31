#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "dll.h"

DLLEXPORT void readstrz(int h,char *s,int m) {
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
