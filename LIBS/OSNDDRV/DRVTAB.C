#include <stddef.h>
#include "..\osnddrv.h"

void __none_drv(void);
void __adlib_drv(void);
void __covox_drv(void);
void __pc1_drv(void);
void __pc8_drv(void);
void __sb_drv(void);
void __sb16_drv(void);
//void __sbnodma_drv(void);

void *snd_drv_tab[7+1]={
  __none_drv,
  __adlib_drv,
  __covox_drv,
  __pc1_drv,
  __pc8_drv,
  __sb_drv,
//  __sbnodma_drv,
  __sb16_drv,
  NULL
};
