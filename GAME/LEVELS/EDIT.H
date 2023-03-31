// editor

#ifdef __cplusplus

enum{
  E_VIEWX=C_USER,E_VIEWY,
  E_DBACK,E_DTYPE,E_DFRONT,E_DTHING,E_DSWITCH,
  E_SYSTEM,E_QUIT,E_OPTIONS,E_GAMMA,
  E_ETYPE,E_WALLIMG,E_WTYPE,
  E_ADDWALL,E_LOADMAP,E_SAVEMAP,E_ABOUT,
  E_THTYPE,E_THF_R,E_THF_DM,
  E_MUSIC,
  E_NEXTSW,E_PREVSW,E_DELSW,
  E_SWF_PP,E_SWF_MP,E_SWF_PN,E_SWF_MN,E_SWF_KR,E_SWF_KG,E_SWF_KB,
  E_NEWMAP,E_SETSKY,E_HELP
};

enum{
  H_NONE=0,H_MAIN,H_BACK,H_FRONT,H_WALL,H_THING,H_SWITCH,H_SWTYPE
};

struct view:gui_obj{
  hscroll *hs;
  vscroll *vs;
  chkbox *cb[5];
  view();
  ~view();
  virtual void draw(void);
  virtual void command(int,long);
  virtual int mouse(int,int,int);
};

struct mapwin:frame{
  mapwin(int x,int y);
  virtual void draw(void);
  virtual int mouse(int,int,int);
};

struct sprview:gui_obj{
  vgaimg *img;
  char color;
  sprview(int x,int y,int w,int h,vgaimg *,char c=6);
  virtual void draw(void);
};

extern "C" {
#endif

void edit(void);

extern vgaimg back_icon,wall_icon,front_icon,thing_icon;
extern vgaimg switch_icon,build_icon,system_icon;
extern vgaimg load_icon,save_icon,help_icon;

#ifdef __cplusplus
}
#endif

extern int edx1,edy1,edx2,edy2,edt;
extern int etype,wallimg[3],pthing;
