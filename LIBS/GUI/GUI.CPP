#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <av.h>
#include <averr.h>
#include <stdlib.h>
#include <string.h>
#include <vga.h>
#include "gui.h"

#define MAXCOM 50

extern "C" {
vgaimg *mouseshape=&stdarrow;
unsigned char gray[256];
int msg;
}

static void *gui_scr;
static int top_com,mx=160,my=100;
static struct{gui_obj *o;int c;long p;} combuf[MAXCOM];

gui_obj::gui_obj() {next=prev=child=owner=NULL;}

gui_obj::gui_obj(int x,int y,int w,int h) {
  next=prev=child=owner=NULL;
  if(x+w>320) x=320-w;
  if(y+h>200) y=200-h;
  if(x<0) x=0;
  if(y<0) y=0;
  left=x;top=y;width=w;height=h;
}

void gui_obj::redraw(void) {RD_rect(left,width,top,height);}

void gui_obj::update(void) {redraw();GUI_update();}

void gui_obj::draw(void) {
  if(!child) return;
  for(gui_obj *p=child;p->next;p=p->next);
  for(;p;p=p->prev) p->draw();
}

int gui_obj::chkmouse(int x,int y,int b) {
  for(gui_obj *p=child;p;p=p->next) if(p->chkmouse(x,y,b)) return 1;
  x-=left;y-=top;
  if(x<0 || y<0 || x>=width || y>=height) return 0;
  return mouse(x,y,b);
}

#pragma argsused
int gui_obj::mouse(int x,int y,int b) {return 0;}

int gui_obj::key(int k) {
  for(gui_obj *p=child;p;p=p->next) if(p->key(k)) return 1;
  return 0;
}

void gui_obj::command(int c,long p) {
  if(owner) owner->command(c,p);
}

gui_obj& gui_obj::operator +=(gui_obj& a) {
  for(gui_obj *p=&a;p->next;p=p->next) p->owner=owner;
  p->owner=owner;
  p->next=next;next->prev=p;next=&a;a.prev=this;
  return *this;
}

gui_obj& gui_obj::operator <<=(gui_obj& a) {
  for(gui_obj *p=&a;p->next;p=p->next) p->owner=this;
  p->owner=this;
  p->next=child;child->prev=p;child=&a;a.prev=NULL;
  return *this;
}

void gui_obj::remove(void) {
  next->prev=prev;
  if(owner->child==this) {owner->child=next;return;}
  for(gui_obj *p=owner->child;p;p=p->next)
	if(p->next==this) {p->next=next;return;}
}

// frame

frame::frame(int x,int y,int w,int h):gui_obj(x,y,w,h) {
  clrh=10;clrn=6;clrl=2;
}

void frame::draw(void) {
  V_clr(left+2,width-4,top+2,height-4,gray[clrn]);
  V_clr(left,width-2,top,2,gray[clrh]);V_dot(left+width-2,top,gray[clrh]);
  V_clr(left,2,top+2,height-4,gray[clrh]);V_dot(left,top+height-2,gray[clrh]);
  V_clr(left+1,width-1,top+height-2,2,gray[clrl]);V_dot(left,top+height-1,gray[clrl]);
  V_clr(left+width-2,2,top+1,height-3,gray[clrl]);V_dot(left+width-1,top,gray[clrl]);
  gui_obj::draw();
}

// container box

vcontbox::vcontbox(int x,int y,int w,int h,int wc,cont_t *t,int c,int cm)
:frame(x,y,w,h) {
  gui_obj *p;

  cw=wc;cont=t;cur=c;com=cm;
  child=cont[cur].o;
  for(num=0;t->o;++t,++num)
	for(p=t->o;p;p=p->next) p->owner=this;
}

