#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dll.h"

#define MAX_CFG_STR 500

DLLEXPORT char cfg_str[MAX_CFG_STR+5];
DLLEXPORT int cfg_val;

static FILE *fh;

DLLEXPORT int CFG_read(char *fn,char *sec,char *nm) {
  char *p,*s;

  if(!(fh=fopen(fn,"rt"))) return 0;
  for(;;) {
    if(!fgets(cfg_str,MAX_CFG_STR,fh)) {fclose(fh);return 0;}
    if(!(p=strtok(cfg_str," \r\n\t"))) continue;
    if(*p=='\'') continue;
    if(*p=='[') {
  sect:
      ++p;
      if(s=strchr(p,']')) *s=0;
      if(stricmp(p,sec)==0) break;
    }
  }
  for(;;) {
    char *str=NULL;
    if(!fgets(cfg_str,MAX_CFG_STR,fh)) {fclose(fh);return 0;}
    str=strchr(cfg_str,'=');
    if(str) if(*(++str)!='\"') str=NULL;
    if(!(p=strtok(cfg_str," \r\n\t="))) continue;
    if(*p=='\'') continue;
    if(*p=='[') goto sect;
    if(stricmp(p,nm)!=0) continue;
    if(str) {
      for(p=cfg_str,++str;*str && *str!='\"';++str,++p)
	if(*str=='\\') {
	  ++str;
	  if(!*str) break;
	  else if(*str=='n') *p='\n';
	  else if(*str=='r') *p='\r';
	  else if(*str=='t') *p='\t';
	  else *p=*str;
	}else *p=*str;
      *p=0;
      if(stricmp(cfg_str,"ON")==0 || stricmp(cfg_str,"YES")==0) {cfg_val=1;break;}
      if(stricmp(cfg_str,"OFF")==0 || stricmp(cfg_str,"NO")==0) {cfg_val=0;break;}
      cfg_val=strtol(cfg_str,NULL,0);
      break;
    }
    if(!(p=strtok(NULL," \r\n\t\'"))) continue;
    memmove(cfg_str,p,strlen(p)+1);
    if(stricmp(cfg_str,"ON")==0 || stricmp(cfg_str,"YES")==0) {cfg_val=1;break;}
    if(stricmp(cfg_str,"OFF")==0 || stricmp(cfg_str,"NO")==0) {cfg_val=0;break;}
    cfg_val=strtol(cfg_str,NULL,0);
    break;
  }
  fclose(fh);
  return 1;
}
