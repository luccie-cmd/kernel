if [ "$2" == "release" ]; then
    # -device nvme,drive=nvme1,serial=deadbeef \
qemu-system-x86_64 \
    -bios /usr/share/OVMF/x64/OVMF.4m.fd \
    -drive file="$1/image.img",if=ide,format=raw \
    -m 128 \
    -debugcon file:debug.log \
    -global isa-debugcon.iobase=0xe9 \
    -d int,cpu_reset \
    -no-reboot \
    -M pc \
    -cpu host \
    -enable-kvm
elif [ "$2" == "debug" ]; then
qemu-system-x86_64 \
    -bios /usr/share/OVMF/x64/OVMF.4m.fd \
    -drive file="$1/image.img",format=raw,cache=writeback,snapshot=off,if=ide,index=0 \
    -debugcon file:debug.log \
    -global isa-debugcon.iobase=0xe9 \
    -d int,cpu_reset \
    -no-reboot \
    -m 128 \
    -M pc \
    -S -s \
    -cpu host \
    -enable-kvm &
sleep 1
gdb -ex "target remote localhost:1234" -ex "add-symbol-file $1/kernel.elf 0xffffffff80000000" -ex "set disassembly-flavor intel"
fi