void vcontbox::draw(void) {
  int i,j;

  frame::draw();
  V_clr(left+cw,1,top+2,j=(height-4)*cur/num,gray[clrh]);
  i=(height-4)*(cur+1)/num;
  V_clr(left+2,1,top+2+j,i-j,gray[clrh]);
  V_clr(left+cw,1,top+1+i,height-3-i,gray[clrh]);
  for(i=0;i<num;++i) {
	V_clr(left+2,cw-2,j=(top+1+(height-4)*(i+1)/num),1,gray[(i+1==cur)?clrh:clrl]);
	V_spr(left+cw/2+((i==cur)?1:0),j-height/num/2,(vgaimg*)cont[i].t);
  }
}

int vcontbox::mouse(int x,int y,int b) {
  if(x>=cw) return gui_obj::mouse(x,y,b);
  if(y<2 || y>=height-2) return 1;
  y=(y-2)*num/(height-4);
  if(y==cur) return 1;
  child=cont[cur=y].o;
  redraw();
  send_command(this,com,cur);
  while(I_mget(&x,&y)) GUI_wait();
  return 1;
}

// mdialog

mdialog::mdialog(int x,int y,int w,int h):frame(x,y,w,h) {}

int mdialog::chkmouse(int x,int y,int b) {
  if(gui_obj::chkmouse(x,y,b)) return 1;
  mouse(x-left,y-top,b);
  return 1;
}

// msgbox

msgbox::msgbox(int minw,int minh,char *t,int x,int y)
:mdialog(x-minw/2,y-minh/2,minw,minh) {

  vf_font=&smallfont;vf_step=1;
  text=t;minh+=8;
  for(int w=8;*t;++t) switch(*t) {
	case '\n': minh+=8;
	case '\r':
	  if(w>minw) minw=w;
	  w=8;break;
	default:
	  w+=V_chrlen(*t);
  }if(w>minw) minw=w;
  left=x-minw/2;top=y-minh/2;
  width=minw;height=minh;
}

void msgbox::draw(void) {
  frame::draw();
  vf_font=&smallfont;vf_step=1;
  vf_x=left+4;vf_y=top+4;vf_color=gray[0];
  for(char *p=text;*p;++p) switch(*p) {
	case '\n': vf_y+=8;
	case '\r': vf_x=left+4;break;
	default: V_putch(*p);
  }
}

void msgbox::command(int c,long p) {
  switch(c) {
	case C_CANCEL: case C_OK: case C_YES: case C_NO:
	  msg=c;return;
  }
  gui_obj::command(c,p);
}

// base button class

base_button::base_button(int x,int y,int w,int h,int c,long p,int k):gui_obj(x,y,w,h) {
  clrh=12;clrn=10;clrl=8;
  if(k>='a' && k<='z') k-=32;
  com=c;par=p;hotkey=k;
  pressed=0;
}

void base_button::draw(void) {
  int i;

  i=pressed;
  V_clr(left+1+i,width-2,top+1+i,height-2,gray[clrn]);
  V_clr(left+i,width-1,top+i,1,gray[i?clrl:clrh]);
  V_clr(left+i,1,top+i,height-1,gray[i?clrl:clrh]);
  if(!i) {
	V_clr(left,width,top+height-1,1,gray[clrl]);
	V_clr(left+width-1,1,top,height,gray[clrl]);
  }else{
	V_clr(left,width,top,1,gray[0]);
	V_clr(left,1,top,height,gray[0]);
  }
}

#pragma argsused
int base_button::mouse(int x,int y,int b) {
  pressed=1;redraw();
  while(I_mget(&x,&y)) {
	x-=left;y-=top;
	if(x<0 || x>=width || y<0 || y>=height) {
	  if(pressed) {pressed=0;redraw();}
	}else
	  if(!pressed) {pressed=1;redraw();}
	GUI_wait();
  }
  if(pressed) {
	send_command(this,com,par);
	pressed=0;redraw();
  }return 1;
}

