// Globals

#ifndef __386__
#error А как же DOS4GW?
#endif

#ifndef NULL
#define NULL 0
#endif

#define ON 1
#define OFF 0
#define TRUE 1
#define FALSE 0

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;

#define random(n) (rand()%(n))

void logo(const char *,...);
void logo_gas(int,int);

extern int gamma;
extern int snd_card;
extern int _cpu;
