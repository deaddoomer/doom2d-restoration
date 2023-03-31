// switch functions

#define MAXSW 100

enum{
  SW_NONE,SW_EXIT,SW_EXITS,SW_OPENDOOR,SW_SHUTDOOR,SW_SHUTTRAP,
  SW_DOOR,SW_DOOR5,SW_PRESS,SW_TELE,SW_SECRET,SW_LIFTUP,SW_LIFTDOWN,
  SW_TRAP,SW_LIFT,
  SW_LAST
};

#pragma pack(1)

typedef struct{
  byte x,y;
  byte t,tm;
  byte a,b;
  word c;
  byte f;
}switch_t;

#ifdef __cplusplus
extern "C" {
#endif

void SW_init(void);

void SW_load_old(int);
int SW_load(int h);
void SW_save(void);

void SW_draw(void);
int SW_add(byte,byte);
void SW_setlink(byte,byte);
void SW_delete(int);
void SW_select(int);
int SW_ishere(byte,byte);
void SW_nexttype(void);
void SW_prevtype(void);
char *SW_getname(void);
char *SW_gethelp(void);
void SW_info(char *);
void SW_setf(byte);
byte SW_getf(void);

void SW_draw_mini(int,int);

extern int cursw;

#ifdef __cplusplus
}
#endif

#pragma pack()
