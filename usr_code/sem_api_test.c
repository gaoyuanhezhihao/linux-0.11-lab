#include "semaphore.h"
#include "fcntl.h"

int main(int argc, const char ** argv) {
    struct sem_t *pSem = NULL;
    sem_unlink("empty");
    pSem = sem_open("empty",O_CREAT | O_EXCL,0600, 1);
    sem_wait(pSem);
    sem_post(pSem);
}
