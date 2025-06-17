if [ "$2" == "release" ]; then
qemu-system-x86_64 \
    -bios /usr/share/OVMF/x64/OVMF.4m.fd \
    -m 1G \
    -debugcon file:debug.log \
    -global isa-debugcon.iobase=0xe9 \
    -no-reboot \
    -d int,cpu_reset,in_asm \
    -D qemu.log \
    -M pc \
    -smp 2 \
    -cpu core2duo,vendor=GenuineIntel,+sse,+sse2,+sse3,+hypervisor \
    -drive file="./bin/image.img",if=ide,cache=writethrough,format=raw \
    -device pci-bridge,chassis_nr=1,id=pci_bridge1 \
    -device e1000,netdev=net0,bus=pci_bridge1,addr=0x1 \
    -netdev user,id=net0 \
    # -drive id=disk,file="/dev/sda",format=raw,if=ide
fi