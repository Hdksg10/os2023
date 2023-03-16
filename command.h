//
// Created by Wu Mianzhi on 2023/3/9.
//

#ifndef UNTITLED1_COMMAND_H
#define UNTITLED1_COMMAND_H
//

#define MAX_ARGS 15

struct command
{
    // arg vector used for command, each argument should be null-terminated string, argv[0] is the command, argv[argc] = 0
    char* argv[MAX_ARGS+1];
    // count of arguments
    int argc;

    /* file descriptor used
     * 0: stdin, 0 for default
     * 1: stderr, 2 for default
     * 2: stdout, 1 for default
     * the reason that set stdout as the last element is to simplify redirection(shell.h/set_fd)
     */
    int fd[3];

};

#endif //UNTITLED1_COMMAND_H
