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
#define FILEROUNDUP 1024
#define FILEROUNDUPBYTES (FILEROUNDUP * 1024 * 1024) // total file size (1024MB)
#define PFILEROUNDUP 70
#define PFILEROUNDUPBYTES (PFILEROUNDUP * 1024 * 1024)
const static char* path_format[2] = {"/root/myram/ram_%d", "/usr/disk_%d"};

void read_file(int fd, unsigned block_size, int random);
void write_file(int fd, unsigned block_size, int random);
int init_file(char* filepath, unsigned sz);
void single_test(unsigned concurrency, unsigned block_size, int random, int disk, int file[]);
typedef struct timeval  timeval_t;
double get_time_left(timeval_t st, timeval_t ed)
{
    return (ed.tv_usec - st.tv_usec) / 1e6 + (ed.tv_sec - st.tv_sec);
}

int init_file(char* filepath, unsigned sz)
{
    int fd = 0;
    fd = open(filepath, O_CREAT | O_RDWR | O_TRUNC | O_SYNC, S_IRWXU);
    if (fd < 0)
        fprintf(stderr, "Cannot create file: %s", filepath);
    char junk[1024];
    memset(junk, 5, 1024);
    for (int i = 0; i < sz * 1024; i++)
    {
        unsigned write_bytes = write(fd, junk, 1024);
        if (write_bytes != 1024)
        {
            fprintf(stderr, "Error occurred when init file: %s\n", filepath);
            return -1;
        }
    }
    lseek(fd, 0, SEEK_SET);
    return fd;
}

void write_file(int fd, unsigned block_size, int random)
{
    
    char* buffer = malloc(block_size * sizeof(char) + 1);
    for (int i = 0; i < NRROUND; i++)
    {
        if (random)
        {
            lseek(fd, rand() % (PFILEROUNDUPBYTES - block_size), SEEK_SET);
        }
        unsigned write_bytes = write(fd, buffer, block_size);
        assert(write_bytes == block_size);
    }
    free(buffer);
    lseek(fd, 0, SEEK_SET);
    close(fd);
}

void read_file(int fd, unsigned block_size, int random)
{
    char* buffer = malloc(block_size);
    for (int i = 0; i < NRROUND; i++)
    {
        if (random)
        {
            lseek(fd, rand() % (PFILEROUNDUPBYTES - block_size), SEEK_SET);
        }
        unsigned read_bytes = read(fd, buffer, block_size);
        assert(read_bytes == block_size);
    }
    free(buffer);
    lseek(fd, 0, SEEK_SET);
    close(fd);
}

void single_test(unsigned concurrency, unsigned block_size, int random, int disk, int file[])
{
    srand(time(0));
    const static char* storage[2] = {"ram", "disk"};
    double interval = 0;
    double datasize = (NRROUND * concurrency * block_size) / (double)(1024 * 1024);

    timeval_t start_time, end_time;

    printf("Testing: blocksize = %u, concurrency = %u, storage = %s, random = %d\n",
            block_size, concurrency, storage[disk], random);
    
    gettimeofday(&start_time, NULL);
    for (int i = 0; i < concurrency; i++)
    {
        if (fork() == 0)
        {
            read_file(file[i], block_size, random);
            exit(0);
        }
    }
    for (int i = 0; i < concurrency; i++)
        wait(NULL);
    gettimeofday(&end_time, NULL);
    interval = get_time_left(start_time, end_time);
    printf("Test read done: time = %lf, filesize = %lf, throughput = %lf\n", interval, datasize, datasize / interval);

    gettimeofday(&start_time, NULL);
    for (int i = 0; i < concurrency; i++)
    {
        if (fork() == 0)
        {
            write_file(file[i], block_size, random);
            exit(0);
        }
    }
    for (int i = 0; i < concurrency; i++)
        wait(NULL);
    gettimeofday(&end_time, NULL);
    interval = get_time_left(start_time, end_time);
    printf("Test write done: time = %lf, filesize = %lf, throughput = %lf\n", interval, datasize, datasize / interval);
}

// NOTE: deprecated
void concurrency_test(unsigned lower, unsigned upper)
{
    const static unsigned blocksize = 4096; // fixed blocksize(16 KB)
    unsigned concurrency;
    unsigned filesize;
    printf("Searching for concurrency\n");
    printf("RAM Test:\n");
    for (concurrency = lower; concurrency < upper; concurrency++)
    {
        filesize = FILEROUNDUP / concurrency;
        single_test(concurrency, blocksize, 0, 0, filesize);
    }
    printf("Disk Test:\n");
    for (concurrency = lower; concurrency < upper; concurrency++)
    {
        filesize = FILEROUNDUP / concurrency;
        single_test(concurrency, blocksize, 0, 1, filesize);
    }
}

void block_test(unsigned concurrency)
{
    const static int nrsize = 6;
    const static unsigned size[nrsize] = {64, 256, 1024, 4096, 16384, 65536}; //bytes  
    char path[32]; 
    int file[concurrency];
    printf("Testing different block sizes...\n");
    /* init file */
    for (int i = 0; i < concurrency; i++)
    {
        sprintf(path, path_format[0], i);
        file[i] = init_file(path, PFILEROUNDUP);
    }
    printf("File init done\n");
    printf("RAM Test: Order\n");
    for (int i = 0; i < nrsize; i++)
    {
        single_test(concurrency, size[i], 0, 0, file);
    }
    printf("RAM Test: Random\n");
    for (int i = 0; i < nrsize; i++)
    {
        single_test(concurrency, size[i], 1, 0, file);
    }
    /* init file */
    for (int i = 0; i < concurrency; i++)
    {
        sprintf(path, path_format[1], i);
        file[i] = init_file(path, PFILEROUNDUP);
    }
    printf("Disk Test: Order\n");
    for (int i = 0; i < nrsize; i++)
    {
        single_test(concurrency, size[i], 0, 1, file);
    }
    printf("Disk Test: Random\n");
    for (int i = 0; i < nrsize; i++)
    {
        single_test(concurrency, size[i], 1, 1, file);
    }
}

int main(int argc, char** argv)
{
    if (argc != 3 && argc != 4){
        fprintf(stderr, "Usage: utils -c <lower> <upper> | utils -b <concurrency>\n");
        return 0;
    }
    if (!strcmp(argv[1], "-c"))
    {
        if (argc != 4)
        {
            fprintf(stderr, "Usage: utils -c <lower> <upper>\n");
            return 0;
        }
        concurrency_test(atoi(argv[2]), atoi(argv[3]));
    }
    else if (!strcmp(argv[1], "-b"))
    {
        if (argc != 3)
        {
            fprintf(stderr, "Usage: utils -b <concurrency>\n");
            return 0;
        }
        block_test(atoi(argv[2]));
    }
    else
        fprintf(stderr, "Usage: utils -c <lower> <upper> | utils -b <concurrency>\n");
    return 0;
}