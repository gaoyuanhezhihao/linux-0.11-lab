#include <errno.h>
#include <string.h>
#include <linux/sched.h>
#include <linux/sem.h>
#include <linux/log.h>
#include <asm/segment.h>
#include <asm/system.h>
/*#include <linux/sem.h>*/


#define NR_SEM 5 
/*#define SEM_NAME_LEN 80*/

/*sem_t{*/
    /*char key[SEM_NAME_LEN];*/
    /*unsigned int cnt;*/
    /*struct task_struct * wait_proc;*/
/*};*/

sem_t sem_vec[NR_SEM];

void sem_init() {
    int i = 0;
    for(i = 0; i < NR_SEM; ++i) {
        sem_vec[i].key[0] = '\0';
        sem_vec[i].cnt = 0;
        sem_vec[i].wait_proc = 0;
    }
}

void strcpy_krn(char * dst, const char * src_usr) {
    while(*dst = get_fs_byte(src_usr)) {
        ++dst;
        ++src_usr;
    }
}

sem_t * sys_sem_open(const char * key, unsigned int init_val) {
    /*printk("sys_sem_open\n");*/
    /*return 0;*/
    char key_cp[SEM_NAME_LEN];
    strcpy_krn(key_cp, key);
    /*fprintk(3, "sys_sem_open: PID=%d, key=%s, val=%d\n", current->pid, key_cp, init_val);*/
    int i = 0;
    sem_t * empty;
    for(i = 0; i < NR_SEM; ++i) {
        if(sem_vec[i].key[0] == '\0') {
            empty = sem_vec + i;
        }else if(0 == strcmp(key_cp, sem_vec[i].key)) {
            return sem_vec+i;
        }
    }

    strcpy(empty->key, key_cp);
    empty->cnt = init_val;
    empty->wait_proc = (struct task_struct *)NULL;
    return empty;
}

void sys_sem_unlink(const char *key) {
    /*printk("sys_sem_unlink\n");*/
    /*return;*/
    char key_cp[SEM_NAME_LEN];
    strcpy_krn(key_cp, key);

    /*fprintk(3, "sys_sem_unlink: PID=%d, key=%s\n", current->pid, key_cp);*/
    int i = 0;
    for(i = 0; i < NR_SEM; ++i) {
        if(strcmp(key_cp, sem_vec[i].key)) {
            sem_vec[i].key[0] = '\0';
            break;
        }
    }
    return;
}

int sys_sem_wait(sem_t * p_sem) {
    /*printk("sys_sem_wait\n");*/
    /*return 0;*/
    cli();
    while(p_sem->cnt == 0) {
        sleep_on_stack(&(p_sem->wait_proc));
    }
    /*fprintk(3, "sys_sem_wait: PID=%d, key=%s, cnt=%d\n", current->pid, p_sem->key, p_sem->cnt);*/
    --p_sem->cnt;
    sti();
    return 0;
}

int sys_sem_post(sem_t * p_sem) {
    /*printk("sys_sem_post\n");*/
    /*return 0;*/
    cli();
    /*fprintk(3, "sys_sem_post: PID=%d, key=%s, cnt=%d\n", current->pid, p_sem->key, p_sem->cnt);*/
    ++p_sem->cnt;
    if(p_sem->wait_proc) {
        wake_up(&(p_sem->wait_proc));
    }
    sti();
    return 0;
}
