#include "glob.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "files.h"
#include "memory.h"
#include "vga.h"
#include "error.h"
#include "keyb.h"
#include "sound.h"
#include "view.h"
#include "bmap.h"
#include "fx.h"
#include "switch.h"
#include "weapons.h"
#include "items.h"
#include "dots.h"
#include "player.h"
#include "monster.h"
#include "menu.h"
#include "misc.h"
#include "map.h"

#define GETIME 1092

void ANM_start(void *p);
int ANM_play(void);

extern short lastkey;

extern word chk;

extern vgapal std_pal;
void setgamma(int);

extern int sw_secrets;

#define PL_FLASH 90

void Z_clrst(void);

extern int PL_JUMP;

extern map_block_t blk;

extern byte clrmap[256*11];

void V_maptoscr(int,int,int,int,void *);

void F_loadgame(int);

extern byte cheat;

extern int sky_type;

byte _2pl=0,g_dm=0,g_st=GS_TITLE,g_exit=0,g_map=1,_warp=0,g_music[8]="";
int g_sttm=1092;

dword g_time;
int dm_pnum,dm_pl1p,dm_pl2p;
pos_t dm_pos[100];
static void *telepsnd,*endanim1,*endanim2;
static void *scrnh[3];

extern int sky_type;

static void chk_exit(void) {
  static char msg[]={'П'^85,'о'^85,'ж'^85,'а'^85,'л'^85,'у'^85,'й'^85,'с'^85,
    'т'^85,'а'^85,' '^85,
    'п'^85,'е'^85,'р'^85,'е'^85,'у'^85,'с'^85,'т'^85,'а'^85,'н'^85,'о'^85,
    'в'^85,'и'^85,'т'^85,'е'^85,' '^85,'и'^85,'г'^85,'р'^85,'у'^85,0};
  int i;

  for(i=0;msg[i];++i) msg[i]^=85;
  ERR_fatal("%s",msg);
}

void G_savegame(int h) {
  write(h,&_2pl,1);write(h,&g_dm,1);write(h,&g_exit,1);write(h,&g_map,1);
  write(h,&g_time,4);write(h,&dm_pl1p,4);write(h,&dm_pl2p,4);
  write(h,&dm_pnum,4);write(h,dm_pos,dm_pnum*sizeof(pos_t));
  write(h,&cheat,1);
  write(h,g_music,8);
}

void G_loadgame(int h) {
  read(h,&_2pl,1);read(h,&g_dm,1);read(h,&g_exit,1);read(h,&g_map,1);
  read(h,&g_time,4);read(h,&dm_pl1p,4);read(h,&dm_pl2p,4);
  read(h,&dm_pnum,4);read(h,dm_pos,dm_pnum*sizeof(pos_t));
  read(h,&cheat,1);
  read(h,g_music,8);F_loadmus(g_music);
}

int G_load(int h) {
  switch(blk.t) {
	case MB_MUSIC:
	  read(h,g_music,8);
	  F_loadmus(g_music);
	  return 1;
  }return 0;
}

void load_game(int n) {
  F_freemus();
  W_init();
  F_loadgame(n);
  V_setscr(scrbuf);V_setrect(0,320,0,200);
  V_clr(0,320,0,200,0);
  if(_2pl) {w_o=0;Z_clrst();w_o=100;Z_clrst();}
  else {w_o=50;Z_clrst();}
  V_copytoscr(0,320,0,200);
  pl1.drawst=0xFF;
  if(_2pl) pl2.drawst=0xFF;
  g_st=GS_GAME;
  BM_remapfld();
  BM_clear(BM_PLR1|BM_PLR2|BM_MONSTER);
  BM_mark(&pl1.o,BM_PLR1);
  if(_2pl) BM_mark(&pl2.o,BM_PLR2);
  MN_mark();
  S_startmusic();
}

void G_start(void) {
  char s[8];

  F_freemus();
  sprintf(s,"MAP%02u",(word)g_map);
  F_loadmap(s);
  V_setscr(scrbuf);V_setrect(0,320,0,200);
  V_clr(0,320,0,200,0);
  if(_2pl) {w_o=0;Z_clrst();w_o=100;Z_clrst();}
  else {w_o=50;Z_clrst();}
  V_copytoscr(0,320,0,200);
  pl1.drawst=0xFF;
  if(_2pl) pl2.drawst=0xFF;
  g_st=GS_GAME;
  g_exit=0;
  itm_rtime=(g_dm)?1092:0;
  p_immortal=0;PL_JUMP=10;
  g_time=0;
  BM_remapfld();
  BM_clear(BM_PLR1|BM_PLR2|BM_MONSTER);
  BM_mark(&pl1.o,BM_PLR1);
  if(_2pl) BM_mark(&pl2.o,BM_PLR2);
  MN_mark();
  S_startmusic();
}

#define GGAS_TOTAL (MN__LAST-MN_DEMON+16+10)

