qemu-system-x86_64 \
    -bios /usr/share/OVMF/x64/OVMF.4m.fd \
    -device ide-hd,drive=mydrive,bus=ide.0 \
    -drive id=mydrive,file=$1/image.img,format=raw,if=none \
    -m 128 \
    -debugcon stdio \
    -global isa-debugcon.iobase=0xe9 \
    -d int,cpu_reset \
    -no-reboot
