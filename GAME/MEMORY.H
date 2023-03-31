// High-level memory allocation

void M_startup(void);
void M_shutdown(void);
//void *M_mapres(int);
//void M_loadblock(word);
//void M_preload(void);
void *M_lock(int);
void M_unlock(void *);