void G_init(void) {
  logo("G_init: setting up game data ");
  telepsnd=Z_getsnd("TELEPT");
  scrnh[0]=M_lock(F_getresid("TITLEPIC"));
  scrnh[1]=M_lock(F_getresid("INTERPIC"));
  scrnh[2]=M_lock(F_getresid("ENDPIC"));
  endanim1=M_lock(F_getresid("ENDANIM"));
  endanim2=M_lock(F_getresid("END2ANIM"));
  logo_gas(5,GGAS_TOTAL);
  DOT_alloc();
  FX_alloc();
  WP_alloc();
  IT_alloc();
  SW_alloc();
  PL_alloc();
  MN_alloc();
  Z_initst();
  logo_gas(GGAS_TOTAL,GGAS_TOTAL);
  logo("\n");
//  GM_init();
  pl1.color=0x70;
  pl2.color=0x60;
}

void G_act(void) {
  static byte pcnt=0;

  if(g_st==GS_ENDANIM || g_st==GS_END2ANIM) {
    V_setscr(NULL);
    if(!ANM_play()) {
      g_st=(g_st==GS_ENDANIM)?GS_DARKEN:GS_ENDSCR;
      g_sttm=0;
    }
    V_setscr(scrbuf);
    return;
  }else if(g_st==GS_DARKEN) {
    if(++g_sttm>=105) {
      V_setscr(NULL);V_clr(0,320,0,200,0);V_setscr(scrbuf);
      setgamma(gamma);
      g_st=GS_END2ANIM;ANM_start(endanim2);
    }
    if(g_sttm>64) return;
    VP_tocolor(std_pal,0,0,0,64,g_sttm);VP_setall(pal_tmp);
    return;
  }
  if(GM_act()) return;
  switch(g_st) {
	case GS_TITLE: case GS_ENDSCR:
	  return;
	case GS_INTER:
#ifdef DEMO
	  if(keys[0x39] || keys[0x1C] || keys[0x9C]) {
	    g_st=GS_TITLE;
	  }
#else
	  if(keys[0x39] || keys[0x1C] || keys[0x9C])
	    G_start();
#endif
	  return;
  }
  ++g_time;
  pl1.hit=0;pl1.hito=-3;
  if(_2pl) {pl2.hit=0;pl2.hito=-3;}
  G_code();
  if(chk==1)
    if(g_time>GETIME) {
      p_immortal=0;
      PL_hit(&pl1,1,0,HIT_SOME);
      if(_2pl) PL_hit(&pl2,1,0,HIT_SOME);
      if(!_2pl) {
        if(PL_isdead(&pl1)) chk_exit();
      }else if(PL_isdead(&pl1) && PL_isdead(&pl2)) chk_exit();
    }
  W_act();
  IT_act();
  SW_act();
  if(_2pl) {
	if(pcnt) {PL_act(&pl1);PL_act(&pl2);}
	else {PL_act(&pl2);PL_act(&pl1);}
	pcnt^=1;
  }else PL_act(&pl1);
  MN_act();
  if(fld_need_remap) BM_remapfld();
  BM_clear(BM_PLR1|BM_PLR2|BM_MONSTER);
  BM_mark(&pl1.o,BM_PLR1);
  if(_2pl) BM_mark(&pl2.o,BM_PLR2);
  MN_mark();
  WP_act();
  DOT_act();
  FX_act();
  if(_2pl) {
	PL_damage(&pl1);PL_damage(&pl2);
	if(!(pl1.f&PLF_PNSND) && pl1.pain) PL_cry(&pl1);
	if(!(pl2.f&PLF_PNSND) && pl2.pain) PL_cry(&pl2);
	if((pl1.pain-=5) < 0) {pl1.pain=0;pl1.f&=(0xFFFF-PLF_PNSND);}
	if((pl2.pain-=5) < 0) {pl2.pain=0;pl2.f&=(0xFFFF-PLF_PNSND);}
  }else{
	PL_damage(&pl1);
	if(!(pl1.f&PLF_PNSND) && pl1.pain) PL_cry(&pl1);
	if((pl1.pain-=5) < 0) {pl1.pain=0;pl1.f&=(0xFFFF-PLF_PNSND);}
  }
  if(g_exit==1) {
	switch(g_map) {
	  case 19: g_st=GS_ENDANIM;ANM_start(endanim1);break;
	  case 31: case 32: g_map=16;g_st=GS_INTER;break;
	  default: ++g_map;g_st=GS_INTER;break;
	}
	F_freemus();
	if(g_st==GS_INTER) {
	  F_loadmus("INTERMUS");
	}else {F_loadmus("КОНЕЦ");if(mus_vol>0) mus_vol=128;}
	S_startmusic();
  }else if(g_exit==2) {
	switch(g_map) {
	  case 31: g_map=32;g_st=GS_INTER;break;
	  case 32: g_map=16;g_st=GS_INTER;break;
	  default: g_map=31;g_st=GS_INTER;break;
	}
	F_freemus();
	F_loadmus("INTERMUS");
	S_startmusic();
  }
#ifdef DEMO
  if(g_dm && g_time>10920) {g_st=GS_INTER;}
#endif
}

