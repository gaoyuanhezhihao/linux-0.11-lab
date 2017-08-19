#include <sys/shm.h>

int main(int argc, char ** argv) {
    shmget(0, 0, 0);
    shmat(0, 0, 0);
    shmdt(0);
}
