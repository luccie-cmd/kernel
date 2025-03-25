if [ "$2" == "release" ]; then
qemu-system-x86_64 \
    -bios /usr/share/OVMF/x64/OVMF.4m.fd \
    -m 2G \
    -debugcon file:debug.log \
    -global isa-debugcon.iobase=0xe9 \
    -no-reboot \
    -d int,cpu_reset \
    -D qemu.log \
    "$1/image.img"
    # -drive file="$1/image.img",if=none,id=nvme1,format=raw \
    # -device nvme,drive=nvme1,serial=deadbeef \
    # -cpu host \
    # -enable-kvm
    # -M q35 \
elif [ "$2" == "debug" ]; then
qemu-system-x86_64 \
    -bios /usr/share/OVMF/x64/OVMF.4m.fd \
    -debugcon file:debug.log \
    -global isa-debugcon.iobase=0xe9 \
    -d int,cpu_reset \
    -D qemu.log \
    -no-reboot \
    -m 2G \
    "$1/image.img"
    # -drive file="$1/image.img",if=none,id=nvme1,format=raw \
    # -device nvme,drive=nvme1,serial=deadbeef \
    # -enable-kvm \
    # -cpu host \
    # -M q35 \
    # -S -s
# sleep 1
# gdb -ex "target remote localhost:1234" -ex "add-symbol-file $1/kernel.elf 0xffffffff80000000" -ex "set disassembly-flavor intel"
fi