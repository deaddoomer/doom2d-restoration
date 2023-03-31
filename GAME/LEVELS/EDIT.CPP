#include "glob.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <direct.h>
#include <malloc.h>
#include "vga.h"
#include "view.h"
#include "error.h"
#include "gui.h"
#include "edit.h"
#include "files.h"
#include "memory.h"
#include "misc.h"

#define SW_PL_PRESS		1
#define SW_MN_PRESS		2
#define SW_PL_NEAR		4
#define SW_MN_NEAR		8
#define SW_KEY_R		16
#define SW_KEY_G		32
#define SW_KEY_B		64

enum{EBACK,EWALL,EFRONT,ETHING,ESWITCH};

extern "C" dword dpmi_memavl(void);

extern "C" {
extern int gamma;
extern char f_path[];
extern vgaimg *walp[256];

char musname[8]="";
int curth=-1;
}

char mapdir[_MAX_DIR]="",*mapmask[]={"*.WAD",NULL},mapfile[13]="";

int etype=ETHING,wallimg[3]={-1,-1,-1},wtype=1,pthing=TH_PLR1;
int thtnum,*tht_tlist;
char **thtn_list;
long par;

char *walls[256];

char *wt_name[]={"пусто","стена","закр.дверь","откр.дверь","ступень","вода",
  "кислота1","кислота2","блок. мон.","лифт \x18","лифт \x19",NULL};

static view vw;
static mapwin mw1(213,93);
static sprview *wlsv[3];
static textstr th_cn(115,127,0,5,"",&smallfont,12),
		sw_cn(45,125,0,5,"",&smallfont,12);
static chkbox thf_r(115,135,10,9,E_THF_R,0),
	      thf_dm(115,145,10,9,E_THF_DM,0),
	      swf_pp(25,155,10,9,E_SWF_PP,0),
	      swf_mp(25,165,10,9,E_SWF_MP,0),
	      swf_pn(25,175,10,9,E_SWF_PN,0),
	      swf_mn(25,185,10,9,E_SWF_MN,0),
	      swf_kr(100,155,10,9,E_SWF_KR,0),
	      swf_kg(100,165,10,9,E_SWF_KG,0),
	      swf_kb(100,175,10,9,E_SWF_KB,0);
static strlist2 wl1(25,125,60,70,walls,E_WALLIMG,8),
		wl2(25,125,60,70,walls,E_WALLIMG,8),
		wl3(25,125,60,70,walls,E_WALLIMG,8);

static void about(void) {
  static char s[2000];

  sprintf(s,"Редактор уровней для DOOM'а 2D  V1.30\n\n"
    "Свободно DPMI-памяти: %uK\n\n",dpmi_memavl()>>10);
  TH_info(s+strlen(s));
  SW_info(s+strlen(s));
  sprintf(s+strlen(s),"Музыка: %.8s",musname);
  message(MB_OK,"%s",s);
}

void rebuild_wl(void) {
  char s[9];

  s[8]=0;
  for(int i=0;walls[i];++i) free(walls[i]);
  for(i=0;i<255 && wal[i].n[0];++i) {
	strncpy(s,wal[i].n,8);
	walls[i]=strdup(s);
  }walls[i]=NULL;
  wl1.newlist(walls);wallimg[0]=-1;
  wl2.newlist(walls);wallimg[1]=-1;
  wl3.newlist(walls);wallimg[2]=-1;
}

static void redraw_th(void) {
  th_cn.newtext(115,127,0,5,(curth==-1)?"":TH_getname(TH_gett(curth)),
	&smallfont,12);
  int i=TH_getf(curth);
  thf_r.st=i&THF_DIR;thf_r.redraw();
  thf_dm.st=i&THF_DM;thf_dm.redraw();
}

static void redraw_sw(void) {
  sw_cn.newtext(45,125,0,5,SW_getname(),&smallfont,12);
  byte f=SW_getf();
  swf_pp.st=f&SW_PL_PRESS;swf_pp.redraw();
  swf_mp.st=f&SW_MN_PRESS;swf_mp.redraw();
  swf_pn.st=f&SW_PL_NEAR;swf_pn.redraw();
  swf_mn.st=f&SW_MN_NEAR;swf_mn.redraw();
  swf_kr.st=f&SW_KEY_R;swf_kr.redraw();
  swf_kg.st=f&SW_KEY_G;swf_kg.redraw();
  swf_kb.st=f&SW_KEY_B;swf_kb.redraw();
}

