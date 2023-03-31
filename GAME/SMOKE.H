// Smoke

#define MAXSMOK 500

void SMK_init(void);
void SMK_alloc(void);
void SMK_act(void);
void SMK_draw(void);
void SMK_add(int x,int y,int xv,int yv,byte t,byte s,short o);
void SMK_gas(int x,int y,int rx,int ry,int xv,int yv,int k);
void SMK_flame(int x,int y,int ox,int oy,int rx,int ry,int xv,int yv,int k,int o);
