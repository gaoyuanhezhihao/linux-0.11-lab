#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#define FILECELLSIZE 3
#define BUF_SIZE 10
FILE *fpWork;
FILE *fpFakeTerminal;


void producer(const int id, const int start, const int end, sem_t * pSem_write, sem_t *pSem_mutex, sem_t *pSem_read) {

    int i = start;
    int w_p = 0;
    int r_p = 0;
    while(i < end) {
        sem_wait(pSem_write);//P(Empty)
        sem_wait(pSem_mutex);//P(Mutex)

        //#1读取文件中记录的写序号（接下来应该写的位置）
        fseek(fpWork,0,SEEK_SET);//把文件指针指向文件第一个字节
        fread(&r_p,sizeof(int),1,fpWork);
        fread(&w_p,sizeof(int),1,fpWork);
        //#2写入
        if(w_p == BUF_SIZE) {
            w_p = 0;
        }
        fseek(fpWork,(w_p+2)*sizeof(int),SEEK_SET);
        fwrite(&i,sizeof(int),1,fpWork);
        fflush(fpWork);
        printf("P%d:%d, %d is writed\n",id, w_p, i);
        /*printf("cur_sz =%d\n", cur_sz);*/
        /*fflush(stdout);*/
        ++w_p;
        ++i;

        //#3更新写序号
        fseek(fpWork,1*sizeof(int),SEEK_SET);
        fwrite(&w_p, sizeof(int), 1, fpWork);
        fflush(fpWork);

        sem_post(pSem_mutex);//V(Mutex)
        sem_post(pSem_read);//V(Full)
    }
}

void consumer(const int id, int cnt, sem_t * pSem_write, sem_t *pSem_mutex, sem_t *pSem_read) {

    int w_p = 0;
    int r_p = 0;
    int data = 0;
    while(cnt-- > 0) {
        sem_wait(pSem_read);
        sem_wait(pSem_mutex);

        fseek(fpWork,0,SEEK_SET);//把文件指针指向文件第一个字节
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
        /*fflush(stdout);*/
        sem_post(pSem_mutex);
        sem_post(pSem_write);//V(Full)
        /*if(cur_sz == 1) {*/
            /*sem_post(pSem_write);*/
        /*}else if(cur_sz > 1){*/
            /*sem_post(pSem_read);//V(Full)*/
        /*}*/

        int i = 0;
        for(i = 0; i < 10000000; ++i) {
            ;//heavy work
        }
    }
}
int main(int argc,char **argv)
{

    sem_t sem_empty,*pSem_write=&sem_empty;
    sem_t sem_full,*pSem_read=&sem_full;
    sem_t sem_mutex,*pSem_mutex=&sem_mutex;//only one process can handle the file at the same time
    printf("main start");
    fflush(stdout);
    int iwait=0;
    int WriteReadIndex[2]={0,0};
    //系统中可能已经存在着三个信号量，把它们清楚掉
    sem_unlink("empty");
    sem_unlink("full");
    sem_unlink("mutex");
    //建立这三个型号量
    pSem_write=sem_open("empty",O_CREAT | O_EXCL,0600,BUF_SIZE);
    pSem_read=sem_open("full",O_CREAT | O_EXCL,0600,0);
    pSem_mutex=sem_open("mutex",O_CREAT | O_EXCL,0600,1);
    /*打开作为缓存的文件。文件的书写格式是：写进程从文件的4号字节开始顺序写入内容，读进程
      也从4号字节开始顺序读进程内容。为了不同的写进程之间同步写的位置，文件的第0，1字节用来
      保存当前写的位置。同理，为了读进程之间能够同步文件的第2，3用来保存读的位置（这里也可以
      使用一个信号量来同步读写位置）。为了简化，这里并没有完全按照实验指导中的要求，把读过的
      内容删除掉。*/
    fpWork = fopen("work.dat","wb+");
    fwrite(WriteReadIndex,2*sizeof(int),1,fpWork);
    int init_data = 0;
    int i = 0;
    for(i = 0; i < BUF_SIZE; ++i) {
        fwrite(&init_data, sizeof(int), 1, fpWork);
    }
    fflush(fpWork);

    const int write_len = 500;
    const int producer_num = 1;
    const int consumer_num = 5;
    const int read_len = write_len * producer_num / consumer_num;

    int pid = 1; 
    for(i = 0; i < producer_num; ++i) {
        if(pid == 0) {
            break;
        }
        pid = fork();
        if(0 == pid) {
            producer(i, i*write_len, (i+1)*write_len, pSem_write, pSem_mutex, pSem_read);
        }
    }

    i = 0;
    for(i = 0; i < consumer_num; ++i) {
        if(pid == 0) {
            break;
        }

        pid = fork();
        if(0 == pid) {
            consumer(i, read_len, pSem_write, pSem_mutex, pSem_read);
        }
    }
    while(1){;}
}

