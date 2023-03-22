#ifndef SHELL_BUILTINCMD_H
#define SHELL_BUILTINCMD_H
//
// Created by Wu Mianzhi on 2023/3/16.
//
#include <stdlib.h>

#include "history.h"
#include "wpath.h"
#include "jobs.h"
#include "top.h"

// builtin command code
#define BUILTIN_CD   0 //wpath.h
#define BUILTIN_PWD  1 //wpath.h
#define BUILTIN_HIST 2 //history.h
#define BUILTIN_EXIT 3 //builtincmd.h
#define BUILTIN_FG   4 //jobs.h
#define BUILTIN_BG   5 //jobs.h
#define BUILTIN_JOBS 6 //jobs.h
#define BUILTIN_TOP  7 //top.h

// builtin command declarion
void builtin_exit();
#endif //SHELL_BUILTINCMD_H