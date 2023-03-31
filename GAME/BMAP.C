#include "glob.h"
#include "view.h"
#include "bmap.h"

byte fld_need_remap=1;

byte bmap[FLDH/4][FLDW/4];

void BM_mark(obj_t *o,byte f) {
  int x,y;
  int xs,ys,xe,ye;

  if((xs=(o->x-o->r)>>5)<0) xs=0;
  if((xe=(o->x+o->r)>>5)>=FLDW/4) xe=FLDW/4-1;
  if((ys=(o->y-o->h)>>5)<0) ys=0;
  if((ye=o->y>>5)>=FLDH/4) ye=FLDH/4-1;
  for(y=ys;y<=ye;++y)
	for(x=xs;x<=xe;++x)
	  bmap[y][x]|=f;
}
