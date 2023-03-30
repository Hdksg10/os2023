//
// Created by Wu Mianzhi on 2023/3/13.
//

#ifndef SHELL_UTILS_H
#define SHELL_UTILS_H

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#ifndef NAME_MAX
#define NAME_MAX 64
#endif

void unix_error(const char*);
void app_error(const char*);

enum io_mode{in, out, append};
int Open(const char*, enum io_mode);
void Close(int);
int Dup(int);

void Pipe(int *);
pid_t Fork();

/* Exceve - call exceve in different path 
 * searching path:
 * ./
 * /bin/
 * /usr/bin/
 */
int Execve(const char* path, char** argv, char** envp);

int Chdir(const char*);
void Getcwd(char*, size_t);
#endif //SHELL_UTILS_H
