// Error handling

#ifdef __cplusplus
extern "C" {
#endif

void ERR_failinit(char *,...);
void ERR_fatal(char *,...);
void ERR_quit(void);

#ifdef __cplusplus
}
#endif
