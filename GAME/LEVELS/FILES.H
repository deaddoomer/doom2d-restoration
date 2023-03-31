// File operations

#include <io.h>
#include <fcntl.h>

#define MAX_WADS 20
#define MAX_WAD 3000

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

#ifdef __cplusplus
extern "C" {
#endif

void F_startup(void);
void F_addwad(char *);
void F_initwads(void);
void F_allocres(void);
void F_readstr(int,char *,int);
void F_readstrz(int,char *,int);
void F_loadres(int,void *,dword,dword);
int F_getresid(char *);
int F_findres(char *);
int F_getsprid(char[4],int,int);
int F_getreslen(int);
void F_loadmap(char *,char *);
int F_savemap(char *,char *);

void F_start_blk(int t,int st);
void F_write_blk(void *ptr,dword len);
void F_end_blk(void);

char **F_make_wall_list(void);
char **F_make_dmm_list(void);
char **F_make_map_list(char *);
char **F_make_file_list(char *,char **);

#ifdef __cplusplus
}
#endif

extern char wads[][80];
extern int wad_num;
