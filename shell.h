//
// Created by Wu Mianzhi on 2023/3/9.
//

/* The main routine of shell read a line with max length MAX_LENGTH from stdin,
 * phrases it to command(defined in command.h) list, assigns the file descriptor
 * for each command and run it in a new process, if the command is not built-in,
 * shell(the child process) call execve() to run the program in shell's working
 * path. If the input command(s) is foreground, the main (and parent) process
 * suspend until all its child process terminated or receive the signal from
 * keyboard. The shell also manage a list of inputs(defined in history.h) to save
 * input commands it gets. The main routine should not terminate or stop even if
 * it receives SIGTSTP and SIGINT(virtually it sends these signals to child process)
 * unless the user use the exit command to exit it explicitly.
 *
 */

#ifndef UNTITLED1_SHELL_H
#define UNTITLED1_SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <assert.h>
#include <signal.h>

#include "command.h"
#include "history.h"
#include "utils.h"

#define MAX_LENGTH 1024
#define MAX_FN_LENGTH 32
#define MAX_COMMAND 64

static char line_buffer[MAX_LENGTH];
static struct command commands_buffer[MAX_COMMAND]; // the bound element is set argc == 0

// which way should process group run, foreground or background?
enum cgs{fg, bg};

int run_shell(void);

/* parse - parse an input cmdline into valid command vector(command_buffer)
 * parse function search all cmdline according to policy below:
 * search for '&' to determine whether foreground or background
 * search for '|' to split input cmdline into different command roughly
 * search for '<', '>', '>>', '2>' to assign file descriptor of stdin, stdout
 * and stderr for each command (see set_fd)
 * split command with delim ' ' (see set_argv)
 * parse function also open the file which command should write or read, and
 * create pipes for commands (see set_fd)
 * return value: 1 for background
 */
int parse(const char*);

void eval(void);

void wait_fg(void);

/* set_fd - set file descriptor from a null-terminated string
 * the routine search for substring ">>", "2>", "<", ">", open specified file,
 * and set file descriptor of command.
 * the routine also replace all these operators by "<" as terminated character
 * if ">>" found, the routine won't try to search for ">", since the user should
 * redirect stdout only once for a command
 */
void set_fd(char*, struct command*);

/* set_argv - set argv from a null-terminated string preprocessed by set_fd
 * the routine search for '<' first, then replace it with '\0', and split
 * substrings by ' '(space), replacing them with '\0'
 */
void set_argv(char*, struct command*);

/* check the content in commands_buffer */
void check_buffer(void);
#endif //UNTITLED1_SHELL_H
