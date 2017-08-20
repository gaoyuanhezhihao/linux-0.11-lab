#include <sys/shm.h>
#include <linux/mm.h>
#include <linux/sched.h>

#define SHM_NR 32
shm_t shm_vec[32];


static int get_free_shm_p() {
    int i = 0;
    for(i = 0; i < NR_SHM_OPEN; ++i){
        if(current->shm_p[i] == NULL) {
            return i;
        }
    }
    return -1;
}

int sys_shmget(int key, int size, int flag ){
    /*printk("sys_shmget\n");*/
    /*return 0;*/
    int empty_i = -1;
    int i = 0;
    int shm_id = get_free_shm_p();
    if(-1 == shm_id) {
        return -1;
    }

    for(i = 0; i < SHM_NR; ++i) {
        if(-1 == shm_vec[i].key) {
            empty_i = i;
        }else if(shm_vec[i].key == key){
            current->shm_p[shm_id] = &shm_vec[i];
            ++shm_vec[i].cnt;
            return shm_id;
        }
    }

    if(-1 == empty_i) {
        panic("out of free shm!!!\n");
        return -1;
    }
    current->shm_p[shm_id] = &shm_vec[empty_i];
    shm_vec[empty_i].key = key;
    shm_vec[empty_i].cnt = 1;
    shm_vec[empty_i].addr = 0;
    return shm_id;
}

const void * sys_shmat(int shm_id) {
    /*printk("sys_shmat\n");*/
    /*return 0;*/
    shm_t * shm_p = current->shm_p[shm_id];
    if(shm_id < 0 || shm_id >= NR_SHM_OPEN || shm_p->key == -1) {
        panic("ERROR ! sys_shmat: shm_id out of range\n");
        return 0;
    }



    /*align */
    current->brk += (PAGE_SIZE - current->brk %PAGE_SIZE);
    unsigned long v_addr = current->brk + current->start_code;
    current->brk += PAGE_SIZE;

    /*put_page(shm_p->addr, v_addr);*/
    if(shm_p->addr == 0) {
        shm_p->addr = get_free_page();
        shm_map(shm_p->addr, v_addr, 1);
    }else {
        shm_map(shm_p->addr, v_addr, 0);
    }
    /*printk("shm_at: %d\n", shm_p->addr);*/
    return (const void *) (v_addr-current->start_code);
}

int sys_shmdt(const void * v_addr) {
    printk("sys_shmdt\n");
    return 0;
}

void clear_one_shm(shm_t * shm_p) {
    if(NULL == shm_p) {
        panic("Temp to decrease non-exist shm!!!");
    }

    --shm_p->cnt;
    if(0 == shm_p->cnt){
        shm_p->key = -1;
        shm_p->addr = 0;
    }
}

void init_proc_shm_p(struct task_struct * tsk) {
    int i = 0;
    for(i = 0; i < NR_SHM_OPEN; ++i) {
        tsk->shm_p[i] = NULL;
    }
}

/** 
 * @brief clear shm_p[] of a task_struct. called by do_exit()
 * 
 * @param tsk : task_struct exiting.
 */
void clear_shm_p(struct task_struct * tsk) {
    int i = 0;
    for(i = 0; i < NR_SHM_OPEN; ++i) {
        if(tsk->shm_p[i] != NULL) {
            clear_one_shm(tsk->shm_p[i]);
            tsk->shm_p[i] = 0;
        }
    }
}

void shm_vec_init() {
    int i = 0;
    for(i = 0; i < SHM_NR; ++i) {
        shm_vec[i].key = -1;
        shm_vec[i].cnt = 0;
        shm_vec[i].addr = 0;
    }
}