static void options(void) {
  mdialog m(110,70,100,60);
  hslider h1(150,90,50,10,4,gamma,E_GAMMA);
  textstr t1(115,92,0,5,"Яркость",&smallfont,15);
  button b1(130,110,60,15,"О'кей",C_OK,0,13);

  desktop<<=m<<=(h1+=t1+=b1);
  desktop.redraw();
  for(msg=0;!msg;) GUI();
  m.remove();desktop.redraw();
}

static void near add_wall(void) {
  static char buf[9]="";
  mdialog m(60,45,200,110);
  inputbox ib(195,50,60,10,buf,9);
  char **p=F_make_wall_list();
  strlist2 lst(195,65,60,70,p,C_YES,8);
  sprview sv1(64,68,128,64,NULL);
  button b1(194,140,30,10,"О'кей",C_OK,0,13),
	 b2(226,140,30,10,"Не надо",C_CANCEL,0,27);

  desktop<<=m<<=(b1+=b2+=ib+=lst+=sv1);
  desktop.redraw();
  int lc=-1;
  for(msg=0;!msg || msg==C_YES;) {
    GUI();
    if(msg==C_YES) {
      ib.cur=strlen(strcpy(buf,p[par]));ib.redraw();
    }
    if(lc!=ib.cur || msg==C_YES) {
      M_unlock(sv1.img);
      sv1.img=(vgaimg*)M_lock(F_findres(buf));sv1.redraw();
      lc=ib.cur;
    }
    if(msg==C_YES) msg=0;
  }
  m.remove();desktop.redraw();
  Z_freelist(p);
  if(msg==C_CANCEL || buf[0]==0) return;
  W_addwall(buf);rebuild_wl();
}

static void set_music(void) {
  static char buf[9]="";
  mdialog m(125,45,70,110);
  inputbox ib(130,50,60,10,buf,9);
  char **p=F_make_dmm_list();
  strlist2 lst(130,65,60,70,p,C_YES,8);
  button b1(129,140,30,10,"О'кей",C_OK,0,13),
		 b2(161,140,30,10,"Не надо",C_CANCEL,0,27);

  memcpy(buf,musname,8);buf[8]=0;
  desktop<<=m<<=(b1+=b2+=ib+=lst);
  desktop.redraw();
  for(msg=0;!msg || msg==C_YES;) {
    GUI();if(msg==C_YES) {
      msg=0;ib.cur=strlen(strcpy(buf,p[par]));ib.redraw();
    }
  }
  m.remove();desktop.redraw();
  Z_freelist(p);
  if(msg==C_CANCEL) return;
  memset(musname,0,8);
  strncpy(musname,buf,8);
}

static char *choose_map(char *fn,char *title) {
  static char buf[9]="MAP01";
  mdialog m(125,35,70,120);
  inputbox ib(130,50,60,10,buf,9);
  char **p=F_make_map_list(fn);
  strlist2 lst(130,65,60,70,p,C_YES,8);
  button b1(129,140,30,10,"О'кей",C_OK,0,13),
	 b2(161,140,30,10,"Не надо",C_CANCEL,0,27);
  textstr t1(160,40,1,5,title,&smallfont,15);

  buf[8]=0;
  desktop<<=m<<=(b1+=b2+=ib+=lst+=t1);
  desktop.redraw();
  for(msg=0;!msg || msg==C_YES;) {
    GUI();if(msg==C_YES) {
      msg=0;ib.cur=strlen(strcpy(buf,p[par]));ib.redraw();
    }
  }
  m.remove();desktop.redraw();
  Z_freelist(p);
  if(msg==C_CANCEL) return NULL;
  return buf;
}

