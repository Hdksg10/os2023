#ifndef SHELL_JOBS_H
#define SHELL_JOBS_H

#include <unistd.h>

#include "utils.h"
/* jobs.h contains a few methods to manage jobs
 * list, foreground jobs now, to make sure the
 * parent process know its childs running state
 * and determine whether suspend or not
 * Since Minix does not support job control, shell 
 * doesn't implement fg & bg in that condition and
 * would not track background job's state
 */

#define MAX_JOBS 16

enum st{undef, bg, fg, stop};

struct job
{
    pid_t pid;
    int jid;
    enum st st;
    char* cmd;
};

int initialize_jobs();
int add_job(pid_t, int, char*);
int del_job(pid_t);
int maxjid(void);
struct job * get_jpid(pid_t);
struct job * get_jjid(int);
struct job * get_fg(void);

void list_jobs(void);
void free_jobs(void);
#endif