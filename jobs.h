#ifndef SHELL_JOBS_H
#define SHELL_JOBS_H

#include <unistd.h>

#include "utils.h"
/* jobs.h contains a few methods to manage jobs
 * list, foreground jobs now, to make sure the
 * parent process know its childs running state
 * and determine whether suspend or not
 * Since Minix does not support job control, that
 * is, cannot set process group id, the pgid would 
 * always be 0, so when the shell find getpgid() fail,
 * it won't allow fg & bg in that condition and
 * would not track background job's state
 */

/* We maintain job list as an array, with
 * its element is a list of process, the
 * first node in list is the leader of this
 * process group, that is pid = pgid
 */

#define MAX_JOBS 16

enum st{undef, fg, bg, stop};

struct job
{
    pid_t pgid;
    int jid;
    enum st st;
    char* cmd;
    struct process * leader;
    int count;
};

struct process
{
    pid_t pid;
    struct process * next;
    //char* cmd;
};

int initialize_jobs();
int add_job(pid_t, int, char*, int);
// job->count--, if job->count = 0, delete job
int del_job(pid_t);
int maxjid(void);
struct job * get_jpgid(pid_t);
struct job * get_jjid(int);
struct job * get_fg(void);

int pid2jid(pid_t pid);
void add_process(int jid, pid_t pid);
void del_process(int jid, pid_t pid);

void list_jobs(void);
void free_jobs(void);
#endif