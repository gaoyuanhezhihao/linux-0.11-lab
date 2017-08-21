
if ! mountpoint -q ./mnt; then
    sudo mount -t minix -o loop,offset=1024 ./rootfs/hdc-0.11.img ./mnt
fi

sudo cp ./mnt/usr/root/shm/print.txt ./usr_code/shm/print.txt
sudo chmod 644 ./usr_code/shm/print.txt
sudo umount ./mnt