int choose_file(char *dir,char **mask,char *file,char *title) {
  static char str[14],cdir[_MAX_DIR];
  mdialog m(10,10,300,180);
  char **p=F_make_file_list(dir,mask);
  strlist2 lst(115,50,90,120,p,C_YES,10);
  inputbox ib(115,35,90,10,file,13);
  button b1(115,175,40,10,"О'кей",C_OK,0,13),
		 b2(165,175,40,10,"Не надо",C_CANCEL,0,27);
  textstr t1(160,15,1,5,title,&smallfont,15),
	  tdir(160,25,1,5,dir,&smallfont,12);

  getcwd(cdir,_MAX_DIR);
  chdir(dir);
  desktop<<=m<<=(b1+=b2+=ib+=lst+=t1+=tdir);
  desktop.redraw();
  for(msg=0;msg!=C_OK && msg!=C_CANCEL;) {
	GUI();
	if(msg==C_YES) {
	  msg=0;
	  if(p[par][0]=='<') {
		strcpy(str,p[par]+1);str[strlen(str)-1]=0;
		chdir(str);getcwd(dir,_MAX_DIR);
		Z_freelist(p);
		lst.newlist(p=F_make_file_list(dir,mask));
		*file=0;ib.cur=0;ib.redraw();
		tdir.newtext(160,25,1,5,dir,&smallfont,12);
	  }else{
		ib.cur=strlen(strcpy(file,p[par]));ib.redraw();
	  }
	}
  }
  m.remove();desktop.redraw();
  Z_freelist(p);
  chdir(cdir);
  if(msg==C_CANCEL || file[0]==0) return 0;
  if(f_path[strlen(strcpy(f_path,dir))-1]!='\\') strcat(f_path,"\\");
  strcat(f_path,file);
  return 1;
}

// desktop

struct desk:gui_obj{
  desk();
  virtual void draw(void);
  virtual int key(int);
  virtual void command(int,long);
};

desk::desk():gui_obj(0,0,320,200) {}

void desk::draw(void) {
  V_clr(200,120,0,200,gray[5]);
  gui_obj::draw();
}

int desk::key(int k) {
  if(k==0x4400) {
	send_command(this,E_QUIT,0);
	return 1;
  }
  return gui_obj::key(k);
}

void desk::command(int c,long p) {
  int i;
  char *s;
  static menu_t sysmnu[]={
	"Опции",E_OPTIONS,"Справка",E_ABOUT,"Очистка",E_NEWMAP,
	"Выход",E_QUIT,
	NULL,0
  };
  static byte swf[7]={SW_PL_PRESS,SW_MN_PRESS,SW_PL_NEAR,SW_MN_NEAR,
    SW_KEY_R,SW_KEY_G,SW_KEY_B};

  switch(c) {
	case E_DBACK: w_drawb=p;vw.redraw();break;
	case E_DTYPE: w_drawt=p;vw.redraw();break;
	case E_DFRONT: w_drawf=p;vw.redraw();break;
	case E_DTHING: w_drawth=p;vw.redraw();break;
	case E_DSWITCH: w_drawsw=p;vw.redraw();break;
	case E_SYSTEM: desktop<<=*new menubox(305,2,sysmnu);desktop.redraw();break;
	case E_QUIT:
	  if(message(MB_YN,"   Уже уходите?")==C_YES) ERR_quit();
	  break;
	case E_OPTIONS: options();break;
	case E_GAMMA: setgamma(p);break;
	case E_ETYPE: send_command(&vw,E_DBACK+p,1);etype=p;break;
	case E_WALLIMG: if(etype<3) {
	    wallimg[etype]=p+1;
	    wlsv[etype]->img=(p>=-1)?walp[p+1]:NULL;wlsv[etype]->redraw();
	    if(p<=-2) {wl2.lst->cline=-1;wl2.redraw();}
	  }break;
	case E_WTYPE: wtype=p+1;break;
	case E_THTYPE: pthing=tht_tlist[p];break;
	case E_ADDWALL: add_wall();break;
	case E_NEXTSW: SW_nexttype();redraw_sw();desktop.redraw();break;
	case E_PREVSW: SW_prevtype();redraw_sw();desktop.redraw();break;
	case E_DELSW: SW_delete(cursw);redraw_sw();desktop.redraw();break;
	case E_THF_R: if(curth==-1) break;
	  TH_setf(curth,(TH_getf(curth)&(0xFFFF-THF_DIR))|(p?THF_DIR:0));
	  vw.redraw();break;
	case E_THF_DM: if(curth==-1) break;
	  TH_setf(curth,(TH_getf(curth)&(0xFFFF-THF_DM))|(p?THF_DM:0));
	  break;
	case E_SWF_PP: case E_SWF_MP: case E_SWF_PN: case E_SWF_MN:
	case E_SWF_KR: case E_SWF_KG: case E_SWF_KB:
	  if(cursw==-1) break;
	  i=swf[c-E_SWF_PP];
	  SW_setf((SW_getf()&(0xFF-i))|(p?i:0));
	  break;
	case E_MUSIC:
	  set_music();
	  break;
	case E_NEWMAP:
	  if(message(MB_YN,"Стереть весь уровень?")!=C_YES) break;
	  if(message(MB_YN,"Вы уверены?")!=C_YES) break;
	  W_init();rebuild_wl();curth=-1;redraw_th();redraw_sw();
	  for(i=0;i<3;++i) wlsv[i]->img=NULL;
	  desktop.redraw();
	  break;
	case E_LOADMAP:
	  if(!choose_file(mapdir,mapmask,mapfile,"ЗАГРУЗКА КАРТЫ")) break;
	  if(!(s=choose_map(f_path,"ЗАГРУЗКА КАРТЫ"))) break;
	  W_init();F_loadmap(f_path,s);rebuild_wl();curth=-1;redraw_th();
	  for(i=0;i<3;++i) wlsv[i]->img=NULL;
	  desktop.redraw();
	  break;
	case E_SAVEMAP:
	  if(!choose_file(mapdir,mapmask,mapfile,"ЗАПИСЬ КАРТЫ")) break;
//	  if(access(f_path,0)==0)
//		if(message(MB_YN,"Стереть старый %s ?",f_path)!=C_YES) break;
	  if(!(s=choose_map(f_path,"ЗАПИСЬ КАРТЫ"))) break;
	  if(!F_savemap(f_path,s))
	    message(MB_OK,"ВНИМАНИЕ!!!\n\nПроизошла ошибка при записи %s в файл\n"
	      "%s",s,f_path);
	  break;
	case E_ABOUT:
	  about();
	  break;
	default:
	  msg=c;par=p;break;
//	  def_command(c,p);
  }
}

