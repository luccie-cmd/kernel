if [ "$2" == "release" ]; then
qemu-system-x86_64 \
    -m 1G \
    -debugcon file:debug.log \
    -global isa-debugcon.iobase=0xe9 \
    -no-reboot \
    -d int,cpu_reset \
    -D qemu.log \
    -drive file="$1/image.img",format=raw \
    -cpu "Penryn" \
    -M pc \
    # -device nvme,drive=nvme1,serial=deadbeef \
    # -bios /usr/share/OVMF/x64/OVMF.4m.fd \
    # -enable-kvm
fi