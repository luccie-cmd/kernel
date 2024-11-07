#!/bin/python3
#
# Copyright (c) - All Rights Reserved.
#
# See the LICENCE file for more information.
#


import os
import glob
import sys
import subprocess
import threading
from script.util import parseSize, compareFiles

def readConfig(path: str) -> dict[str, list[str]]:
    """Reads a configuration file and returns a dictionary of lists of strings."""
    config = {}
    try:
        with open(path, "r") as f:
            lines = f.readlines()
            for line in lines:
                line = line.strip()
                if ':' in line:
                    key, value = line.split(":", 1)  # Split at the first ':'
                    key = key.strip()
                    value = value.strip().strip("'")  # Strip leading/trailing whitespace and single quotes
                    config[key] = [v.strip().strip("'") for v in value.split(",")]  # Split by ',' and strip quotes from each part
    except FileNotFoundError:
        print(f"Error: Configuration file '{path}' not found.")
        exit(1)
    return config


def writeConfig(config: dict[str, list[str]], path: str) -> None:
    """Writes a configuration dictionary to a file."""
    with open(path, "w") as f:
        for key, values in config.items():
            f.write(f"{key}:")
            for val in enumerate(values):
                f.write(f"'{val[1]}'")
                if val[0]+1 < len(values):
                    # pass
                    f.write(", ")
            f.write('\n')

def checkConfig(config: dict[str, list[str]], allowed_config: list[list[str, list[str], bool]]) -> bool:
    for (key, valid_values, is_mandatory) in allowed_config:
        if key not in config:
            if is_mandatory:
                print(f"Mandatory config {key} is missing")
                return False
            continue
        if valid_values:
            for value in config[key]:
                if value not in valid_values:
                    print(f"Invalid value '{value}' for key '{key}'")
                    return False
    return True

CONFIG = readConfig("./script/config.py")
OLD_CONFIG = readConfig("./script/config.py.old")
ALLOWED_CONFIG = [
    ["config", ["release", "debug"], True],
    ["arch", ["x64"], True],
    ["compiler", ["gcc", "clang"], True],
    ["imageFS", ["fat32", 'ext2', 'ext3', "ext4"], True],
    ["bootloader", ["limine-uefi", "custom"], True],
    ["outDir", [], True],
    ["imageSize", [], False],
]
if not checkConfig(CONFIG, ALLOWED_CONFIG):
    print("Invalid config file.")
    print("Allowed config items")
    for option in ALLOWED_CONFIG:
        name = option[0]
        values = option[1]
        required = option[2]
        print(f"{name} (required = {required})")
        if len(values) == 0:
            print("    This can be anything as long as it's provided")
        else:
            for val in values:
                print(f"  - {val}")
    exit(1)
writeConfig(CONFIG, "./script/config.py.old")
force_rebuild = False
if OLD_CONFIG != CONFIG:
    force_rebuild = True
    print("Configuration changed, rebuilding...")
# Add some default values to the config
CONFIG["CFLAGS"] = ['-c', '-g']
CONFIG["CFLAGS"] += ['-ffreestanding', '-finline-functions', '-fmax-errors=1', '-fno-use-cxa-atexit', '-fno-strict-aliasing', '-fno-common', '-fno-asynchronous-unwind-tables', '-fno-delete-null-pointer-checks', '-fstack-protector-strong', '-fno-stack-protector']
CONFIG["CFLAGS"] += ['-fno-builtin', '-fno-PIE', '-fno-omit-frame-pointer', '-fvar-tracking', '-fconserve-stack', '-fno-PIE', '-fno-pie', '-fno-PIC', '-fno-pic']
CONFIG["CFLAGS"] += ['-mno-red-zone', '-mcmodel=kernel', '-mno-sse', '-mno-sse2', '-mno-80387', '-mno-avx', '-mno-avx512f']
CONFIG["CFLAGS"] += ['-Werror', '-Wall', '-Wextra', '-Wno-unused-parameter', '-Wno-unused-variable', '-Wno-unused-function', '-Wcast-align', '-Wcast-qual', '-Wpointer-arith', '-Wshadow']
CONFIG["CXXFLAGS"] = ['-fno-rtti', '-fno-exceptions']
CONFIG["ASFLAGS"] = ['-felf64']
CONFIG["LDFLAGS"] = ['-nostdlib', '-g']
CONFIG["INCPATHS"] = ['-Iinclude']
if "imageSize" not in CONFIG:
    CONFIG["imageSize"] = '128m'

