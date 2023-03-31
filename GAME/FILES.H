// File operations

#include <io.h>
#include <fcntl.h>

#define MAX_WADS 20
#define MAX_WAD 2000

typedef char wadname[8];

typedef struct {
  int o,l;
  char n[8];
}wad_t;

typedef struct {
  int o,l;
  char n[8];
  int f;
}mwad_t;

void F_startup(void);
void F_addwad(char *);
void F_initwads(void);
void F_allocres(void);
//void F_preload(void);
void F_readstr(int,char *,int);
void F_readstrz(int,char *,int);
void F_loadres(int,void *,dword,dword);
int F_getresid(char *);
void F_getresname(char *,int);
int F_findres(char *);
int F_getsprid(char[4],int,int);
int F_getreslen(int);
void F_set_snddrv(void);
void F_loadmap(char[8]);
void F_loadmus(char[8]);
void F_freemus(void);

extern char wads[][80];
extern int wad_num;
