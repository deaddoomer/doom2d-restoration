
#ifndef __STRING_LIB_H
#define __STRING_LIB_H

#include <tab.h>

#ifdef __cplusplus

class String:public Tab<char>{
public:
  String() {}
  String(int sz,char *s,...);
  String(char *s) {
    int l=strlen(s);if(resize(l+1)) {memcpy(ptr,s,l+1);}
  }
  String& operator =(char *s) {
    int l=strlen(s);if(resize(l+1)) {memcpy(ptr,s,l+1);}
    return *this;
  }
  int length() {if(used<=0) return 0; else return used-1;}
  int printf(int sz,char *s,...);
  String operator +(String s);
  String& operator +=(String s);
};

#endif

#endif