if "debug" in CONFIG.get("config"):
    CONFIG["CFLAGS"] += ["-O0"]
    CONFIG["CFLAGS"] += ["-DDEBUG"]
else:
    CONFIG["CFLAGS"] += ["-O2"]
    CONFIG["CFLAGS"] += ["-DNDEBUG"]
if "x64" in CONFIG.get("arch"):
    CONFIG["CFLAGS"] += ["-m64"]

if "debug" in CONFIG.get("config"):
    CONFIG["LDFLAGS"] += ["-O0"]
else:
    CONFIG["LDFLAGS"] += ["-O2"]

def callCmd(command, print_out=False):
    with open("commands.txt", "a") as f:
        f.write(command+'\n')
    # Run the command and capture the output
    # result = subprocess.run(command, capture_output=False, text=True, shell=True)
    result = subprocess.run(command, capture_output=not print_out, text=True, shell=True)
    if result.returncode != 0:
        print(result.stderr)
    return [result.returncode, result.stdout]

callCmd("rm -rf commands.txt")

def checkExtension(file: str, valid_extensions: list[str]):
    for ext in valid_extensions:
        if file.endswith(ext):
            return True
    return False

def getExtension(file):
    return file.split(".")[-1]

def buildC(file):
    compiler = CONFIG["compiler"][0]
    options = CONFIG["CFLAGS"].copy()
    options.append("-std=c11")
    command = compiler + " " + file
    for option in options:
        command += " " + option
    print(f"C     {file}")
    command += f" -o {CONFIG['outDir'][0]}/{file}.o"
    return callCmd(command, True)[0]

def buildCXX(file):
    compiler = CONFIG["compiler"][0]
    if compiler == "gcc":
        compiler = "g"
    compiler += "++"
    options = CONFIG["CFLAGS"].copy()
    options += CONFIG["CXXFLAGS"].copy()
    options.append("-std=c++23")
    command = compiler + " " + file
    for option in options:
        command += " " + option
    print(f"CXX   {file}")
    command += f" -o {CONFIG['outDir'][0]}/{file}.o"
    return callCmd(command, True)[0]

def buildASM(file):
    compiler = "nasm"
    options = CONFIG["ASFLAGS"].copy()
    command = compiler + " " + file
    for option in options:
        command += " " + option
    print(f"AS    {file}")
    command += f" -o {CONFIG['outDir'][0]}/{file}.o"
    return callCmd(command, True)[0]

def buildKernel(kernel_dir: str):
    files = glob.glob(kernel_dir+'/**', recursive=True)
    CONFIG["INCPATHS"] += [f"-I{kernel_dir}"]
    for file in files:
        if not os.path.isfile(file):
            continue
        if not checkExtension(file, ["c", "cc", "asm"]):
            continue
        if getExtension(file) == "inc" or getExtension(file) == "h":
            continue
        basename = os.path.basename(os.path.dirname(os.path.realpath(__file__)))
        str_paths = ""
        for incPath in CONFIG["INCPATHS"]:
            str_paths += f" {incPath}"

        code, _ = callCmd(f"cpp {str_paths} -D_GLIBCXX_HOSTED=1 {file} -o ./tmp.txt", True)
        if code != 0:
            print(f"CPP failed to pre process {file}")
            exit(code)
        if not force_rebuild and compareFiles("./tmp.txt", os.path.abspath(f"/tmp/{basename}/cache/{file}")):
            continue
        callCmd(f"mkdir -p {CONFIG['outDir'][0]}/{os.path.dirname(file)}")
        callCmd(f"mkdir -p /tmp/{basename}/cache/{os.path.dirname(file)}")
        callCmd(f"cp ./tmp.txt /tmp/{basename}/cache/{file}")
        code = 0
        CONFIG["CFLAGS"] += CONFIG["INCPATHS"]
        if getExtension(file) == "c":
            code = buildC(file)
        elif getExtension(file) == "asm":
            code = buildASM(file)
        elif getExtension(file) == "cc":
            code = buildCXX(file)
        else:
            print(f"Invalid or unhandled extension `{getExtension(file)}` on file {file}")
            exit(1)

        for incPath in CONFIG["INCPATHS"]:
            CONFIG["CFLAGS"].remove(incPath)

        if code != 0:
            callCmd(f"rm -f /tmp/{basename}/cache/{file}")
            exit(code)

