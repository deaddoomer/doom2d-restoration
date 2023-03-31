// Game menus

typedef struct{
  byte type;
  int n,cur,x;
  char *ttl;
  char **m;
  byte *t;
}menu_t;

void GMV_say(char *);
void GMV_stop(void);

void GM_init(void);
int GM_act(void);
int GM_draw(void);

void G_code(void);
