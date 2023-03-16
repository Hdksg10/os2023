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
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(1);
}


int Open(const char * path, enum io_mode mode)
{
    int fd;
    mode_t oflag;

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
            break;
    }
    fd = open(path, oflag);
    if (fd < 0)
        unix_error("cannot open file");
    return fd;
}