int base_button::key(int k) {
  if(k>='a' && k<='z') k-=32;
  if(k==hotkey) {send_command(this,com,par);return 1;}
  return 0;
}

// text button

button::button(int x,int y,int w,int h,char *s,int c,long p,int k)
:base_button(x,y,w,h,c,p,k) {
  text=s;
  vf_font=&smallfont;vf_step=1;
  textx=(w-V_strlen(s))/2+1;texty=(h-5)/2;
}

void button::draw(void) {
  base_button::draw();
  vf_font=&smallfont;vf_step=1;
  vf_x=left+textx+pressed;vf_y=top+texty+pressed;vf_color=gray[0];
  V_puts(text);
}

// sprite button

sprbutton::sprbutton(int x,int y,int w,int h,vgaimg *s,int c,long p,int k)
:base_button(x,y,w,h,c,p,k) {
  spr=s;
  sprx=left+w/2;spry=top+h/2;
}

void sprbutton::draw(void) {
  V_spr1color(sprx+2,spry+2,spr,gray[0]);
  V_spr(sprx+pressed,spry+pressed,spr);
}

void sprbutton::redraw(void) {
  RD_rect(sprx-spr->sx,spr->w+2,spry-spr->sy,spr->h+2);
}

// menubox

menubox::menubox(int x,int y,menu_t *m):frame(x,y,8,8) {
  int i,w;

  clrh=12;clrn=8;clrl=4;
  mnu=m;
  vf_font=&smallfont;vf_step=1;
  for(i=0;mnu[i].c;++i,height+=8)
	if((w=V_strlen(mnu[i].t)+8)>width) width=w;
  if(left+width>320) left=320-width;
  if(top+height>200) top=200-height;
  line=-1;
}

void menubox::draw(void) {
  int i;

  frame::draw();
  vf_font=&smallfont;vf_step=1;
  for(i=0;mnu[i].c;++i) {
	vf_color=gray[(i==line)?15:0];
	vf_x=left+4;vf_y=top+i*8+4;
	V_puts(mnu[i].t);
  }
}

#pragma argsused
int menubox::mouse(int x,int y,int b) {
  int l;

  while(I_mget(&x,&y)) {
	x-=left;y-=top;
	if(x<4 || x>=width-4 || y<4 || y>=height-4) l=-1;
	  else l=(y-4)/8;
	if(l!=line) {line=l;redraw();}
	GUI_wait();
  }
  if(line>=0) send_command(this,mnu[line].c,0);
  return 1;
}

int menubox::chkmouse(int x,int y,int b) {
  x-=left;y-=top;
  if(x<0 || y<0 || x>=width || y>=height) {
	remove();owner->update();
	while(I_mget(&x,&y)) GUI_wait();
	delete this;return 1;
  }
  return mouse(x,y,b);
}

// base slider class

base_slider::base_slider(long t,long c,int cm) {
  com=cm;total=t;cur=c;adjust();
}

void base_slider::adjust(void) {
  if(cur<0) cur=0;
  if(cur>total) cur=total;
  send_command(this,com,cur);
}

// horizontal slider

hslider::hslider(int x,int y,int w,int h,long t,long c,int cm):base_slider(t,c,cm) {
  left=x;top=y;width=w;height=h;
}

void hslider::draw(void) {
  int x;

  V_clr(left,width,top,height,gray[4]);
  V_clr(left,width-1,top,height-1,gray[12]);
  V_clr(left+1,width-2,top+1,height-2,gray[8]);
  x=(width-height)*cur/(total?total:1)+left+1;
  V_clr(x,height-2,top+1,height-2,gray[6]);
  V_clr(x,height-3,top+1,height-3,gray[12]);
  V_clr(x+1,height-4,top+2,height-4,gray[10]);
}

