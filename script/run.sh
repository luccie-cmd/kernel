if [ "$2" == "release" ]; then
qemu-system-x86_64 \
    -m 1G \
    -debugcon file:debug.log \
    -global isa-debugcon.iobase=0xe9 \
    -no-reboot \
    -d int,cpu_reset \
    -D qemu.log \
    -M pc \
    -smp 2 \
    -cpu core2duo,vendor=GenuineIntel,+sse3,+cx16,+hypervisor \
    -drive file="./bin/image.img",if=ide,cache=writethrough,format=raw \
    # -drive id=disk,file="/dev/sda",format=raw,if=ide \
    # -device piix3-ide,id=ide \
    # -device ide-hd,drive=disk,bus=ide.0
    # -cpu "Penryn" \
    # -drive file="$1/image.img",format=raw \
    # -device nvme,drive=nvme1,serial=deadbeef \
    # -bios /usr/share/OVMF/x64/OVMF.4m.fd \
    # -enable-kvm
fi