#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#define __LIBRARY__
#include <unistd.h>

#define SEM_NAME_LEN 80
struct sem_t{
    char key[SEM_NAME_LEN];
    unsigned int cnt;
    struct task_struct * wait_proc;
};

inline struct sem_t * sem_open(const char * name, unsigned int flag, unsigned int mode, unsigned int init_val) {
    long __res;
    __asm__ volatile ("int $0x80" 
        : "=a" (__res) 
        : "0" (__NR_sem_open),"b" ((long)(name)),"c" ((long)(init_val)));
    return (struct sem_t *)__res;
}

inline int sem_wait(struct sem_t * p_sem) {
    long __res;
    __asm__ volatile ("int $0x80"
        : "=a" (__res)
        : "0" (__NR_sem_wait),"b" ((long)(p_sem)));
    return (int)__res;
}

inline int sem_post(struct sem_t * p_sem) {
    long __res;
    __asm__ volatile ("int $0x80"
        : "=a" (__res)
        : "0" (__NR_sem_post),"b" ((long)(p_sem)));
    return (int)__res;
}

inline int sem_unlink(const char * name) {
    long __res;
    __asm__ volatile ("int $0x80"
        : "=a" (__res)
        : "0" (__NR_sem_unlink),"b" ((long)(name)));
    return (int)__res;
}



#endif //SEMAPHORE_H
