//
// Created by Wu Mianzhi on 2023/3/13.
//

#ifndef UNTITLED1_UTILS_H
#define UNTITLED1_UTILS_H

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

void unix_error(const char*);
void app_error(const char*);

enum io_mode{in, out, append};
int Open(const char*, enum io_mode);

#endif //UNTITLED1_UTILS_H
