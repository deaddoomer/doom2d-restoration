#include "glob.h"
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <malloc.h>
#include "keyb.h"
#include "sound.h"
#include "vga.h"
#include "memory.h"
#include "files.h"
#include "error.h"

void CFG_save(void);

void K_fast(void);

void gotoxy(int,int);

void error(void) {}

void close_all(void) {
  S_done();
  K_done();K_fast();
  V_done();
  M_shutdown();
}

void ERR_failinit(char *s,...) {
  va_list ap;

  close_all();
  va_start(ap,s);
  vprintf(s,ap);
  va_end(ap);
  puts("");
  exit(1);
}

void ERR_fatal(char *s,...) {
  va_list ap;

  close_all();
  puts("\nКРИТИЧЕСКАЯ ОШИБКА:");
  va_start(ap,s);
  vprintf(s,ap);
  va_end(ap);
  puts("");
  exit(2);
}

void ERR_quit(void) {
  void *p;

  V_done();
  if(!(p=malloc(4000)))
    puts("Спасибо за то, что вы играли в Doom2D!");
  else {
    F_loadres(F_getresid("ENDOOM"),p,0,4000);
    memcpy((void*)0xB8000,p,4000);free(p);gotoxy(1,24);
  }
  close_all();
  CFG_save();
  exit(0);
}
