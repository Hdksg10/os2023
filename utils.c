#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#define NRROUND 1024
#define FILEROUNDUP (NRROUND * 64 * 1024)

double read_file(char* filepath, unsigned block_size, int random);
double write_file(char* filepath, unsigned block_size, int random);
void init_file(char* filepath);

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

double write_file(char* filepath, unsigned block_size, int random)
{
    char byte8[9] = "8bytestr";
    clock_t st, et;
    double writetime = 0.0;
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
            lseek(fd, rand() % FILEROUNDUP, SEEK_SET);
        }
        st = clock();
        unsigned write_bytes = write(fd, buffer, block_size);
        et = clock();
        if (write_bytes < block_size)
            i--;
        else
            writetime += (et - st) / CLOCKS_PER_SEC;
    }
    free(buffer);
    lseek(fd, 0, SEEK_SET);
    close(fd);
    return writetime;
}

double read_file(char* filepath, unsigned block_size, int random)
{
    clock_t st, et;
    double readtime = 0.0;
    int fd = 0;
    fd = open(filepath, O_CREAT | O_RDWR | O_SYNC, S_IRWXU);
    if (fd < 0)
        fprintf(stderr, "Cannot open file: %s", filepath);
    
    char* buffer = malloc(block_size);
    for (int i = 0; i < NRROUND; i++)
    {
        if (random)
        {
            lseek(fd, rand() % FILEROUNDUP, SEEK_SET);
        }
        st = clock();
        unsigned read_bytes = read(fd, buffer, block_size);
        et = clock();
        if (read_bytes < block_size)
            i--;
        else
            readtime += (et - st) / CLOCKS_PER_SEC;
    }
    free(buffer);
    lseek(fd, 0, SEEK_SET);
    close(fd);
    return readtime;
}

int main()
{
    char byte8[9] = "8bytestr";
    unsigned busz = 64;
    char* buffer = malloc(busz);
    for (int i = 0; i < busz / 8; i++)
    {
        strcat(buffer, byte8);
    }
    printf("%d\n", strlen(buffer));
}