static void drawview(player_t *p) {
  if(p->looky<-50) p->looky=-50;
  else if(p->looky>50) p->looky=50;
  w_x=p->o.x;w_y=p->o.y-12+p->looky;W_draw();PL_drawst(p);
}

static int get_pu_st(int t) {
  if(t>=PL_FLASH) return 1;
  if((t/9)&1) return 0;
  return 1;
}

static void pl_info(player_t *p,int y) {
  dword t;

  t=p->kills*10920/g_time;
  Z_gotoxy(25,y);Z_printbf("УБИЛ");
  Z_gotoxy(25,y+15);Z_printbf("УБИЙСТВ В МИНУТУ");
  Z_gotoxy(25,y+30);Z_printbf("НАШЕЛ СЕКРЕТОВ %u ИЗ %u",p->secrets,sw_secrets);
  Z_gotoxy(255,y);Z_printbf("%u",p->kills);
  Z_gotoxy(255,y+15);Z_printbf("%u.%u",t/10,t%10);
}

void G_draw(void) {
  int h;
  word hr,mn,sc;

  switch(g_st) {
    case GS_ENDANIM: case GS_END2ANIM: case GS_DARKEN:
      return;
    case GS_TITLE:
      V_pic(0,0,scrnh[0]);
      break;
    case GS_ENDSCR:
      V_clr(0,320,0,200,0);V_pic(0,0,scrnh[2]);
      break;
    case GS_INTER:
	  V_pic(0,0,scrnh[1]);
	  Z_gotoxy(60,20);Z_printbf("УРОВЕНЬ ПРОЙДЕН");
	  Z_calc_time(g_time,&hr,&mn,&sc);
	  Z_gotoxy(115,40);Z_printbf("ЗА %u:%02u:%02u",hr,mn,sc);
	  h=60;
	  if(_2pl) {
		Z_gotoxy(80,h);Z_printbf("ПЕРВЫЙ ИГРОК");
		Z_gotoxy(80,h+70);Z_printbf("ВТОРОЙ ИГРОК");
		h+=20;
	  }
	  pl_info(&pl1,h);
	  if(_2pl) pl_info(&pl2,h+70);
	  break;
  }
  if(g_st!=GS_GAME) {
    GM_draw();
    V_copytoscr(0,320,0,200);
    return;
  }
  if(_2pl) {
	w_o=0;drawview(&pl1);
	w_o=100;drawview(&pl2);
  }else{
	w_o=50;drawview(&pl1);
  }
  if(GM_draw()) {
    V_copytoscr(0,320,0,200);
    pl1.drawst=pl2.drawst=0;
    return;
  }
  if(pl1.invl) h=get_pu_st(pl1.invl)*6;
  else if(pl1.pain<15) h=0;
  else if(pl1.pain<35) h=1;
  else if(pl1.pain<55) h=2;
  else if(pl1.pain<75) h=3;
  else if(pl1.pain<95) h=4;
  else h=5;
  if(h) V_maptoscr(0,200,(_2pl)?1:51,98,clrmap+h*256);
  else V_copytoscr(0,200,(_2pl)?1:51,98);
  if(pl1.drawst) V_copytoscr(200,120,(_2pl)?0:50,100);
  pl1.drawst=0;
  if(_2pl) {
	if(pl2.invl) h=get_pu_st(pl2.invl)*6;
	else if(pl2.pain<15) h=0;
    else if(pl2.pain<35) h=1;
    else if(pl2.pain<55) h=2;
    else if(pl2.pain<75) h=3;
    else if(pl2.pain<95) h=4;
    else h=5;
    if(h) V_maptoscr(0,200,101,98,clrmap+h*256);
    else V_copytoscr(0,200,101,98);
    if(pl2.drawst) V_copytoscr(200,120,100,100);
    pl2.drawst=0;
  }
}

void G_respawn_player(player_t *p) {
  int i;

  if(dm_pnum==2) {
    if(p==&pl1) i=dm_pl1p^=1;
    else i=dm_pl2p^=1;
    p->o.x=dm_pos[i].x;p->o.y=dm_pos[i].y;p->d=dm_pos[i].d;
    FX_tfog(dm_pos[i].x,dm_pos[i].y);Z_sound(telepsnd,128);
    return;
  }
  do{i=random(dm_pnum);}while(i==dm_pl1p || i==dm_pl2p);
  p->o.x=dm_pos[i].x;p->o.y=dm_pos[i].y;p->d=dm_pos[i].d;
  if(p==&pl1) dm_pl1p=i; else dm_pl2p=i;
  FX_tfog(dm_pos[i].x,dm_pos[i].y);Z_sound(telepsnd,128);
}