static desk dsk;
gui_obj& desktop=dsk;

// view

view::view():gui_obj(0,0,250,110) {
  *this<<=*(hs=new hscroll(0,100,200,10,FLDW*CELW-200,FLDW*CELW/2,E_VIEWX,8))
	+=*(vs=new vscroll(200,0,10,100,FLDH*CELH-98,FLDH*CELH/2,E_VIEWY,8));
  for(int i=0;i<5;++i)
	*this<<=*(cb[i]=new chkbox(215,i*15+10,10,9,E_DBACK+i,1));
  cb[1]->st=0;
}

view::~view() {delete hs;delete vs;}

void view::draw(void) {
  static vgaimg *s[5]=
	{&back_icon,&wall_icon,&front_icon,&thing_icon,&switch_icon};

  W_draw();gui_obj::draw();
  for(int i=0;i<5;++i)
	V_spr(240,i*15+14,s[i]);
}

void view::command(int c,long p) {
  switch(c) {
	case E_VIEWX: w_x=p+100;W_adjust();redraw();break;
	case E_VIEWY: w_y=p+49;W_adjust();redraw();break;
	case E_DBACK: case E_DTYPE: case E_DFRONT:
	case E_DTHING: case E_DSWITCH:
	  cb[c-E_DBACK]->st=p;
	  redraw();
	default: gui_obj::command(c,p);
  }
}

