#include "glob.h"
#include <stdio.h>
#include <conio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "vga.h"
#include "memory.h"
#include "files.h"
#include "error.h"

void GUI_close(void);

static void near close_all(void) {
  GUI_close();
//  V_done();
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
  puts("\nFATAL ERROR:");
  va_start(ap,s);
  vprintf(s,ap);
  va_end(ap);
  puts("");
  exit(2);
}

void ERR_quit(void) {
  close_all();
  exit(0);
}
