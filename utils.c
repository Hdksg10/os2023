//
// Created by Wu Mianzhi on 2023/3/13.
//
#include "utils.h"

void app_error(const char* msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

void unix_error(const char* msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}


int Open(const char * path, enum io_mode mode)
{
    int fd;
    mode_t oflag;

    mode_t pflag; // set new file permissions
    pflag = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH;

    switch (mode) {
        case in:
            oflag = O_RDONLY;
            break;
        case out:
            oflag = O_WRONLY | O_CREAT | O_TRUNC;
            break;
        case append:
            oflag = O_WRONLY | O_APPEND | O_CREAT;
            break;
        default:
            oflag = 0;
            break;
    }
    fd = open(path, oflag, pflag);
    if (fd < 0)
        unix_error("cannot open file");
    return fd;
}

void Close(int fd)
{
    if (close(fd) < 0)
        unix_error("cannot close fd");
}

int Dup(int fd)
{
    int new_fd = 0;
    if ((new_fd = dup(fd)) < 0)
        unix_error("cannot duplicate fd");
    return new_fd;
}

pid_t Fork()
{
    pid_t pid;
    if ((pid = fork()) < 0)
        unix_error("fork error");
    return pid;
}

int Execve(const char* path, char** argv, char** envp)
{
    int result;

    char buffer[NAME_MAX];
    char searching_path[3][16] = {"./", "/bin/", "/usr/bin"};

    for (int i = 0; i < 3; i++)
    {
        strcpy(buffer, searching_path[i]);
        strcat(buffer, path);
        result = execve(buffer, argv, envp);
    }

    return result;
}

void Pipe(int pipefd[2])
{
    if (pipe(pipefd) < 0)
        unix_error("cannot create pipe");
}

int Chdir(const char* path)
{
    if (chdir(path) < 0 && errno != ENOENT)
        unix_error("cannot change working dir");
    else if (errno == ENOENT) // no such dir
        return -1;
    return 0;
}

void Getcwd(char* buffer, size_t size)
{
    if (!getcwd(buffer, size))
    {
        unix_error("working directory pathname is too long");
    }
}