int view::mouse(int x,int y,int b) {
  int i,lx,ly,sx,sy;
  byte *p;

  if(x>=200 || y>=99 || y==0) return gui_obj::mouse(x,y,b);
  switch(etype) {
	case EBACK: case EWALL: case EFRONT:
	  if(b!=1 && b!=2) break;
	  edt=b;edx1=edx2=(x+w_x-100)/8;edy1=edy2=(y+w_y-50)/8;
	  redraw();
	  while(I_mget(&x,&y)&b) {
		if(x>=200) x=199;
		if(y==0) y=1;
		if(y>=99) y=98;
		x=(x+w_x-100)/8;y=(y+w_y-50)/8;
		if(x!=edx2 || y!=edy2) {edx2=x;edy2=y;redraw();}
		GUI_wait();
	  }
	  if(edx2<edx1) {i=edx1;edx1=edx2;edx2=i;}
	  if(edy2<edy1) {i=edy1;edy1=edy2;edy2=i;}
	  switch(etype) {
		case EWALL: if(wtype==2 || (wtype>=5 && wtype<=7)) {p=(byte*)fldf;break;}
		case EBACK: p=(byte*)fldb;break;
		case EFRONT: p=(byte*)fldf;break;
	  }
	  if(b==1) {i=wallimg[etype];b=wtype;}
	    else{b=0;i=(wallimg[etype]>=0)?0:-1;}
	  sx=sy=1;
	  if(i>0) if(((unsigned)walp[i]) > 16)
	    {sx=(walp[i]->w+7)/8;sy=(walp[i]->h+7)/8;}
	  for(edx2=edx2-edx1+1,ly=0;edy1<=edy2;++edy1,++ly) {
	    if(i>0) {
	      if(ly%sy==0)
	        for(lx=0;lx<edx2;lx+=sx)
	          p[edy1*FLDW+edx1+lx]=i;
	    }else if(i==0) memset(p+edy1*FLDW+edx1,0,edx2);
	    if(etype==EWALL) memset(&fld[edy1][edx1],b,edx2);
	  }
	  edt=0;redraw();mw1.redraw();
	  break;
	case ETHING:
	  if((i=TH_isthing(w_x-100+x,w_y-50+y))!=-1) {
		curth=i;
		if(b&1) {
		  lx=TH_getx(curth);ly=TH_gety(curth);
		  sx=x;sy=y;
		  while((b=I_mget(&x,&y))&1) {
			if(b&2) break;
			TH_move(curth,lx+x-sx,ly+y-sy);
			vw.redraw();
			GUI_wait();
		  }
		  if((b&2) || x>=200 || y==0 || y>=99) TH_move(curth,lx,ly);
		  else TH_move(curth,lx+x-sx,ly+y-sy);
		}else {TH_delete(curth);curth=-1;}
	  }else if(b&2) curth=TH_add(w_x-100+x,w_y-50+y,pthing);
	  redraw_th();
	  desktop.redraw();
	  while(I_mget(&x,&y)) GUI_wait();
	  break;
	case ESWITCH:
	  if((i=SW_ishere((w_x-100+x)>>3,(w_y-50+y)>>3))!=-1) {
	    if((b&3)==1) {cursw=i;redraw_sw();desktop.redraw();}
	  }else if((b&3)==3) {
	    SW_add((w_x-100+x)>>3,(w_y-50+y)>>3);
	    redraw_sw();desktop.redraw();
	    while((I_mget(&x,&y)&3)==3) GUI_wait();
	  }
	  if((b&3)==2) {
	    SW_setlink((w_x-100+x)>>3,(w_y-50+y)>>3);
	    desktop.redraw();
	    while(I_mget(&x,&y)&2) GUI_wait();
	  }
	  break;
  }
  return 1;
}

// map window

mapwin::mapwin(int x,int y):frame(x,y,104,104) {
  clrh=2;clrn=0;clrl=8;
}

void mapwin::draw(void) {
  frame::draw();
  Z_drawmap(left+2,top+2);
  SW_draw_mini(left+2,top+2);
}

int mapwin::mouse(int x,int y,int b) {
  x-=2;y-=2;
  if(x>=100 || y>=100 || x<0 || y<0) return 1;
  while(I_mget(&x,&y)) {
    x-=left+2;y-=top+2;
    if(x<0) x=0;
    if(y<0) y=0;
    if(x>=100) x=99;
    if(y>=100) y=99;
    vw.hs->sld->cur=x*8-100;vw.hs->sld->adjust();
    vw.vs->sld->cur=y*8-49;vw.vs->sld->adjust();
    GUI_wait();
  }
  return 1;
}

// sprite viewer

sprview::sprview(int x,int y,int w,int h,vgaimg *v,char c):gui_obj(x,y,w,h) {
  color=c;img=v;
}

void sprview::draw(void) {
  short w,h;

  w=scrw;h=scrh;
  if(scrx>=left+width) return;
  if(scry>=top+height) return;
  if(scrx+w>left+width) scrw=left+width-scrx;
  if(scry+h>top+height) scrh=top+height-scry;
  V_clr(left,width,top,height,gray[color]);
  if(img) if(img->w<=320 && img->h<=200) V_spr(left+img->sx,top+img->sy,img);
  scrw=w;scrh=h;
}

