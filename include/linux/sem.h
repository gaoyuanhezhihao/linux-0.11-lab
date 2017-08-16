#ifndef SEM_H
#define SEM_H

#include <linux/sched.h>

#define SEM_NAME_LEN 80
struct sem_struct{
    char key[SEM_NAME_LEN];
    unsigned int cnt;
    struct task_struct * wait_proc;
};

typedef struct sem_struct sem_t;
extern void sem_init();


#endif //SEM_H