#pragma argsused
int hslider::mouse(int x,int y,int b) {
  cur=(x-height/2+1)*total/(width-height);
  adjust();redraw();
  while(I_mget(&x,&y)) {
	x-=left;
	if(x<0) x=0;
	if(x>=width) x=width-1;
	cur=(x-height/2+1)*total/(width-height);
	adjust();redraw();
	GUI_wait();
  }
  return 1;
}

// horizontal scroll bar

hscroll::hscroll(int x,int y,int w,int h,long t,long c,int cm,long s)
:gui_obj(x,y,w,h) {
  *this<<=*(sld=new hslider(x+h,y,w-h*2,h,t,c,cm))
	+=*(b1=new button(x,y,h,h,"\x1B",C_SLIDE,-s,0x4B00))
	+=*(b2=new button(x+w-h,y,h,h,"\x1A",C_SLIDE,s,0x4D00));
}

hscroll::~hscroll(void) {
  delete sld;delete b1;delete b2;
}

void hscroll::command(int c,long p) {
  if(c==C_SLIDE) {
	sld->cur+=p;sld->adjust();sld->redraw();
	return;
  }
  gui_obj::command(c,p);
}

// vertical slider

vslider::vslider(int x,int y,int w,int h,long t,long c,int cm):base_slider(t,c,cm) {
  left=x;top=y;width=w;height=h;
}

void vslider::draw(void) {
  int y;

  V_clr(left,width,top,height,gray[4]);
  V_clr(left,width-1,top,height-1,gray[12]);
  V_clr(left+1,width-2,top+1,height-2,gray[8]);
  y=(height-width)*cur/(total?total:1)+top+1;
  V_clr(left+1,width-2,y,width-2,gray[6]);
  V_clr(left+1,width-3,y,width-3,gray[12]);
  V_clr(left+2,width-4,y+1,width-4,gray[10]);
}

#pragma argsused
int vslider::mouse(int x,int y,int b) {
  cur=(y-width/2+1)*total/(height-width);
  adjust();redraw();
  while(I_mget(&x,&y)) {
	y-=top;
	if(y<0) y=0;
	if(y>=height) y=height-1;
	cur=(y-width/2+1)*total/(height-width);
	adjust();redraw();
	GUI_wait();
  }
  return 1;
}

// vertical scroll bar

vscroll::vscroll(int x,int y,int w,int h,long t,long c,int cm,long s)
:gui_obj(x,y,w,h) {
  *this<<=*(sld=new vslider(x,y+w,w,h-w*2,t,c,cm))
	+=*(b1=new button(x,y,w,w,"\x18",C_SLIDE,-s,0x4800))
	+=*(b2=new button(x,y+h-w,w,w,"\x19",C_SLIDE,s,0x5000));
}

vscroll::~vscroll(void) {
  delete sld;delete b1;delete b2;
}

void vscroll::command(int c,long p) {
  if(c==C_SLIDE) {
	sld->cur+=p;sld->adjust();sld->redraw();
	return;
  }
  gui_obj::command(c,p);
}

// string list

strlist::strlist(int x,int y,int w,int h,char **p,int c):frame(x,y,w,h) {
  com=c;newlist(p);
  clrh=2;clrn=0;clrl=8;
}

void strlist::newlist(char **p) {
  list=p;
  cline=-1;tline=0;
  for(lines=0;*p;++lines,++p);
}

void strlist::draw(void) {
  int i,y;

  frame::draw();
  vf_font=&smallfont;vf_step=1;
  for(i=tline,y=4;list[i] && y+9<height;++i,y+=8) {
	vf_color=gray[(i==cline)?15:8];
	vf_x=left+4;vf_y=top+y;
	V_puts(list[i]);
  }
}

#pragma argsused
int strlist::mouse(int x,int y,int b) {
  int l;

  while(I_mget(&x,&y)) {
	x-=left;y-=top;
	if(x<4 || x>=width-4 || y<4 || y>=height-4) l=-1;
	  else l=(y-4)/8+tline;
	if(l>=lines) l=-1;
	if(l!=cline) {cline=l;redraw();}
	GUI_wait();
  }
  if(cline>=0) send_command(this,com,cline);
  return 1;
}

