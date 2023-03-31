/*
  Звуковые драйвера для DOS4GW
  Версия 1.0
  (C) Алексей Волынсков, 1996
*/

#pragma library("snddrv.lib")

#ifdef __cplusplus
extern "C" {
#endif

// номера драйверов
enum{
  SDRV_NONE=0,          // тишина...
  SDRV_ADLIB,           // Adlib: 6 бит
  SDRV_COVOX,           // Covox Speech Thing: 8 бит
			// snd_port = номер LPT(1-4)
  SDRV_PC1,             // PC Speaker: 1 бит
  SDRV_PC8,             // PC Speaker: вроде как 8 бит (или 7)
  SDRV_SB,              // Sound Blaster: 8 бит, DMA
			// snd_port = базовый порт (обычно 0x220)
			// snd_iqr = номер IRQ (обычно 7 или 5)
			// sorry, DMA пока только 1
  SDRV_SBNODMA,               // Sound Blaster: 8 бит, без DMA (медленнее)
  SDRV__END
};

// адреса драйверов
extern void *snd_drv_tab[];

#ifdef __cplusplus
}
#endif