def linkKernel(kernel_dir, linker_file, static_lib_files=[]):
    files = glob.glob(kernel_dir+'/**', recursive=True)
    command = "ld"
    options = CONFIG["LDFLAGS"]
    for option in options:
        command += " " + option
    for file in files:
        if not os.path.isfile(file):
            continue
        if not checkExtension(file, ["o"]):
            continue
        command += " " + file
    command += f" -T {linker_file}"
    command += " --no-whole-archive"
    command += " --whole-archive"
    for static_lib in static_lib_files:
        command += f" {static_lib}"
    if "debug" in CONFIG.get("config"):
        command += f" -Map={CONFIG['outDir'][0]}/kernel.map"
    command += f" -o {CONFIG['outDir'][0]}/kernel.elf"
    file = f"{CONFIG['outDir'][0]}/kernel.elf"
    print(f"LD   {file}")
    if callCmd(command, True)[0] != 0:
        print(f"LD   {file} Failed")
        exit(1)
    callCmd(f"strip --keep-section-symbols --strip-debug {CONFIG['outDir'][0]}/kernel.elf -o {CONFIG['outDir'][0]}/kernel.elf")
    callCmd(f"objdump -C -d -Mintel {CONFIG['outDir'][0]}/kernel.elf > {CONFIG['outDir'][0]}/kernel.asm")

def makeImageFile(out_file):
    size = parseSize(CONFIG["imageSize"][0])
    divSize = parseSize("1M")
    command = f"dd if=/dev/zero of={out_file} bs=1M count={size//divSize}"
    print("> Making image file")
    callCmd(command)

def makePartitionTable(out_file):
    print("> Making GPT partition")
    command = f"parted {out_file} --script mklabel gpt"
    callCmd(command)
    print("> Making EFI partition")
    command = f"parted {out_file} --script mkpart EFI FAT32 2048s 100MB"
    callCmd(command)
    print("> Making root partition")
    command = f"parted {out_file} --script mkpart ROOT {CONFIG.get('imageFS')[0]} 100MB 100%"
    callCmd(command)
    print("> Setting EFI partition to be bootable")
    command = f"parted {out_file} --script set 1 boot on"
    callCmd(command)

def setupLoopDevice(out_file):
    print("> Setting up loop device")
    command = f"sudo losetup --show -f -P {out_file} > /tmp/tmp.txt"
    callCmd(command)
    loop_device = ""
    with open("/tmp/tmp.txt") as f:
        loop_device = f.readline()
    loop_device = loop_device.strip()
    print(f"> Loop device: {loop_device}")
    return loop_device

def makeFileSystem(loop_device):
    print("> Formatting file systems")
    callCmd(f"sudo mkfs.fat -F32 {loop_device}p1")
    callCmd(f"sudo mkfs.ext4 {loop_device}p2")

def mountFs(device, boot, kernel):
    callCmd(f"mkdir -p mnt")
    print("> Copying boot files")
    callCmd(f"sudo mount {device}p1 mnt")
    callCmd(f"sudo mkdir -p mnt/EFI/BOOT")
    callCmd(f"sudo cp {boot} mnt/EFI/BOOT")
    callCmd(f"sudo cp {kernel} mnt")
    if "limine-uefi" in CONFIG["bootloader"]:
        callCmd(f"sudo cp {CONFIG['outDir'][0]}/limine.conf mnt")
    callCmd(f"sudo umount mnt")
    callCmd(f"sudo losetup -d {device}")
    callCmd(f"rm -rf mnt")


def buildImage(out_file, boot_file, kernel_file):
    callCmd(f"rm -f {out_file}")
    makeImageFile(out_file)
    makePartitionTable(out_file)
    LOOP_DEVICE=setupLoopDevice(out_file)
    makeFileSystem(LOOP_DEVICE)
    mountFs(LOOP_DEVICE, boot_file, kernel_file)

