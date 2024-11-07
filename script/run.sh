qemu-system-x86_64 \
    -bios /usr/share/OVMF/OVMF_CODE.fd \
    -drive file=$1/image.img,format=raw \
    -m 128 \
    -M q35 \
    -debugcon stdio \
    -global isa-debugcon.iobase=0xe9 \
    -d int,cpu_reset \
    -no-reboot