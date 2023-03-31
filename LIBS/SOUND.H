/*
  Управление звуком и музыкой для DOS4GW
  Версия 1.0
  (C) Алексей Волынсков, 1996
*/

#pragma library("sound.lib")

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
  unsigned int   len,		// длина в байтах
                 rate,		// частота в Гц.
                 lstart,	// начало повтора в байтах от начала данных
                 llen;		// длина повтора в байтах
}snd_t;

// установить драйвер n
// (см. snddrv.h)
void set_snd_drv(short n);

// проверить наличие звуковой карты
// возвращает 0, если карты нет
short S_detect(void);

// включить драйвер
void S_init(void);

// выключить драйвер
void S_done(void);

// проиграть звук s на канале c (1-8), частоте r и громкости v (0-255)
// возвращает номер канала, на котором играется звук
// если c==0, то звук попадет в любой свободный канал
// r - это относительная частота (обычно 1024)
short S_play(snd_t *s,short c,unsigned r,short v);

// остановить звук на канале c (1-8)
void S_stop(short c);

// установить частоту r у звука на канале c
void S_setrate(short c,unsigned r);

// установить громкость v (0-255) у звука на канале c
void S_setvolume(short c,int v);

// начать музыку
void S_startmusic(void);

// остановить музыку
void S_stopmusic(void);

// включить драйвер таймера
// обычно включается при вызове S_init()
void T_init(void);

// выключить драйвер таймера
// обычно выключается при вызове S_done()
void T_done(void);

// громкость звука и музыки (0-128)
extern short snd_vol,mus_vol;

extern char s_interp;

// порт, DMA, IRQ
extern unsigned short snd_port,snd_dma,snd_irq;

// тип карты (см. выше)
extern short snd_type;

// название карты
extern char *snd_name;

// частота смешанного звука в Герцах
extern unsigned short sfreq;

// таблица относительных частот нот
extern unsigned short note_tab[96];

// таймер
extern volatile int timer;

#ifdef __cplusplus
}
#endif