def buildStaticLib(directory, out_file):
    os.makedirs(CONFIG["outDir"][0]+'/'+directory, exist_ok=True)
    CONFIG["INCPATHS"] += [f'-I{directory}']
    files = glob.glob(directory+'/**', recursive=True)
    for file in files:
        if not os.path.isfile(file):
            continue
        if not checkExtension(file, ["c", "cc", "asm"]):
            continue
        basename = os.path.basename(os.path.dirname(os.path.realpath(__file__)))
        str_paths = ""
        for incPath in CONFIG["INCPATHS"]:
            str_paths += f" {incPath}"
        callCmd(f"cpp {str_paths} {file} -o ./tmp.txt")
        if not force_rebuild and compareFiles("./tmp.txt", os.path.abspath(f"/tmp/{basename}/cache/{file}")):
            continue
        callCmd(f"mkdir -p {CONFIG['outDir'][0]}/{os.path.dirname(file)}")
        callCmd(f"mkdir -p /tmp/{basename}/cache/{os.path.dirname(file)}")
        callCmd(f"cp ./tmp.txt /tmp/{basename}/cache/{file}")
        code = 0
        CONFIG["CFLAGS"] += CONFIG["INCPATHS"]
        if getExtension(file) == "c":
            code = buildC(file)
        elif getExtension(file) == "asm":
            code = buildASM(file)
        elif getExtension(file) == "cc":
            code = buildCXX(file)
        else:
            print(f"Invalid or unhandled extension {getExtension(file)}")
            exit(1)

        for incPath in CONFIG["INCPATHS"]:
            CONFIG["CFLAGS"].remove(incPath)

        if code != 0:
            callCmd(f"rm -f /tmp/{basename}/cache/{file}")
            exit(code)

    files = glob.glob(f"{CONFIG['outDir'][0]}/{directory}/**", recursive=True)
    obj_files = []
    for file in files:
        if not os.path.isfile(file):
            continue
        if not checkExtension(file, ["o"]):
            continue
        obj_files.append(file)
    obj_files_str = " ".join(obj_files)
    cmd = f"ar rcs {out_file} {obj_files_str}"
    print(f"AR    {out_file}")
    callCmd(cmd)

def setupLimine():
    build_limine: bool = False
    if not os.path.exists("limine"):
        build_limine = True
    elif not os.path.exists("limine/bin/BOOTX64.EFI"):
        build_limine = True
    if build_limine:
        print("Building limine")
        callCmd("git clone --depth=1 https://github.com/limine-bootloader/limine", True)
        os.chdir("limine")
        callCmd("./bootstrap", True)
        callCmd("./configure --enable-uefi-x86-64", True)
        callCmd("make -B", True)
        os.chdir("..")
        callCmd("rm -rf ./limine/commands.txt")
    callCmd(f"cp ./util/limine.conf {CONFIG['outDir'][0]}/limine.conf")
    callCmd(f"cp ./limine/bin/BOOTX64.EFI {CONFIG['outDir'][0]}/BOOTX64.EFI")

def main():
    basename = os.path.basename(os.path.dirname(os.path.realpath(__file__)))
    if "clean" in sys.argv:
        callCmd(f"rm -rf /tmp/{basename}")
        callCmd(f"rm -rf {CONFIG['outDir'][0]}")
    if "clean-all" in sys.argv:
        callCmd(f"rm -rf limine")
        callCmd(f"rm -rf /tmp/{basename}")
        callCmd(f"rm -rf {CONFIG['outDir'][0]}")
    if force_rebuild:
        print("Rebuilding...")
    print("> Creating necesarry dirs")
    callCmd(f"mkdir -p {CONFIG['outDir'][0]}")
    callCmd(f"mkdir -p {CONFIG['outDir'][0]}/kernel")
    if 'limine-uefi' in CONFIG["bootloader"]:
        setupLimine()
    else:
        print("TODO: Other bootloaders")
        exit(1)
    print("> Building Libcxx")
    buildStaticLib("libcxx", f"{CONFIG['outDir'][0]}/libcxx.a")
    print("> Building drivers")
    buildStaticLib("drivers", f"{CONFIG['outDir'][0]}/drivers.a")
    print("> Building common")
    buildStaticLib("common", f"{CONFIG['outDir'][0]}/common.a")
    print("> Building kernel")
    buildKernel("kernel")
    print("> Linking kernel")
    linkKernel(f"{CONFIG['outDir'][0]}/kernel", "util/linker.ld", [f"{CONFIG['outDir'][0]}/libcxx.a", f"{CONFIG['outDir'][0]}/drivers.a", f"{CONFIG['outDir'][0]}/common.a"])
    if "compile" in sys.argv:
        return
    buildImage(f"{CONFIG['outDir'][0]}/image.img", f"{CONFIG['outDir'][0]}/BOOTX64.EFI", f"{CONFIG['outDir'][0]}/kernel.elf")
    if "run" in sys.argv:
        print("> Running QEMU")
        callCmd(f"./script/run.sh {CONFIG['outDir'][0]}", True)
    callCmd("chown -R luccie:luccie *")

if __name__ == '__main__':
    main()