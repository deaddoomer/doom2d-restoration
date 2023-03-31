// Graphics User Interface

#pragma library("gui.lib")

#ifdef __cplusplus
extern "C" {
#endif

// Commands
enum{C_CANCEL=-1,C_NONE=0,C_OK,C_YES,C_NO,C_SLIDE,C_SCROLL,C_USER};

typedef struct{
  char *t;
  int c;
}menu_t;

#ifdef __cplusplus

}

// Main class
struct gui_obj{
  int left,top,width,height;
  gui_obj *next,*prev,*child,*owner;
  gui_obj();
  gui_obj(int x,int y,int w,int h);
  virtual void draw(void);
  virtual void redraw(void);
  virtual void update(void);
  virtual int chkmouse(int x,int y,int b);
  virtual int mouse(int x,int y,int b);
  virtual int key(int k);
  virtual void command(int,long);
  virtual gui_obj& operator +=(gui_obj&);
  virtual gui_obj& operator <<=(gui_obj&);
  virtual void remove(void);
};

typedef struct{
  gui_obj *o;
  void *t;
}cont_t;

// base button class
struct base_button:gui_obj{
  char pressed;
  int com,hotkey;
  int clrl,clrn,clrh;
  long par;
  base_button(int x,int y,int w,int h,int c,long p,int k);
  virtual void draw(void);
  virtual int mouse(int x,int y,int b);
  virtual int key(int k);
};

// text button
struct button:base_button{
  char *text;
  int textx,texty;
  button(int x,int y,int w,int h,char *s,int c,long p,int k);
  virtual void draw(void);
};

// sprite button
struct sprbutton:base_button{
  vgaimg *spr;
  int sprx,spry;
  sprbutton(int x,int y,int w,int h,vgaimg *s,int c,long p,int k);
  virtual void draw(void);
  virtual void redraw(void);
};

// frame
struct frame:gui_obj{
  int clrl,clrn,clrh;
  frame(int x,int y,int w,int h);
  virtual void draw(void);
};

// modal dialog
struct mdialog:frame{
  mdialog(int x,int y,int w,int h);
  virtual int chkmouse(int x,int y,int b);
};

// message box
struct msgbox:mdialog{
  char *text;
  msgbox(int minw,int minh,char *,int x=160,int y=100);
  virtual void draw(void);
  virtual void command(int,long);
};

// container box
struct vcontbox:frame{
  cont_t *cont;
  int cw,cur,com,num;
  vcontbox(int x,int y,int w,int h,int cw,cont_t *,int cur,int cmd);
  virtual void draw(void);
  virtual int mouse(int x,int y,int b);
};

// menu box
struct menubox:frame{
  menu_t *mnu;
  int line;
  menubox(int x,int y,menu_t *);
  virtual void draw(void);
  virtual int mouse(int x,int y,int b);
  virtual int chkmouse(int x,int y,int b);
};

// base slider class
struct base_slider:gui_obj{
  long total,cur;
  int com;
  base_slider(long t,long c,int cmd);
  virtual void adjust(void);
};

// horizontal slider
struct hslider:base_slider{
  hslider(int x,int y,int w,int h,long t,long c,int cmd);
  virtual void draw(void);
  virtual int mouse(int x,int y,int b);
};

// horizontal scroll bar
struct hscroll:gui_obj{
  hslider *sld;
  button *b1,*b2;
  hscroll(int x,int y,int w,int h,long t,long c,int cmd,long step);
  virtual ~hscroll(void);
  virtual void command(int,long);
};

// vertical slider
struct vslider:base_slider{
  vslider(int x,int y,int w,int h,long t,long c,int cmd);
  virtual void draw(void);
  virtual int mouse(int x,int y,int b);
};

// vertical scroll bar
struct vscroll:gui_obj{
  vslider *sld;
  button *b1,*b2;
  vscroll(int x,int y,int w,int h,long t,long c,int cmd,long step);
  virtual ~vscroll(void);
  virtual void command(int,long);
};

// string list
struct strlist:frame{
  int com,lines,tline,cline;
  char **list;
  strlist(int x,int y,int w,int h,char **,int cmd);
  virtual void newlist(char **);
  virtual void draw(void);
  virtual int mouse(int x,int y,int b);
};

// string list with scroll bar
struct strlist2:gui_obj{
  vscroll *sb;
  strlist *lst;
  strlist2(int x,int y,int w,int h,char **,int cmd,long step);
  virtual ~strlist2(void);
  virtual void newlist(char **);
  virtual void command(int,long);
};

// checkbox
struct chkbox:gui_obj{
  int com;
  char st;
  chkbox(int x,int y,int w,int h,int c,int s);
  virtual void draw(void);
  virtual int mouse(int x,int y,int b);
};

// text string
struct textstr:gui_obj{
  char *txt,clr;
  void *fnt;
  textstr(int x,int y,int cntr,int h,char *t,void *f,char c);
  virtual void newtext(int x,int y,int cntr,int h,char *,void *,char c);
  virtual void draw(void);
};

// input box
struct inputbox:gui_obj{
  char *buf;
  int sz,cur;
  inputbox(int x,int y,int w,int h,char *buf,int sz);
  virtual void draw(void);
  virtual int key(int);
};

extern "C" {

#endif

void I_init(void);
void I_close(void);
void I_mon(void);
void I_moff(void);
int I_mget(int *x,int *y);
int I_iskey(void);
int I_getkey(void);

void GUI_start(void); // init GUI
void GUI(void);       // run GUI
void GUI_wait(void);  // poll mouse, keys, but don't do anything
void GUI_close(void); // close GUI
void GUI_update(void); // update screen

enum{MB_OK,MB_C,MB_OKC,MB_YN,MB_YNC};

int message(int type,char *format,...);

#ifdef __cplusplus

}

void def_command(int,long);

void send_command(gui_obj *,int,long);

extern "C" {

#endif

extern vgaimg stdarrow; // cursor image
extern int smallfont; // font
extern vgapal mainpal; // GUI palette

extern int msg;
extern unsigned char gray[256];
extern vgaimg *mouseshape;

#ifdef __cplusplus
}
extern gui_obj& desktop;
#endif
