#include "jobs.h"

static struct job jobs[MAX_JOBS];
static int next_jid = 1;

int initialize_jobs()
{
    for (int job_index = 0; job_index < MAX_JOBS; job_index++)
    {
        jobs[job_index].st = undef;
        jobs[job_index].cmd = NULL;
        jobs[job_index].pid = 0;
        jobs[job_index].jid = 0;
    }
    return 0;
}

int add_job(pid_t pid, int st, char* cmd)
{
    for (int job_index = 0; job_index < MAX_JOBS; job_index++)
    {
        if (jobs[job_index].st == undef)
        {
            jobs[job_index].st = st;
            jobs[job_index].pid = pid;
            jobs[job_index].jid = next_jid++;
            jobs[job_index].cmd = malloc(strlen(cmd));
            strcpy(jobs[job_index].cmd, cmd);
            if (next_jid > MAX_JOBS)
                next_jid = 1;
            return 1;
        }
    }
    printf("jobs limit exceeds\n");
    return 0;
}

int del_job(pid_t pid)
{
    for (int job_index = 0; job_index < MAX_JOBS; job_index++)
    {
        if (jobs[job_index].pid == pid)
        {
            jobs[job_index].st = undef;
            jobs[job_index].pid = 0;
            jobs[job_index].jid = 0;
            free(jobs[job_index].cmd);
            jobs[job_index].cmd = NULL;
            next_jid = maxjid() + 1;
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
struct job * get_jpid(pid_t pid)
{
    for (int job_index = 0; job_index < MAX_JOBS; job_index++)
    {
        if (jobs[job_index].pid == pid)
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

void list_jobs(void)
{
    for (int job_index = 0; job_index < MAX_JOBS; job_index++)
    {
        if (jobs[job_index].st != undef)
        {
            printf("[%d] (%d) ", jobs[job_index].jid, jobs[job_index].pid);
            switch (jobs[job_index].st) {
            case bg: 
                printf("Running ");
                break;
            case fg: 
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
        }
    }
}