if [ "$2" == "release" ]; then
qemu-system-x86_64 \
    -m 1G \
    -debugcon file:debug.log \
    -global isa-debugcon.iobase=0xe9 \
    -no-reboot \
    -d int,cpu_reset \
    -D qemu.log \
    -M pc \
    -cpu qemu64,+sse2,-sse4.1,-sse4.2,-avx,-avx2,-x2apic,-aes \
    -drive file="./bin/image.img",if=ide,cache=writethrough,format=raw
    # -cpu "Penryn" \
    # -drive file="$1/image.img",format=raw \
    # -device nvme,drive=nvme1,serial=deadbeef \
    # -bios /usr/share/OVMF/x64/OVMF.4m.fd \
    # -enable-kvm
fi