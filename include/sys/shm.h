#ifndef SHM_H
#define SHM_H

#define __LIBRARY__
#include <unistd.h>

#define IPC_CREAT 1
#define SHM_KEY_LEN 20
typedef struct shm_struct{
    unsigned long addr;
    int key;
    int cnt;
}shm_t;

struct task_struct;

void init_proc_shm_p(struct task_struct * tsk);
void clear_shm_p(struct task_struct * tsk);
void shm_vec_init();


static inline int shmget(int key, unsigned int size, int shmflg) {
    long __res;
    __asm__ volatile("int $0x80"
            : "=a"(__res)
            : "0" (__NR_shm_get), "b" ((long)key), "c"((long)size), "d" ((long)shmflg) );
    return (int)__res;
}

static inline void * shmat(int shm_id, const void * shm_addr, int shmflg) {
    long __res;
    __asm__ volatile ("int $0x80"
            : "=a" (__res)
            : "0" (__NR_shm_at), "b" ((long)shm_id));
    return (void *) __res;
}

static inline int shmdt(int shm_id) {
    long __res;
    __asm__ volatile ("int $0x80"
            : "=a" (__res)
            : "0" (__NR_shm_dt), "b" ((long)shm_id));
    return (int) __res;
}

#endif //SHM_H
