#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
/*#include <sys/shm.h>*/
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#define FILECELLSIZE 3
#define BUF_SIZE 10
FILE *fpWork;
FILE *fpFakeTerminal;
void producer(const int id, const int start, const int end, struct sem_t * pSem_write, struct sem_t *pSem_mutex, struct sem_t *pSem_read) {

    int i = start;
    int w_p = 0;
    int r_p = 0;
    while(i < end) {
        sem_wait(pSem_write);
        sem_wait(pSem_mutex);

        fseek(fpWork,0,SEEK_SET);
        fread(&r_p,sizeof(int),1,fpWork);
        fread(&w_p,sizeof(int),1,fpWork);
        if(w_p == BUF_SIZE) {
            w_p = 0;
        }
        fseek(fpWork,(w_p+2)*sizeof(int),SEEK_SET);
        fwrite(&i,sizeof(int),1,fpWork);
        fflush(fpWork);
        printf("P%d:%d, %d is writed\n",id, w_p, i);
        /*printf("cur_sz =%d\n", cur_sz);*/
        fflush(stdout);
        ++w_p;
        ++i;

        fseek(fpWork,1*sizeof(int),SEEK_SET);
        fwrite(&w_p, sizeof(int), 1, fpWork);
        fflush(fpWork);

        sem_post(pSem_mutex);
        sem_post(pSem_read);
    }
    exit(0);
}

void consumer(const int id, int cnt, struct sem_t * pSem_write, struct sem_t *pSem_mutex, struct sem_t *pSem_read) {

    int w_p = 0;
    int r_p = 0;
    int data = 0;
    int i = 0;
    while(cnt-- > 0) {
        sem_wait(pSem_read);
        sem_wait(pSem_mutex);

        fseek(fpWork,0,SEEK_SET);
        fread(&r_p,sizeof(int),1,fpWork);
        fread(&w_p,sizeof(int),1,fpWork);
        if(r_p == BUF_SIZE) {
            r_p = 0;
        }
        fseek(fpWork, (r_p+2) * sizeof(int), SEEK_SET);
        fread(&data, sizeof(int), 1, fpWork);
        printf("C%d:%d, %d\n", id, r_p, data);
        /*printf("cur_sz =%d\n", cur_sz);*/

        ++r_p;
        fseek(fpWork, 0, SEEK_SET);
        fwrite(&r_p, sizeof(int), 1, fpWork);
        fflush(fpWork);
        fflush(stdout);
        sem_post(pSem_mutex);
        sem_post(pSem_write);

        for(i = 0; i < 10000000; ++i) {
            ;
        }
    }
    exit(0);
}
int main(int argc,char **argv)
{

    struct sem_t *pSem_write= NULL;
    struct sem_t *pSem_read= NULL;
    struct sem_t *pSem_mutex= NULL;
    int iwait=0;
    int WriteReadIndex[2]={0,0};
    int init_data = 0;
    int i = 0;
    int pid = 1; 
    const int write_len = 50;
    const int producer_num = 1;
    const int consumer_num = 5;
    const int read_len = write_len * producer_num / consumer_num;
    int child_pid = 0;
    printf("main start");
    fflush(stdout);
    sem_unlink("write");
    sem_unlink("read");
    sem_unlink("mutex");
    pSem_write=sem_open("write",O_CREAT | O_EXCL,0600,BUF_SIZE);
    pSem_read=sem_open("read",O_CREAT | O_EXCL,0600,0);
    pSem_mutex=sem_open("mutex",O_CREAT | O_EXCL,0600,1);
    fpWork = fopen("work.dat","wb+");
    fwrite(WriteReadIndex,2*sizeof(int),1,fpWork);
    for(i = 0; i < BUF_SIZE; ++i) {
        fwrite(&init_data, sizeof(int), 1, fpWork);
    }
    fflush(fpWork);


    for(i = 0; i < producer_num; ++i) {
        if(pid == 0) {
            break;
        }
        pid = fork();
        if(0 == pid) {
            producer(i, i*write_len, (i+1)*write_len, pSem_write, pSem_mutex, pSem_read);
        }
    }

    for(i = 0; i < consumer_num; ++i) {
        if(pid == 0) {
            break;
        }

        pid = fork();
        if(0 == pid) {
            consumer(i, read_len, pSem_write, pSem_mutex, pSem_read);
        }
    }
    
    while((child_pid = wait(NULL)) != -1) {
        continue;
    }
    /*while(1){;}*/
}
