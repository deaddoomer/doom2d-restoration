// thing functions

#define MAXTH 500

#ifdef __cplusplus
extern "C" {
#endif

void TH_alloc(void);
void TH_init(void);

void TH_load_old(int);
int TH_load(int h);
void TH_save(void);

void TH_draw(void);
void TH_drawth(int x,int y,int t);
int TH_add(int,int,int);
void TH_move(int,int,int);
int TH_isthing(int,int);
void TH_delete(int);
int TH_getx(int);
int TH_gety(int);
int TH_getf(int);
int TH_gett(int);
void TH_setf(int,int);
void TH_sett(int,int);
char *TH_getname(int);
int TH_nextt(int);
int TH_prevt(int);
void TH_info(char*);

#ifdef __cplusplus
}
#endif

#pragma pack(1)

typedef struct{
  short x;
  short y;
  short t;
  word f;
}thing_t;

#pragma pack()
