#ifndef LOG_H
#define LOG_H

//extern int logK_info(const char * info, int count);
//extern int logK_init(void);
extern int fprintk(int fd, const char *fmt, ...);
#endif //LOG_H
