#include <sys/shm.h>

int main(int argc, char ** argv) {
    const int shm_id = shmget(123, 1, 0666 | IPC_CREAT);
    int * const shm_hp = shmat(shm_id, 0, 0);
    
    for(int i = 0; i < 10; ++i) {
        shm_hp[i] = i;
        printf("write %d\n", i);
    }
    /*const int child_cnt = 1;*/
    /*int i = 0;*/
    /*for(i = 0; i < child_cnt; ++i) {*/
        /*if(0!=fork()){*/
            /*const int shm_id = shmget(123, 1, 0666 | IPC_CREAT);*/
            /*int * const shm_hp = shmat(shm_id, 0, 0);*/
        /*}*/
    /*}*/
}
