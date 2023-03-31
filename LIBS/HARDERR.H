/*
  Библиотека для DOS4GW
  Версия 1.0
  Обработка критических ошибок DOS
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

typedef int harderr_f(int error,int drive,int flags);

enum{HARDERR_IGNORE=0,HARDERR_RETRY,HARDERR_ABORT,HARDERR_FAIL};

#define HARDERRF_DISK           0x80
#define HARDERRF_MAY_IGNORE     0x20
#define HARDERRF_MAY_RETRY      0x10
#define HARDERRF_MAY_FAIL       8
#define HARDERRF_WRITE          1

DLLEXTERN void harderr_inst(harderr_f *);

#ifdef __cplusplus
}
#endif

#undef DLLEXTERN