extern "C" void edit(void) {
  static textstr t1(115,115,1,5,"ФОН",&smallfont,15),
		 t2(115,115,1,5,"СТЕНЫ",&smallfont,15),
		 t3(115,115,1,5,"ПЕРЕДНИЙ ПЛАН",&smallfont,15),
		 t4(115,115,1,5,"ВЕЩИ, МОНСТРЫ И Т.П.",&smallfont,15),
		 t5(115,115,1,5,"КНОПКИ",&smallfont,15),
		 tht1(130,137,0,5,"вправо",&smallfont,15),
		 tht2(130,147,0,5,"только в DM",&smallfont,15),
		 swt1(40,157,0,5,"игрок наж.",&smallfont,15),
		 swt2(40,167,0,5,"монстр наж.",&smallfont,15),
		 swt3(40,177,0,5,"игрок близко",&smallfont,15),
		 swt4(40,187,0,5,"монстр близко",&smallfont,15),
		 swt5(115,157,0,5,"красный ключ",&smallfont,15),
		 swt6(115,167,0,5,"зеленый ключ",&smallfont,15),
		 swt7(115,177,0,5,"синий ключ",&smallfont,15);
  static button baw1(145,125,60,10,"Добавить",E_ADDWALL,0,-1),
		baw2(155,125,50,10,"Добавить",E_ADDWALL,0,-1),
		baw3(145,125,60,10,"Добавить",E_ADDWALL,0,-1),
		bsw1(25,123,9,9,"\x1B",E_PREVSW,0,-1),
		bsw2(34,123,9,9,"\x1A",E_NEXTSW,0,-1),
		bsw3(25,140,40,9,"Убрать",E_DELSW,0,-1),
		bnw1(25,114,9,9,"",E_WALLIMG,-2,-1);
  static strlist2 thl1(25,125,85,70,thtn_list,E_THTYPE,8);
  static cont_t cnt1[]={
	&t1,&back_icon,
	&t2,&build_icon,
	&t3,&front_icon,
	&t4,&thing_icon,
	&t5,&switch_icon,
	NULL,NULL
  };
  static strlist2 wtl1(90,125,60,70,wt_name+1,E_WTYPE,4);
  static sprbutton b1(305,2,13,10,&system_icon,E_SYSTEM,0,-1),
		   b2(270,10,16,15,&load_icon,E_LOADMAP,0,0x3D00),
		   b3(270,40,16,15,&save_icon,E_SAVEMAP,0,0x3C00);
  static vcontbox vc1(0,110,210,90,20,cnt1,ETHING,E_ETYPE);
  static button b4(265,70,36,15,"Музыка",E_MUSIC,0,-1);

  logo("GUI_start: setting up Graphics User Interface\n");
  int i,t;
  for(thtnum=1,t=TH_PLR2;t!=TH_PLR1;++thtnum,t=TH_nextt(t));
  if(!(thtn_list=(char**)malloc((thtnum+1)*4)))
	ERR_failinit("  Not enough memory");
  if(!(tht_tlist=(int*)malloc(thtnum*4)))
	ERR_failinit("  Not enough memory");
  for(t=TH_PLR1,i=0;i<thtnum;++i,t=TH_nextt(t)) {
	thtn_list[i]=TH_getname(tht_tlist[i]=t);
  }thtn_list[i]=NULL;
  thl1.newlist(thtn_list);thl1.lst->cline=0;
  getcwd(mapdir,_MAX_DIR);
  w_x=FLDW*CELW/2+100;w_y=FLDH*CELH/2+49;
  rebuild_wl();
  GUI_start();setgamma(gamma);
  desktop<<=vw+=vc1+=mw1+=b1+=b2+=b3+=b4;
  wtl1.lst->cline=0;
  t1+=wl1+=baw1+=*(wlsv[0]=new sprview(145,140,60,55,NULL));
  t2+=wl2+=wtl1+=baw2+=bnw1+=*(wlsv[1]=new sprview(155,140,50,55,NULL));
  t3+=wl3+=baw3+=*(wlsv[2]=new sprview(145,140,60,55,NULL));
  t4+=thl1+=th_cn+=thf_r+=thf_dm+=tht1+=tht2;
  t5+=bsw1+=bsw2+=bsw3+=sw_cn+=swt1+=swt2+=swt3+=swt4+=swt5+=swt6+=swt7
    +=swf_pp+=swf_mp+=swf_pn+=swf_mn+=swf_kr+=swf_kg+=swf_kb;
  for(;;) GUI();
}
