if [ "$2" == "release" ]; then
qemu-system-x86_64 \
    -bios /usr/share/OVMF/x64/OVMF.4m.fd \
    -m 128 \
    -debugcon file:debug.log \
    -global isa-debugcon.iobase=0xe9 \
    -no-reboot \
    -M q35 \
    -d int,cpu_reset \
    -drive file="$1/image.img",if=none,id=nvme1,format=raw \
    -device nvme,drive=nvme1,serial=deadbeef \
    -cpu host \
    -enable-kvm
elif [ "$2" == "debug" ]; then
qemu-system-x86_64 \
    -bios /usr/share/OVMF/x64/OVMF.4m.fd \
    -debugcon file:debug.log \
    -global isa-debugcon.iobase=0xe9 \
    -d int,cpu_reset \
    -no-reboot \
    -m 128 \
    -M q35 \
    -drive file="$1/image.img",if=none,id=nvme1,format=raw \
    -device nvme,drive=nvme1,serial=deadbeef \
    -enable-kvm \
    -cpu host \
    # -S -s
# sleep 1
# gdb -ex "target remote localhost:1234" -ex "add-symbol-file $1/kernel.elf 0xffffffff80000000" -ex "set disassembly-flavor intel"
fi