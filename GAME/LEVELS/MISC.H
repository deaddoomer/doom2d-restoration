// Miscellaneous functions

#ifdef __cplusplus
extern "C" {
#endif

void Z_drawfld(byte *);
void Z_drawfldt(byte *);
void Z_drawmap(int x,int y);
void Z_dot(int,int,byte);
void Z_drawspr(int,int,void *,char);
void Z_drawmanspr(int,int,void *,char,byte);
char **Z_makelist(void);
void Z_freelist(char **);

#ifdef __cplusplus
}
#endif
