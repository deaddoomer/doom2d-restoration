// View functions

// object data structure
typedef struct{
  int x,y;		// coordinates
  int xv,yv;		// velocity
  int vx,vy;
  int r,h;		// radius, height
}obj_t;

typedef struct{
  int x,y;
  byte d;
}pos_t;

enum{HIT_SOME,HIT_ROCKET,HIT_BFG,HIT_TRAP,HIT_WATER,HIT_ELECTRO,HIT_FLAME};

enum{
  GS_TITLE,GS_GAME,GS_INTER,GS_DARKEN,GS_ENDANIM,GS_END2ANIM,GS_ENDSCR,
  GS_BVIDEO,GS_EVIDEO,GS_END3ANIM
};

#define FLDW 100
#define FLDH 100
#define CELW 8
#define CELH 8
#define MAXTXW 16
#define MAXTXH 8

#pragma pack(1)

typedef struct{
  char n[8];
  char t;
}wall_t;

#pragma pack()

void W_draw(void);
void W_act(void);

void W_init(void);

int W_load(int);
int G_load(int);

void G_init(void);
void G_start(void);
void G_act(void);
void G_draw(void);

extern byte walswp[256];
extern byte g_bot,_2pl,g_dm,g_st,g_exit,w_horiz,g_map;
extern int g_sttm;
extern dword g_time;
extern int w_ht;
extern int w_o,w_x,w_y;
extern byte fldb[FLDH][FLDW];
extern byte fldf[FLDH][FLDW];
extern byte fld[FLDH][FLDW];
extern pos_t dm_pos[];
extern int dm_pnum,dm_pl1p,dm_pl2p;
