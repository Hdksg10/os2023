#include "jobs.h"

static struct job jobs[MAX_JOBS];
static int next_jid = 1;
int jobcontrol = 1; // global mask


/* a few methods help control joblist*/
static void free_process(struct process* head)
{
    if (!head)  return; // empty proc list
    struct process * proc = head; 
    struct process * next; 
    while (proc->next)
    {
        next = proc->next;
        free(proc);
        proc = next;
    }
    free(proc); // free the last process
}

// insert into the end of list
void add_process(int jid, pid_t pid)
{
    struct process * head = jobs[jid].leader;
    struct process * proc; 
    if (!head) //empty list
    {
        proc = malloc(sizeof(struct process));
        memset(proc, 0, sizeof(struct process));
        proc->pid = pid;
        jobs[jid].leader = proc;
    }
    else
    {
        proc = head;
        while (proc->next) proc = proc->next;
        struct process * new_proc = malloc(sizeof(struct process)); //new process
        memset(new_proc, 0, sizeof(struct process));
        proc->next = new_proc;
    }
}


int initialize_jobs()
{
    if (getpgrp() != getpid()) // test job control
        jobcontrol = 0;
    for (int job_index = 0; job_index < MAX_JOBS; job_index++)
    {
        jobs[job_index].st = undef;
        jobs[job_index].cmd = NULL;
        jobs[job_index].pgid = 0;
        jobs[job_index].jid = 0;
        jobs[job_index].count = 0;
        jobs[job_index].leader = NULL;
    }
    return 0;
}

int add_job(pid_t pgid, int st, char* cmd, int count)
{
    for (int job_index = 0; job_index < MAX_JOBS; job_index++)
    {
        if (jobs[job_index].st == undef)
        {
            jobs[job_index].st = st;
            jobs[job_index].pgid = pgid;
            jobs[job_index].jid = next_jid++;
            jobs[job_index].cmd = malloc(strlen(cmd)+1);
            jobs[job_index].count = count;
            strcpy(jobs[job_index].cmd, cmd);
            if (next_jid > MAX_JOBS)
                next_jid = 1;
            return 1;
        }
    }
    printf("jobs limit exceeds\n");
    return 0;
}

int del_job(pid_t pgid)
{
    for (int job_index = 0; job_index < MAX_JOBS; job_index++)
    {
        if (jobs[job_index].pgid == pgid)
        {
            if (jobs[job_index].count)
                jobs[job_index].count--;
            else
            {
                jobs[job_index].st = undef;
                jobs[job_index].pgid = 0;
                jobs[job_index].jid = 0;
                free(jobs[job_index].cmd);
                jobs[job_index].cmd = NULL;
                next_jid = maxjid() + 1;
            }
            return 1;
        }
    }
    return 0;
}

int maxjid()
{
    int jid = 0;
    for (int job_index = 0; job_index < MAX_JOBS; job_index++)
    {
        if (jobs[job_index].jid > jid)
        {
            jid = jobs[job_index].jid;
        }
    }
    return jid;
}
struct job * get_jpgid(pid_t pgid)
{
    for (int job_index = 0; job_index < MAX_JOBS; job_index++)
    {
        if (jobs[job_index].pgid == pgid)
        {
            return &jobs[job_index];
        }
    }
    return NULL;
}
struct job * get_jjid(int jid)
{
    for (int job_index = 0; job_index < MAX_JOBS; job_index++)
    {
        if (jobs[job_index].jid == jid)
        {
            return &jobs[job_index];
        }
    }
    return NULL;
}
struct job * get_fg(void)
{
    for (int job_index = 0; job_index < MAX_JOBS; job_index++)
    {
        if (jobs[job_index].st == fg)
        {
            return &jobs[job_index];
        }
    }
    return NULL;
}

int pid2jid(pid_t pid)
{
    for (int job_index = 0; job_index < MAX_JOBS; job_index++)
    {
        struct process* proc = jobs[job_index].leader;
        while (proc)
        {
            if (proc->pid == pid)
                return job_index;
            proc = proc->next;
        }
    }
    return -1;
}

static void list_processes(int jid)
{
    
}

void list_jobs(void)
{
    for (int job_index = 0; job_index < MAX_JOBS; job_index++)
    {
        if (jobs[job_index].st != undef)
        {
            printf("[%d] (%d) ", jobs[job_index].jid, jobs[job_index].pgid);
            switch (jobs[job_index].st) {
            case fg: 
                printf("Running ");
                break;
            case bg: 
                printf("Foreground ");
                break;
            case stop: 
                printf("Stopped ");
                break;
            default:
		        printf("listjobs: Internal error: job[%d].state=%d ", 
			          job_index, jobs[job_index].st);
            }
            printf("%s\n", jobs[job_index].cmd);
        }
    }
}

void free_jobs()
{
    for (int job_index = 0; job_index < MAX_JOBS; job_index++)
    {
        if (jobs[job_index].st != undef)
        {
            free(jobs[job_index].cmd);
            free_process(jobs[job_index].leader);
        }
    }
}