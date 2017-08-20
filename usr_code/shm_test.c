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
void producer(const int id, const int start, const int end, sem_t * pSem_write, sem_t * pSem_mutex, sem_t * pSem_read, const int key) {

    int i = start;
    int w_p = 0;
    int r_p = 0;

    const int shm_id = shmget(key, 1, 0666 | IPC_CREAT);
    int * const shm_hp = shmat(shm_id, 0, 0);
    shm_hp[RD_POS] = 0;
    shm_hp[WR_POS] = 0;
    sem_wait(pSem_mutex);
    sem_post(pSem_mutex);
    while(i < end) {
        sem_wait(pSem_write);
        sem_wait(pSem_mutex);

        r_p = shm_hp[RD_POS];
        w_p = shm_hp[WR_POS];
        if(w_p == BUF_SIZE) {
            w_p = 0;
        }

        shm_hp[w_p+HEAD_LEN] = i++;
        shm_hp[WR_POS] = ++w_p;
        printf("P%d:%d, %d is writed\n",id, w_p, i);

        sem_post(pSem_mutex);
        sem_post(pSem_read);
    }
    exit(0);
    }


void consumer(const int id, int cnt, sem_t * pSem_write, sem_t * pSem_mutex, sem_t * pSem_read, const int key) {

    int r_p = 0;
    int data = 0;

    const int shm_id = shmget(key, 1, 0666 | IPC_CREAT);
    int * const shm_hp = shmat(shm_id, 0, 0);
    while(cnt-- > 0) {
        sem_wait(pSem_read);
        sem_wait(pSem_mutex);

        r_p = shm_hp[RD_POS];
        if(r_p == BUF_SIZE) {
            r_p = 0;
        }
        data = shm_hp[HEAD_LEN + r_p];
        printf("C%d:%d, %d\n", id, r_p, data);
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
    int pid = 0;

    const int write_len = 50;
    const int producer_num = 1;
    const int consumer_num = 5;
    const int read_len = write_len * producer_num / consumer_num;
    const int key = 226;

    sem_unlink("write");
    sem_unlink("read");
    sem_unlink("mutex");
    pSem_write=sem_open("write",O_CREAT | O_EXCL,0600,BUF_SIZE);
    pSem_read=sem_open("read",O_CREAT | O_EXCL,0600,0);
    pSem_mutex=sem_open("mutex",O_CREAT | O_EXCL,0600,1);

    printf("main start\n");
    for(i = 0; i < producer_num; ++i) {
        if(pid == 0) {
            break;
        }
        pid = fork();
        if(0 == pid) {
            producer(i, i*write_len, (i+1)*write_len, pSem_write, pSem_mutex, pSem_read, key);
        }
    }

    for(i = 0; i < consumer_num; ++i) {
        if(pid == 0) {
            break;
        }

        pid = fork();
        if(0 == pid) {
            consumer(i, read_len, pSem_write, pSem_mutex, pSem_read, key);
        }
    }
    
    while((child_pid = wait(NULL)) != -1) {
        continue;
    }
    printf("main end\n");
    fflush(stdout);
}

