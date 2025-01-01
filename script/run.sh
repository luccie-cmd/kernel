if [ "$2" == "release" ]; then
qemu-system-x86_64 \
    -bios /usr/share/OVMF/x64/OVMF.4m.fd \
    -drive file="$1/image.img",format=raw,cache=writeback,snapshot=off \
    -m 128 \
    -debugcon stdio \
    -global isa-debugcon.iobase=0xe9 \
    -d int,cpu_reset \
    -M q35 \
    -no-reboot  \
    -cpu host \
    -enable-kvm
elif [ "$2" == "debug" ]; then
qemu-system-x86_64 \
    -bios /usr/share/OVMF/x64/OVMF.4m.fd \
    -drive file="$1/image.img",format=raw,cache=writeback,snapshot=off \
    -debugcon stdio \
    -global isa-debugcon.iobase=0xe9 \
    -d int,cpu_reset \
    -no-reboot \
    -M q35 \
    -m 128 \
    -cpu host \
    -enable-kvm \
    -S -s
fi