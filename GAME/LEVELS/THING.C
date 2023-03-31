#include "glob.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vga.h"
#include "error.h"
#include "files.h"
#include "memory.h"
#include "view.h"
#include "misc.h"
#include "..\map.h"

#define MANCOLOR 0xD0

extern map_block_t blk;

#define IT_TN (TH__LASTI-TH_CLIP)
#define MN_TN (TH__LASTM-TH_DEMON)

extern int curth;

static void *spr[IT_TN+1],*mon[MN_TN];
static char mond[MN_TN];
static thing_t th[MAXTH];

void TH_alloc(void) {
  int i,h;
  char nm[IT_TN+1][4]={
    "PLAY",
    "CLIP","SHEL","ROCK","CELL","AMMO","SBOX","BROK","CELP","STIM","MEDI",
    "BPAK",
    "CSAW","SHOT","SGN2","MGUN","LAUN","PLAS","BFUG",
    "ARM1","ARM2","MEGA","PINV","AQUA",
    "KEYR","KEYG","KEYB","SUIT","SOUL",
    "SMRT","SMGT","SMBT","GOR1","FCAN","GUN2"
  }, mn[MN_TN][4]={
	"SARG","TROO","POSS","SPOS","CYBR","CPOS","BOSS","BOS2","HEAD","SKUL",
	"PAIN","SPID","BSPI","FATT","SKEL","VILE","FISH","BAR1","ROBO","PLAY"
  };

  for(i=0;i<IT_TN+1;++i) spr[i]=M_lock(F_getsprid(nm[i],0,1));
  for(i=0;i<MN_TN;++i) {
    mon[i]=M_lock(h=F_getsprid(mn[i],0,1));
    mond[i]=(h&0x8000)?1:0;
  }
}

void TH_load_old(int h) {
  int i;

  for(i=0;i<MAXTH;++i) {
    read(h,&th[i],sizeof(thing_t));
    if(th[i].t==0) break;
  }
  SW_load_old(h);
}

int TH_load(int h) {
  int i;

  switch(blk.t) {
	case MB_THING:
	  for(i=0;i<MAXTH && blk.sz>0;++i,blk.sz-=sizeof(thing_t))
		read(h,&th[i],sizeof(thing_t));
	  return 1;
  }return 0;
}

void TH_save(void) {
  int i;

  F_start_blk(MB_THING,0);
  for(i=0;i<MAXTH;++i) if(th[i].t)
	F_write_blk(&th[i],sizeof(thing_t));
  F_end_blk();
}

void TH_init(void) {
  int i;

  for(i=0;i<MAXTH;++i) th[i].t=0;
}

static byte mctab[3]={0x70,0x60,0x20};

void TH_draw(void) {
  int i,s;

  for(i=0;i<MAXTH;++i) if(th[i].t) {
    if(th[i].t<=TH_DMSTART) s=mctab[th[i].t-1];
    else s=th[i].t-TH_CLIP+1;
    if(th[i].t>=TH_DEMON) {
	if(th[i].t<TH_MAN) Z_drawspr(th[i].x,th[i].y,mon[th[i].t-TH_DEMON],(th[i].f&THF_DIR)^mond[th[i].t-TH_DEMON]);
	else Z_drawmanspr(th[i].x,th[i].y,spr[0],th[i].f&THF_DIR,MANCOLOR);
    }else if(th[i].t>TH_DMSTART) Z_drawspr(th[i].x,th[i].y,spr[s],0);
	else Z_drawmanspr(th[i].x,th[i].y,spr[0],th[i].f&THF_DIR,s);
	if(i==curth) {
	  V_clr(th[i].x-5-w_x+100,11,th[i].y-10-w_y+50,1,0xAD);
	  V_clr(th[i].x-5-w_x+100,11,th[i].y-w_y+50,1,0xAD);
	  V_clr(th[i].x-5-w_x+100,1,th[i].y-9-w_y+50,9,0xAD);
	  V_clr(th[i].x+5-w_x+100,1,th[i].y-9-w_y+50,9,0xAD);
	}
  }
}

void TH_drawth(int x,int y,int t) {
  int s;

  if(t<=TH_DMSTART) s=mctab[t-1];
  else s=t-TH_CLIP+1;
  if(t>=TH_DEMON)
	V_spr(x,y,mon[t-TH_DEMON]);
  else if(t>TH_DMSTART) V_spr(x,y,spr[s]);
  else V_manspr(x,y,spr[0],s);
}

int TH_add(int x,int y,int t) {
  int i;

  for(i=0;i<MAXTH;++i) if(!th[i].t) {
    th[i].t=t;
    th[i].x=x;th[i].y=y;
	th[i].f=0;
    return i;
  }
  return -1;
}

