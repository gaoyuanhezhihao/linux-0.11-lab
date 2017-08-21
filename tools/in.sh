if ! mountpoint -q ./mnt; then
    sudo mount -t minix -o loop,offset=1024 ./rootfs/hdc-0.11.img ./mnt
fi

sudo cp ./usr_code/sem/sem_api_test.c ./mnt/usr/root/
sudo cp ./include/semaphore.h ./mnt/usr/include/
sudo cp ./include/unistd.h ./mnt/usr/include/
sudo cp ./usr_code/sem/sem_test.c ./mnt/usr/root/
sudo mkdir -p ./mnt/usr/root/shm
sudo cp ./usr_code/shm/shm_api_test.c ./mnt/usr/root/shm/
sudo cp ./include/sys/shm.h ./mnt/usr/include/sys/
sudo cp ./usr_code/shm/shm_test.c ./mnt/usr/root/shm/
sudo cp ./usr_code/shm/producer.c ./mnt/usr/root/producer.c
sudo cp ./usr_code/shm/consumer.c ./mnt/usr/root/consumer.c

sudo umount ./mnt
