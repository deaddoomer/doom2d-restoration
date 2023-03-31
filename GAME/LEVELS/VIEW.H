// View functions

#define FLDW 100
#define FLDH 100
#define CELW 8
#define CELH 8
#define MAXTXW 16
#define MAXTXH 8

#include "..\things.h"
#include "thing.h"
#include "switch.h"

#pragma pack(1)

typedef struct{
  char n[8];
  char t;
}wall_t;

#pragma pack()

#ifdef __cplusplus
extern "C" {
#endif

void W_init(void);
void W_allocwalls(void);
void W_addwall(char*);
void W_draw(void);
void W_adjust(void);

void W_drawwl(int x,int y,byte t);

void W_load_old(int);
int W_load(int h);
void W_save(void);

void W_setsky(void);

extern int w_o,w_x,w_y,skytype;
extern byte fldb[FLDH][FLDW];
extern byte fldf[FLDH][FLDW];
extern byte fld[FLDH][FLDW];
extern wall_t wal[255];
extern byte w_drawb,w_drawt,w_drawf,w_drawth,w_drawsw;

#ifdef __cplusplus
}
#endif