void TH_info(char *s) {
  int i,p[3],m,t;

  p[0]=p[1]=p[2]=m=t=0;
  for(i=0;i<MAXTH;++i) if(th[i].t) {
	if(th[i].t>=TH_DEMON) ++m;
	else if(th[i].t>TH_DMSTART) ++t;
	else ++p[th[i].t-TH_PLR1];
  }
  sprintf(s,"1ых игроков: %d   2ых игроков: %d   точек DM: %d\n"
    "предметов: %d   монстров: %d\n\n",p[0],p[1],p[2],t,m);
}

int TH_isthing(int x,int y) {
  int i;

  for(i=0;i<MAXTH;++i) if(th[i].t) {
    if(x<th[i].x-5) continue;
    if(x>th[i].x+5) continue;
    if(y<th[i].y-10) continue;
    if(y>th[i].y) continue;
    return i;
  }
  return -1;
}

void TH_move(int t,int x,int y) {
  th[t].x=x;th[t].y=y;
}

int TH_getx(int t) {return th[t].x;}

int TH_gety(int t) {return th[t].y;}

int TH_getf(int t) {return th[t].f;}

int TH_gett(int t) {return th[t].t;}

void TH_setf(int t,int f) {th[t].f=f;}

void TH_sett(int t,int f) {th[t].t=f;}

void TH_delete(int t) {th[t].t=0;}

int TH_nextt(int t) {
  switch(t) {
	case TH__LASTM-1:	return TH_PLR1;
	case TH__LASTI-1:	return TH_DEMON;
    case TH_DMSTART:	return TH_CLIP;
  }
  return t+1;
}

int TH_prevt(int t) {
  switch(t) {
	case TH_DEMON:	return TH__LASTI-1;
	case TH_PLR1:	return TH__LASTM-1;
    case TH_CLIP:	return TH_DMSTART;
  }
  return t-1;
}

char *TH_getname(int t) {
  switch(t) {
	case TH_PLR1:	return "1ый игрок";
	case TH_PLR2: 	return "2ой игрок";
	case TH_DMSTART:return "точка DM";

	case TH_CLIP:	return "патроны";
	case TH_SHEL:	return "4 гильзы";
	case TH_ROCKET:	return "1 ракета";
	case TH_CELL:	return "батарейка";
	case TH_AMMO:	return "ящик патронов";
	case TH_SBOX:	return "25 гильз";
	case TH_RBOX:	return "5 ракет";
	case TH_CELP:	return "батарея";
	case TH_STIM:	return "аптечка";
	case TH_MEDI:	return "б.аптечка";
	case TH_BPACK:	return "рюгзак";
	case TH_CSAW:	return "бензопила";
	case TH_SGUN:	return "ружьё";
	case TH_SGUN2:	return "двустволка";
	case TH_MGUN:	return "пулемёт";
	case TH_LAUN:	return "ракетница";
	case TH_PLAS:	return "плазм.пушка";
	case TH_BFG:	return "BFG9000";
	case TH_ARM1:	return "зелёная броня";
	case TH_ARM2:	return "синяя броня";
	case TH_MEGA:	return "мегасфера";
	case TH_INVL:	return "неуязвимость";
	case TH_AQUA:	return "акваланг";
	case TH_RKEY:	return "красный ключ";
	case TH_GKEY:	return "зеленый ключ";
	case TH_BKEY:	return "синий ключ";
	case TH_SUIT:	return "костюм";
	case TH_SUPER:	return "шарик 100%";
	case TH_RTORCH:	return "красный факел";
	case TH_GTORCH:	return "зеленый факел";
	case TH_BTORCH:	return "синий факел";
	case TH_GOR1:	return "чувак";
	case TH_FCAN:	return "горящая бочка";
	case TH_GUN2:	return "суперпулемёт";

	case TH_DEMON:	return "демон";
	case TH_IMP:	return "бес";
	case TH_ZOMBY:	return "зомби";
	case TH_SERG:	return "сержант";
	case TH_CYBER:	return "кибердемон";
	case TH_CGUN:	return "пулемётчик";
	case TH_BARON:	return "барон ада";
	case TH_KNIGHT:	return "рыцарь ада";
	case TH_CACO:	return "какодемон";
	case TH_SOUL:	return "огненный череп";
	case TH_PAIN:	return "авиабаза";
	case TH_SPIDER:	return "большой паук";
	case TH_BSP:	return "арахнотрон";
	case TH_MANCUB:	return "манкубус";
	case TH_SKEL:	return "скелет";
	case TH_VILE:	return "колдун";
	case TH_FISH:	return "рыба";
	case TH_BARREL:	return "бочка";
	case TH_ROBO:	return "робот";
	case TH_MAN:	return "приколист";
  }
  return NULL;
}
