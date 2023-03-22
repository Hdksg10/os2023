#ifndef SHELL_BUILTINCMD_H
#define SHELL_BUILTINCMD_H
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <pwd.h>
#include <sys/stat.h>


#include "utils.h"
#define PAGE_MAX 32
#define PROC_MAX 512


struct proc{
    char state;
    unsigned long ticks;
    int priority;
    unsigned long uid;
    char name[NAME_MAX];
    unsigned long memory;
    int nice;
    char username[NAME_MAX];
    int pid;
};

void top();


#endif