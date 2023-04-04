/*
  Упрощенное управление звуком для DOS4GW
  Версия 1.1
  (C) Алексей Волынсков, 1996
*/

#pragma library("fsound.lib")

#ifdef __cplusplus
extern "C" {
#endif

// частота таймера
#define TIMER_FREQ 1193180L

// 1/f доля секунды в еденицах таймера (см. timer)
// например: for(timer=0;timer<tlength(25);); // пауза на 1/25 секунды
#define tlength(f) (TIMER_FREQ/f)

// тип драйвера
enum{
  ST_NONE=-1,	// нет карты
  ST_DAC,	// побайтовый ЦАП
  ST_DMA	// DMA-карта
};

// заголовок инструмента (DMI)
typedef struct{
  unsigned short len,	 // длина в байтах
                 rate,	 // частота в Герцах
                 lstart, // начало повтора в байтах от начала данных
                 llen;	 // длина повтора в байтах
}snd;

// установить драйвер n
// (см. snddrv.h)
void set_snd_drv(short n);

// проверить наличие звуковой карты
// возвращает 0, если карты нет
int S_detect(void);

// включить драйвер
void S_init(void);

// выключить драйвер
void S_done(void);

// проиграть звук s
void S_play(snd *s);

// остановить звук
void S_stop(void);

// проигрывается ли звук?
// возвращает 0, если нет
int S_issound(void);

// загрузить звук из файла f (чистый 8-битовый звук)
// и установить его частоту r (Гц), начало и длину повтора ls и ll
// (если ll==0, то звук не повторяется)
// (если ll==1, то звук повторяется полностью)
snd *load_snd(char *f,unsigned r,unsigned ls,unsigned ll);

// загрузить звук из файла f (формат DMI)
snd *load_dmi(char *f);

// включить драйвер таймера
// обычно включается при вызове S_init()
void T_init(void);

// выключить драйвер таймера
// обычно выключается при вызове S_done()
void T_done(void);

// порт, DMA, IRQ
extern unsigned short snd_port,snd_dma,snd_irq;

// тип карты (см. выше)
extern short snd_type;

// название карты
extern char *snd_name;

// частота смешанного звука в Герцах
extern unsigned short sfreq;

// таймер
extern volatile int timer;

#ifdef __cplusplus
}
#endif
