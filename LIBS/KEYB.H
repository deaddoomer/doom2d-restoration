/*
  Драйвер клавиатуры V1.1 для DOS4GW (а также DirectX 3)
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

// тип функции обработки клавиш
typedef void key_f(int k,int pressed);

#ifndef __NT__
// включить драйвер
DLLEXTERN void K_init(void);
#else
#ifdef _WINDOWS_
DLLEXTERN int K_init(HINSTANCE,HWND);
#endif
DLLEXTERN void K_acquire();
DLLEXTERN void K_unacquire();
#endif

// выключить драйвер
DLLEXTERN void K_done(void);

// установить функцию обработки клавиш
DLLEXTERN void K_setkeyproc(key_f *);

DLLEXTERN key_f *K_getkeyproc();

// стандартная функция обработки клавиш
DLLEXTERN key_f def_key_proc;

// массив клавиш: 0 - отпущена, иначе - нажата
DLLEXTERN char volatile keys[256];

#ifdef __NT__
#ifdef __DINPUT_INCLUDED__
DLLEXTERN LPDIRECTINPUT DInput;
#endif
#endif

#ifdef __cplusplus
}
#endif

#undef DLLEXTERN
