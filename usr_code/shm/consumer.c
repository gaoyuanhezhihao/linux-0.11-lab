#include <sys/shm.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define BUF_SIZE 10

#define WR_POS 1
#define RD_POS 0
#define HEAD_LEN 2
void consumer(const int id, int cnt, sem_t * pSem_write, sem_t * pSem_mutex, sem_t * pSem_read, const int key) {

    int r_p = 0;
    int data = 0;

    const int shm_id = shmget(key, 1, 0666 | IPC_CREAT);
    int * const shm_hp = shmat(shm_id, 0, 0);
    printf("consumer shm_hp=%ld\n", shm_hp);
    fflush(stdout);
    while(cnt-- > 0) {
        sem_wait(pSem_read);
        sem_wait(pSem_mutex);

        r_p = shm_hp[RD_POS];
        if(r_p == BUF_SIZE) {
            r_p = 0;
        }
        data = shm_hp[HEAD_LEN + r_p];
        printf("C%d:%d, %d\n", id, r_p, data);
        fflush(stdout);

        shm_hp[RD_POS] = ++r_p;

        sem_post(pSem_mutex);
        sem_post(pSem_write);
    }
    exit(0);
}
int main(int argc, char ** argv) {
    sem_t *pSem_write= NULL;
    sem_t *pSem_read= NULL;
    sem_t *pSem_mutex= NULL;
    
    int child_pid = 0;
    int i = 0;
    int pid = 1;

    const int key = 226;

    pSem_write=sem_open("write",O_EXCL,0600,BUF_SIZE);
    pSem_read=sem_open("read",O_EXCL,0600,0);
    pSem_mutex=sem_open("mutex",O_EXCL,0600,1);

    consumer(1, 20, pSem_write, pSem_mutex, pSem_read, key);
}
