rm ./shm.exe
gcc -g -o shm.exe -D_REENTRANT shm_test.c -lpthread
