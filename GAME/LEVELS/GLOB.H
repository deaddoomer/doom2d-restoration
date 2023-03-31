// Globals

#ifndef __386__
#error Flat memory model is required!
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

#ifdef __cplusplus
extern "C" {
#endif

void logo(const char *,...);

void setgamma(int);

extern char std_pal[256][3];

#ifdef __cplusplus
}
#endif
