#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <assert.h>

#define NRROUND 1024
#define FILEROUNDUP (NRROUND * 128 * 1024)
const static char* path_format[2] = {"/root/myram/ram_%d", "/usr/disk_%d"};

void read_file(char* filepath, unsigned block_size, int random);
void write_file(char* filepath, unsigned block_size, int random);
void init_file(char* filepath);
void single_test(unsigned concurrency, unsigned block_size, int random, int disk);
typedef struct timeval timeval_t;
double get_time_left(timeval_t st, timeval_t ed)
{
    return (ed.tv_usec - st.tv_usec) / 100000.0;
}

void init_file(char* filepath)
{
    int fd = 0;
    fd = open(filepath, O_CREAT | O_RDWR | O_TRUNC, S_IRWXU);
    if (fd < 0)
        fprintf(stderr, "Cannot create file: %s", filepath);
    char junk[1024];
    memset(junk, 5, 1024);
    for (int i = 0; i < NRROUND * 64; i++)
    {
        unsigned write_bytes = write(fd, junk, 1024);
        if (write_bytes != 1024)
        {
            fprintf(stderr, "Error occurred when init file: %s", filepath);
            return;
        }
    }
    close(fd);
}

void write_file(char* filepath, unsigned block_size, int random)
{
    static char byte8[9] = "8bytestr";
    int fd = 0;
    fd = open(filepath, O_CREAT | O_RDWR | O_SYNC, S_IRWXU);
    if (fd < 0)
        fprintf(stderr, "Cannot open file: %s", filepath);
    
    char* buffer = malloc(block_size);
    for (int i = 0; i < block_size / 8; i++)
    {
        strcat(buffer, byte8);
    }
    for (int i = 0; i < NRROUND; i++)
    {
        if (random)
        {
            lseek(fd, rand() % (FILEROUNDUP - block_size), SEEK_SET);
        }
        unsigned write_bytes = write(fd, buffer, block_size);
        assert(write_bytes == block_size);
        // if (write_bytes < block_size)
        //     i--;
    }
    free(buffer);
    lseek(fd, 0, SEEK_SET);
    close(fd);
}

void read_file(char* filepath, unsigned block_size, int random)
{
    int fd = 0;
    fd = open(filepath, O_CREAT | O_RDWR | O_SYNC, S_IRWXU);
    if (fd < 0)
        fprintf(stderr, "Cannot open file: %s", filepath);
    
    char* buffer = malloc(block_size);
    for (int i = 0; i < NRROUND; i++)
    {
        if (random)
        {
            lseek(fd, rand() % (FILEROUNDUP - block_size), SEEK_SET);
        }
        unsigned read_bytes = read(fd, buffer, block_size);
        assert(read_bytes == block_size);
        // if (read_bytes < block_size)
        //     i--;
    }
    free(buffer);
    lseek(fd, 0, SEEK_SET);
    close(fd);
}

void single_test(unsigned concurrency, unsigned block_size, int random, int disk)
{
    srand(time(0));
    const static char* storage[2] = {"ram", "disk"};
    char path[16];
    double interval = 0;
    long filesize = NRROUND * concurrency * block_size;
    timeval_t start_time, end_time;

    printf("Testing: blocksize = %u, concurrency = %u, storage = %s, random = %d\n",
            block_size, concurrency, storage[disk], random);
    
    /* init file */
    for (int i = 0; i < concurrency; i++)
    {
        sprintf(path, path_format[disk], i);
        init_file(path);
    }
    printf("File init done\n");

    gettimeofday(&start_time, NULL);
    for (int i = 0; i < concurrency; i++)
    {
        if (fork() == 0)
        {
            sprintf(path, path_format[disk], i);
            read_file(path, block_size, random);
            exit(0);
        }
    }
    for (int i = 0; i < concurrency; i++)
        wait(NULL);
    gettimeofday(&end_time, NULL);
    interval = get_time_left(start_time, end_time);
    printf("Test read done: time = %lf, filesize = %ld, throughout = %lf\n", interval, filesize, filesize / interval);

    gettimeofday(&start_time, NULL);
    for (int i = 0; i < concurrency; i++)
    {
        if (fork() == 0)
        {
            sprintf(path, path_format[disk], i);
            write_file(path, block_size, random);
            exit(0);
        }
    }
    for (int i = 0; i < concurrency; i++)
        wait(NULL);
    gettimeofday(&end_time, NULL);
    interval = get_time_left(start_time, end_time);
    printf("Test write done: time = %lf, filesize = %ld, throughout = %lf\n", interval, filesize, filesize / interval);
}

int main(int argc, char** argv)
{
    if (argc != 4){
        fprintf(stderr, "Usage: utils <NRProcess> <Blocksize> <random>\n");
        return 0;
    }
    unsigned concurrency = atoi(argv[1]);
    unsigned blocksize  = atoi(argv[2]);
    int random = atoi(argv[3]);
    printf("RAM Test:\n");
    single_test(concurrency, blocksize, random, 0);
    printf("Disk Test:\n");
    single_test(concurrency, blocksize, random, 1);
    return 0;
}