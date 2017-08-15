#include <errno.h>
#include <string.h>
#include <linux/sched.h>
/*#include <linux/sem.h>*/


#define NR_SEM 32
#define SEM_NAME_LEN 80

struct sem_t{
    char key[SEM_NAME_LEN];
    unsigned int cnt;
    struct task_struct * wait_proc;
};

struct sem_t sem_vec[NR_SEM];

void sem_init() {
    int i = 0;
    for(i = 0; i < NR_SEM; ++i) {
        sem_vec[i].key[0] = '\0';
        sem_vec[i].cnt = 0;
        sem_vec[i].wait_proc = 0;
    }
}

struct sem_t * sys_sem_open(const char * key, unsigned int init_val) {
    printk("sys_sem_open\n");
    return 0;
    int i = 0;
    struct sem_t * empty;
    for(i = 0; i < NR_SEM; ++i) {
        if(sem_vec[i].key[0] == '\0') {
            empty = sem_vec + i;
        }else if(strcmp(key, sem_vec[i].key)) {
            return sem_vec+i;
        }
    }

    strcpy(empty->key, key);
    empty->cnt = init_val;
    empty->wait_proc = (struct task_struct *)NULL;
    return empty;
}

void sys_sem_unlink(const char *key) {
    printk("sys_sem_unlink\n");
    return;

    int i = 0;
    for(i = 0; i < NR_SEM; ++i) {
        if(strcmp(key, sem_vec[i].key)) {
            sem_vec[i].key[0] = '\0';
            break;
        }
    }
    return;
}

int sys_sem_wait(struct sem_t * p_sem) {
    printk("sys_sem_wait\n");
    return 0;
    while(p_sem->cnt == 0) {
        sleep_on(&(p_sem->wait_proc));
    }
    --p_sem->cnt;
    return 0;
}

int sys_sem_post(struct sem_t * p_sem) {
    printk("sys_sem_post\n");
    return 0;
    ++p_sem->cnt;
    if(!p_sem->wait_proc) {
        wake_up(&p_sem->wait_proc);
    }
    return 0;
}
