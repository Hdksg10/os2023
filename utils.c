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
#define PFILEROUNDUP 60
#define PFILEROUNDUPBYTES (PFILEROUNDUP * 1024 * 1024)
const static char* path_format[2] = {"/root/myram/ram_%d", "/usr/disk_%d"};
static unsigned filesize; // file size (MB)

void read_file(char* filepath, unsigned block_size, int random);
void write_file(char* filepath, unsigned block_size, int random);
void init_file(char* filepath, unsigned sz);
void single_test(unsigned concurrency, unsigned block_size, int random, int disk);
typedef struct timeval timeval_t;
double get_time_left(timeval_t st, timeval_t ed)
{
    return (ed.tv_usec - st.tv_usec) / 1000.0 + (ed.tv_sec - st.tv_sec) * 1000;
}

void init_file(char* filepath, unsigned sz)
{
    int fd = 0;
    fd = open(filepath, O_CREAT | O_RDWR | O_TRUNC | O_SYNC, S_IRWXU);
    if (fd < 0)
        fprintf(stderr, "Cannot create file: %s", filepath);
    char junk[1024];
    memset(junk, 5, 1024);
    for (int i = 0; i < PFILEROUNDUP * 1024; i++)
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

// void write_file(char* filepath, unsigned block_size, int random)
// {
//     static char byte8[9] = "8bytestr";
//     int fd = 0;
//     fd = open(filepath, O_CREAT | O_RDWR | O_SYNC, S_IRWXU);
//     if (fd < 0)
//         fprintf(stderr, "Cannot open file: %s", filepath);
    
//     char* buffer = malloc(block_size);
//     for (int i = 0; i < block_size / 8; i++)
//     {
//         strcat(buffer, byte8);
//     }
//     for (int i = 0; i < NRROUND; i++)
//     {
//         if (random)
//         {
//             lseek(fd, rand() % (PFILEROUNDUPBYTES - block_size), SEEK_SET);
//         }
//         unsigned write_bytes = write(fd, buffer, block_size);
//         assert(write_bytes == block_size);
//         // if (write_bytes < block_size)
//         //     i--;
//     }
//     free(buffer);
//     lseek(fd, 0, SEEK_SET);
//     close(fd);
// }
void write_file(char* filepath, unsigned block_size, int random)
{
    const int BUFSIZE = 64;
    static char buffer[BUFSIZE] = "This is a 6KB block!";
    int fd = 0;  
    fd = open(filepath, O_CREAT | O_RDWR | O_SYNC, S_IRWXU);  
    if(fd < 0){  
        fprintf(stdout, "Error occurred when opening file!");  
        return;  
    }  
    char *buf_ext = (char *)malloc(sizeof(char) * block_size);  
    for(int i = 0; i < block_size / BUFSIZE; i++){  
        strcat(buf_ext, buffer);  
    }  
    for(int i = 0; i < NRROUND; i++){  
        unsigned writebytes = write(fd, buf_ext, block_size);  
        if(random){  
            lseek(fd, rand() % ((block_size - 1) * NRROUND), SEEK_SET);  
        }  
        assert(writebytes == block_size);
    }  
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
            lseek(fd, rand() % (PFILEROUNDUPBYTES - block_size), SEEK_SET);
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
    char path[32];
    double interval = 0;
    long datasize = (NRROUND * concurrency * block_size) / (1024 * 1024);
    timeval_t start_time, end_time;

    printf("Testing: blocksize = %u, concurrency = %u, storage = %s, random = %d\n",
            block_size, concurrency, storage[disk], random);
    
    /* init file */
    for (int i = 0; i < concurrency; i++)
    {
        sprintf(path, path_format[disk], i);
        init_file(path, filesize);
    }
    printf("File init done\n");

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
    interval = get_time_left(start_time, end_time) / 1000.0;
    printf("Test write done: time = %lf, filesize = %ld, throughout = %lf\n", interval, datasize, datasize / interval);

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
    interval = get_time_left(start_time, end_time) / 1000.0;
    printf("Test read done: time = %lf, filesize = %ld, throughout = %lf\n", interval, datasize, datasize / interval);
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
    filesize = FILEROUNDUP / concurrency;
    for (int i = 5 ; i < concurrency; i++)
    {
        printf("RAM Test:\n");
        single_test(i, blocksize, random, 0);
        printf("Disk Test:\n");
        single_test(i, blocksize, random, 1);
    }
    return 0;
}