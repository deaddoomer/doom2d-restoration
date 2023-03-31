/*
  Библиотека для DOS4GW
  Версия 1.0
  (C) Алексей Волынсков, 1996
*/

#ifndef _DLL
#pragma library("av.lib")
#define DLLEXTERN extern
#else
#ifndef __B_AVLIB
#pragma library("avdll.lib")
#define DLLEXTERN __declspec(dllimport)
#else
#define DLLEXTERN extern
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* опознавание типа процессора:
  0 - 8086/8088
  2 - 80286
  3 - 80386
  4 - 80486
  5 - Pentium
  возвращает и записывает в _cpu тип процессора */
DLLEXTERN short what_cpu(void);

// читает ASCIIZ-строку s из файла h (максимальная длина строки m)
DLLEXTERN void readstrz(int h,char *s,int m);

DLLEXTERN short _cpu;

#ifdef __cplusplus
}
#endif

#undef DLLEXTERN
