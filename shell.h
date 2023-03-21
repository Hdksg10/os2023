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
#include <sys/wait.h>

#include "command.h"
#include "utils.h"
#include "builtincmd.h"

#define MAX_LENGTH 1024
#define MAX_FN_LENGTH 32
#define MAX_COMMAND 64

extern char **environ;
extern int jobcontrol;

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

/* eval - run all commands in commands_buffer.
 * the routine fork a new process for each command in commands_buffer
 * if fg is set, the routine(parent process) suspend and wait until all 
 * child processes terminated.(see wait_fg)
 * the routine maintainss a job list to track all jobs state.
 * the routine assign fd stdin, stdout, stderr according to commands.fd
 * the routine check whether the command is built-in command or not,
 * use syscall execv to run not built-in command.
 */
void eval(const char*);

/* builtin_command - check if a command is builtin and run it
 * if command is builtin, the routine run it in current process(by prejudece call)
 * if not, return 0 immediately
 * if the builtin command is set to run background, the routine fork a new process
 * and run it. Normally, it would not have any impact on current shell, same as bash
 * the routine also assign file descriptor for builtin command
 */
int builtin_command(struct command *, int);

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

#ifdef SHELL_DEBUG
/* check the content in commands_buffer */
void check_buffer(void);
#endif

#endif //UNTITLED1_SHELL_H
