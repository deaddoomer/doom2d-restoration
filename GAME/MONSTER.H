// Monsters

#define MAXMN 200

enum{
  MN_NONE,MN_DEMON,MN_IMP,MN_ZOMBY,MN_SERG,MN_CYBER,MN_CGUN,
  MN_BARON,MN_KNIGHT,MN_CACO,MN_SOUL,MN_PAIN,MN_SPIDER,MN_BSP,
  MN_MANCUB,MN_SKEL,MN_VILE,MN_FISH,MN_BARREL,MN_ROBO,MN_MAN,
  MN__LAST,
  MN_PL_DEAD=100,MN_PL_MESS
};

void MN_init(void);
void MN_alloc(void);
int MN_spawn(int,int,byte,int);
int MN_spawn_deadpl(obj_t *,byte,int);
void MN_act(void);
void MN_mark(void);
void MN_draw(void);
void MN_warning(int l,int t,int r,int b);