// string list with scroll bar

static int adj1(int a) {return (a>0)?a:0;}

strlist2::strlist2(int x,int y,int w,int h,char **p,int c,long s)
:gui_obj(x,y,w,h) {
  *this<<=*(lst=new strlist(x,y,w-10,h,p,c));
  *this<<=*(sb=new vscroll(x+w-10,y,10,h,adj1(lst->lines-1),0,C_SCROLL,s));
}

strlist2::~strlist2(void) {
  delete sb;delete lst;
}

void strlist2::newlist(char **p) {
  lst->newlist(p);
  sb->sld->total=adj1(lst->lines-1);sb->sld->cur=0;sb->sld->adjust();
  redraw();
}

void strlist2::command(int c,long p) {
  if(c==C_SCROLL) {
	lst->tline=p;lst->redraw();
	return;
  }
  gui_obj::command(c,p);
}

// checkbox

chkbox::chkbox(int x,int y,int w,int h,int c,int s):gui_obj(x,y,w,h) {
  com=c;st=s?1:0;
}

void chkbox::draw(void) {
  V_clr(left,width,top,height,gray[2]);
  V_clr(left+1,width-1,top+1,height-1,gray[8]);
  V_clr(left+1,width-2,top+1,height-2,gray[4]);
  if(st) {
	vf_font=&smallfont;vf_step=1;vf_color=gray[15];
	vf_x=left+width/2-2;vf_y=top+height/2-2;V_putch('√');
  }
}

#pragma argsused
int chkbox::mouse(int x,int y,int b) {
  while(I_mget(&x,&y)) GUI_wait();
  x-=left;y-=top;
  if(x<0 || x>=width || y<0 || y>=height) return 1;
  send_command(this,com,st=!st);
  redraw();
  return 1;
}

// text string

textstr::textstr(int x,int y,int w,int h,char *t,void *f,char c) {
  fnt=vf_font=f;clr=c;
  top=y;height=h;
  vf_step=1;
  left=x-w*(width=V_strlen(txt=t))/2;
}

void textstr::newtext(int x,int y,int w,int h,char *t,void *f,char c) {
  redraw();
  fnt=vf_font=f;clr=c;
  top=y;height=h;
  vf_step=1;
  left=x-w*(width=V_strlen(txt=t))/2;
  redraw();
}

void textstr::draw(void) {
  vf_font=fnt;vf_step=1;vf_color=gray[clr];
  vf_x=left;vf_y=top;V_puts(txt);
}

// input box

inputbox::inputbox(int x,int y,int w,int h,char *b,int s)
:gui_obj(x,y,w,h) {
  sz=s-1;cur=strlen(buf=b);
}

void inputbox::draw(void) {
  V_clr(left,width,top,height,gray[2]);
  V_clr(left+1,width-1,top+1,height-1,gray[8]);
  V_clr(left+1,width-2,top+1,height-2,gray[0]);
  vf_font=&smallfont;vf_step=1;vf_color=gray[12];
  vf_x=left+2;vf_y=top+height/2-2;
  V_puts(buf);
  vf_color=gray[15];V_putch('■');
}

int inputbox::key(int k) {
  if(k==13) return 0;
  if(k==8) {
	if(cur>0) {buf[--cur]=0;redraw();}
	return 1;
  }
  if(k>=0x100) return 0;
  if(cur>=sz) return 1;
  buf[cur++]=k;buf[cur]=0;redraw();
  return 1;
}

// ***

void GUI_close(void) {
  I_close();V_done();
}

static void GUI_draw(void) {
  desktop.draw();
  V_spr(mx,my,mouseshape);
}

