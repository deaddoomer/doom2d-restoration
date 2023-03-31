/*
  Библиотека для DOS4GW
  Версия 1.0
  Обработка ошибок
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

#ifndef __cplusplus

// обработка ошибки в зоне z, типа t, номер n
// определяется пользователем
void error(short z,short t,short n,char *s1,char *s2);

#endif

// зона ошибки EZ_*
enum{
  EZ_SYSTEM,            // в системе
  EZ_LIB,               // в библиотеке
  EZ_VGALIB,            // в библиотеке VGA
  EZ_LOADVGA,           // при загрузке изображения s1 из файла s2
  EZ_LOADANIM,          // при загрузке анимации из файла s1
  EZ_LOADSND,           // при загрузке звука из файла s1
  EZ_USER
};

// тип ошибки ET_*
enum{
  ET_STD,               // стандартная ошибка (см. strerror, errno.h)
  ET_AVLIB,             // ошибка библиотеки AV.LIB
  ET_USER
};

// номера ошибок библиотеки AV.LIB
enum{
  EN_NONE,              // нет ошибки
  EN_BADFORMAT,         // неправильный формат данных
  EN_RESNOTFOUND,       // ресурс не найден
  EN_HARDERR,           // ошибка аппаратуры
  EN_LAST
};

// названия зон ошибок
DLLEXTERN char *av_ez_msg[];

// сообщения об ошибках AV.LIB
DLLEXTERN char *av_err_msg[];

#ifdef __cplusplus
}
#endif
