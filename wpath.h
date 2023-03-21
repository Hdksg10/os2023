#ifndef SHELL_WPATH_H
#define SHELL_WPATH_H
//
// Created by Wu Mianzhi on 2023/3/16.
//
/* Methods defined here is designed to change working
 * directory and print current working directory, implement
 * shell command pwd and cd
 * Sinch methods here change the environment of shell,
 * command pwd and cd are builtin commands
 */
#include <unistd.h>
#include "utils.h"

#define MAX_PATH_LENGTH 128
void cd(char**);
void pwd(void);
#endif //SHELL_WPATH_H