// High-level memory allocation

#ifdef __cplusplus
extern "C" {
#endif

void M_startup(void);
void M_shutdown(void);
void *M_lock(int);
void M_unlock(void *);

#ifdef __cplusplus
}
#endif
