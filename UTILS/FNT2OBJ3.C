#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned long dword;

int vgah,objh;
dword slen;
byte chksum;
dword bofs;
word blen;

char pname[80];

dword fnt[256];

byte far buf[64000];

void startblk(byte t) {
  chksum=t;
  write(objh,&t,3);
  bofs=tell(objh)-2;
  blen=0;
}

void endblk(void) {
  ++blen;
  chksum+=blen+(blen>>8);
  chksum=256-chksum;
  write(objh,&chksum,1);
  lseek(objh,bofs,SEEK_SET);
  write(objh,&blen,2);
  lseek(objh,0L,SEEK_END);
}

void wrblk(void *p,word l) {
  word i;

  for(i=0;i<l;++i) chksum+=((byte*)p)[i];
  blen+=l;
  write(objh,p,l);
}

void wrb(byte a) {
  chksum+=a;++blen;write(objh,&a,1);
}

void wrw(word a) {
  chksum+=a+(a>>8);blen+=2;write(objh,&a,2);
}

void wrstr(char *s) {
  byte l;

  wrb(l=strlen(s));wrblk(s,l);
}

void readstrz(int h,char *s,int m) {
  int i;
  static char c;

  for(i=0;;) {
    c=0;
    read(h,&c,1);
    if(c==0) break;
    if(i<m) s[i++]=c;
  }
  s[i]=0;
}

void chkvga(void) {
  char s[41];
  struct{int w,h,sx,sy;}v;
  word sz;

  puts("Checking VGA");
  read(vgah,s,7);
  if(memcmp(s,"VGAED2",7)!=0)
	{puts("Invalid format");close(vgah);close(objh);exit(1);}
  lseek(vgah,768,SEEK_CUR);
  for(slen=1024L;;) {
	readstrz(vgah,s,40);
	if(!s[0]) break;
	read(vgah,&v,8);
	sz=atoi(s);
	if(sz>255 || sz==0 || sz==32) {lseek(vgah,(dword)v.w*v.h,SEEK_CUR);continue;}
//	printf("%d(%c) %lu\n",sz,sz,slen);
	slen+=(dword)v.w*v.h+8;
	lseek(vgah,(dword)v.w*v.h,SEEK_CUR);
  }
  printf("Size of font: %lu bytes\n",slen);
  if(slen>0x10000L)
	{puts("Segment exceeds 64K");close(vgah);close(objh);exit(1);}
}

void savevga(void) {
  char s[41];
  struct{int w,h,sx,sy;}v;
  word sz,n;
  dword w;

  puts("Converting");
  startblk(0x90);wrb(0);wrb(1);wrstr(pname);wrw(0);wrb(0);endblk();
  lseek(vgah,7+768,SEEK_SET);
  w=0;n=0;
  for(slen=1024L;;) {
	readstrz(vgah,s,40);
	if(!s[0]) break;
	read(vgah,&v,8);
	sz=atoi(s);
	if(sz>255 || sz==0 || sz==32) {lseek(vgah,(dword)v.w*v.h,SEEK_CUR);continue;}
	fnt[sz]=slen;
//	printf("%d(%c) %lu\n",sz,sz,slen);
	w+=v.w-v.sx;++n;
	  startblk(0xA0);wrb(1);wrw(slen);
	  wrblk(&v,8);
	  slen+=(sz=(dword)v.w*v.h)+8;
	  read(vgah,buf,sz);
	  wrblk(buf,sz);
	  endblk();
  }
  fnt[0]=w/n;
  printf("Space size: %u\n",fnt[0]);
  startblk(0xA0);wrb(1);wrw(0);wrblk(fnt,1024);endblk();
/*
  startblk(0x9C);
  for(n=2;n<512;n+=2) if(fnt[n/2]) {
	wrb((n>>8)|0xC4);wrb(n);wrb(0x54);wrb(1);
  }
  endblk();
*/
}

int main(int argc,char *argv[]) {
  puts("\nVGAED2 font to OBJ32 converter V1.0\n"
	"(C) Aleksey Volynskov, 1996\n");
  if(argc<4)
	{puts("Usage: FNT2OBJ3 fontfile.vga objfile.obj public_name");return 1;}
  if((vgah=_open(argv[1],O_BINARY|O_RDONLY))==-1)
	{perror(argv[1]);return 1;}
  if((objh=_creat(argv[2],0x20))==-1)
	{perror(argv[2]);return 1;}
  strcpy(pname,argv[3]);
  startblk(0x80);wrstr(argv[1]);endblk();
  chkvga();
  startblk(0x96);wrb(0);wrstr("CONST");wrstr("DATA");endblk();
  startblk(0x98);wrb(0x69);wrw(slen);wrb(2);wrb(3);wrb(1);endblk();
  savevga();
  startblk(0x8A);wrb(0);endblk();
  close(vgah);close(objh);
  return 0;
}