static unsigned char getbestcolor(int r,int g,int b) {
  int i,best,m,j,rt,gt,bt;

  if(r<0) r=0; if(r>63) r=63;
  if(g<0) g=0; if(g>63) g=63;
  if(b<0) b=0; if(b>63) b=63;
  rt=(r>=g && r>=b)?2:1;
  gt=(g>=r && g>=b)?2:1;
  bt=(b>=r && b>=g)?2:1;
  best=1;
  m=abs((int)mainpal[1].r-r)*rt+
	abs((int)mainpal[1].g-g)*gt+
	abs((int)mainpal[1].b-b)*bt;
  for(i=2;i<256;++i)
	if(m>(j=abs((int)mainpal[i].r-r)*rt+
			abs((int)mainpal[i].g-g)*gt+
			abs((int)mainpal[i].b-b)*bt)) {best=i;m=j;}
  return best;
}

void GUI_start(void) {
  int i;

  I_init();
  for(i=0;i<16;++i) gray[i]=getbestcolor(i*4,i*4,i*4);
  top_com=0;
  V_init();VP_setall(mainpal);
  gui_scr=RD_init(0,320,0,200);
  I_mget(&mx,&my);
  RD_start(gui_scr,GUI_draw);
  RD_rect(0,320,0,200);
}

void GUI_update(void) {
  I_mget(&mx,&my);
  RD_spr(mx,my,mouseshape);
  RD_end();
  RD_start(gui_scr,GUI_draw);
}

void GUI_wait(void) {
  int b;
  long p;
  gui_obj *o;

  if(I_iskey()) I_getkey();
  while(top_com) {
	b=combuf[0].c;p=combuf[0].p;o=combuf[0].o;
	if(--top_com) memmove(combuf,combuf+1,top_com*sizeof(combuf[0]));
	o->command(b,p);
  }
  GUI_update();
}

void GUI(void) {
  int b;
  long p;
  gui_obj *o;

  if(I_iskey()) desktop.key(I_getkey());
  if((b=I_mget(&mx,&my)&3)!=0) {
	RD_spr(mx,my,mouseshape);
	desktop.chkmouse(mx,my,b);
  }
  while(top_com) {
	b=combuf[0].c;p=combuf[0].p;o=combuf[0].o;
	if(--top_com) memmove(combuf,combuf+1,top_com*sizeof(combuf[0]));
	o->command(b,p);
  }
  GUI_update();
}

void send_command(gui_obj *o,int c,long p) {
  if(top_com>=MAXCOM) return;
  combuf[top_com].c=c;
  combuf[top_com].p=p;
  combuf[top_com++].o=o;
}

#pragma argsused
void def_command(int c,long p) {
  switch(c) {
	case C_CANCEL: case C_OK: case C_YES: case C_NO:
	  msg=c;break;
  }
}


extern "C" int message(int t,char *s,...) {
  int i=46;
  va_list ap;
  static char buf[1024];

  msg=0;
  switch(t) {
	case MB_YNC: i+=34;
	case MB_OKC: case MB_YN: i+=34;
  }
  va_start(ap,s);
  vsprintf(buf,s,ap);
  va_end(ap);
  msgbox m(i,25,buf);
  desktop<<=m;
  i=m.left+m.width/2-i/2+8;
  int y=m.top+m.height-18;
  button bo(i,y,30,11,"О'кей",C_OK,0,13),
		 bc(i+34,y,30,11,"Отмена",C_CANCEL,0,27),
		 by(i,y,30,11,"Да",C_YES,0,'Y'),
		 bn(i+34,y,30,11,"Нет",C_NO,0,'N');
  switch(t) {
	case MB_OKC: m<<=bc;
	case MB_OK: m<<=bo;break;
	case MB_C: bc.left=i;m<<=bc;break;
	case MB_YNC: bc.left+=34;m<<=bc;
	case MB_YN: m<<=bn;m<<=by;break;
  }
  desktop.update();
  while(!msg) GUI();
  m.remove();
  desktop.update();
  return msg;
}
