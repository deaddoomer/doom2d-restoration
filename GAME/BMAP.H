// Block map

#define BM_WALL		1
#define BM_PLR1		2
#define BM_PLR2		4
#define BM_MONSTER	8

void BM_clear(byte f);
void BM_mark(obj_t *o,byte f);
void BM_remapfld(void);

extern byte bmap[FLDH/4][FLDW/4];
extern byte fld_need_remap;
