#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <ctype.h>

#undef BLOCK_SIZE
#define BLOCK_SIZE 512
#define SUPERBLOCK_TYPE 1
#define DIRECTORY_TYPE 2
uint64_t start_offset;

void die(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

const char* readFile(const char* path){
    FILE* f = fopen(path, "rb");
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    const char* data = malloc(size+1);
    fread((char*)data, 1, size, f);
    return data;
}

void get_partition_offset(const char *device) {
    if (*(char*)((size_t)device + strlen(device) - 2) == 'p' && isdigit(*(char*)((size_t)device + strlen(device) - 1))){
        const char* data = device;
        device = malloc(strlen(data)-1);
        snprintf((char*)device, strlen(data)-1, "%s", data);
    }
    char* sys;
    sys = malloc(strlen("parted -s %s unit B print | awk 'NR>8 {gsub(/[^[:digit:]]/, \"\", $3); print $2; exit}' > tmp.txt") + strlen(device));
    sprintf(sys, "parted -s %s unit B print | awk 'NR>8 {gsub(/[^[:digit:]]/, \"\", $3); print $2; exit}' > tmp.txt", device);
    system(sys);
    const char* data = readFile("tmp.txt");
    char* data2 = malloc(strlen(data)-1);
    snprintf(data2, strlen(data)-1, "%s", data);
    start_offset = strtoul(data2, NULL, 10) / BLOCK_SIZE;
}

void write_superblock(FILE* fd) {
    uint8_t block[BLOCK_SIZE] = {0};
    block[0] = SUPERBLOCK_TYPE;
    memcpy(&block[1], &start_offset, sizeof(uint64_t));
    uint64_t root_lba = start_offset + 1;
    memcpy(&block[9], &root_lba, sizeof(uint64_t));
    if (fwrite(block, 1, BLOCK_SIZE, fd) != BLOCK_SIZE)
        die("Writing superblock failed");
}

void write_root_directory(FILE* fd) {
    uint8_t block[BLOCK_SIZE] = {0};
    block[0] = DIRECTORY_TYPE;
    uint64_t next = 0;
    uint64_t name = 0;
    uint32_t count = 0;
    uint64_t root_lba = start_offset + 1;
    memcpy(&block[1], &root_lba, sizeof(uint64_t));
    memcpy(&block[9], &next, sizeof(uint64_t));
    memcpy(&block[17], &name, sizeof(uint64_t));
    memcpy(&block[25], &count, sizeof(uint32_t));
    fseeko(fd, BLOCK_SIZE, SEEK_SET);
    if (fwrite(block, 1, BLOCK_SIZE, fd) != BLOCK_SIZE)
        die("Writing superblock failed");
}

void zero_remaining_space(FILE* fd, uint64_t current_pos, uint64_t total_size) {
    uint8_t zero_block[BLOCK_SIZE] = {0};
    uint64_t remaining = total_size - current_pos;

    while (remaining > 0) {
        size_t to_write = (remaining > BLOCK_SIZE) ? BLOCK_SIZE : remaining;
        if (fwrite(zero_block, 1, to_write, fd) != to_write)
            die("Zeroing file failed");
        remaining -= to_write;
    }
    sync();
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <device or image>\n", argv[0]);
        return EXIT_FAILURE;
    }
    FILE* fd = fopen(argv[1], "r+b");
    if (!fd)
        die("open");
    get_partition_offset(argv[1]);
    printf("Partition start offset: %lu bytes\n", start_offset);
    fseeko(fd, 0, SEEK_SET);
    write_superblock(fd);
    write_root_directory(fd);
    sync();
    fclose(fd);
    printf("Filesystem created on %s\n", argv[1]);
    return EXIT_SUCCESS;
}