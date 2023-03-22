#include "top.h"

static void indentation(char* str, size_t length, char delim)
{
    size_t end = strlen(str);
    if (end >= length) return;
    for (size_t i = end; i < length; i++)
    {
        str[i] = delim;
    }
    str[length] = '\0';
}

int pcmpcpu(const void* p1, const void* p2)
{
    struct proc * arg1 = (struct proc *)p1;
    struct proc * arg2 = (struct proc *)p2;
    if (arg1->ticks < arg2->ticks)
        return 1;
    else if (arg1->ticks == arg2->ticks)
        return 0;
    else 
        return -1;
}

int pcmpmem(const void* p1, const void* p2)
{
    struct proc * arg1 = (struct proc *)p1;
    struct proc * arg2 = (struct proc *)p2;
    if (arg1->memory < arg2->memory)
        return 1;
    else if (arg1->memory == arg2->memory)
        return 0;
    else 
        return -1;
}

static void process_info(char* pid, struct proc * p)
{
    FILE* fp;
    char buf[NAME_MAX + 1] = "";
    sprintf(buf, "/proc/%s/psinfo", pid); // prepare psinfo file name
    fp = fopen(buf, "r");
    p->name[0] = '\0';
    p->username[0] = '\0';
    fscanf(fp, "%*d %*c %*d %255s %c %*d %d %lu %*u %*lu %*lu %lu %*u %*u %*c %*d %*u %lu %*u %d",
           p->name, &(p->state), &(p->priority), &(p->ticks), &(p->memory), &(p->uid), &(p->nice));
    fclose(fp);
    strcpy(p->username, getpwuid(p->uid)->pw_name);
}

void top()
{
    FILE* meminfo = fopen("/proc/meminfo", "r");
    FILE* kinfo = fopen("/proc/kinfo", "r");
    DIR * proc = opendir("/proc");
    struct dirent * dir;
    static struct proc process[PROC_MAX];
    unsigned long cpu_ticks = 0;
    int i = 0;
    printf("TOP START\n");

    /* Read memeory info */
    unsigned long memory;
    unsigned long freemem;
    unsigned long cachedmem;
    unsigned long buf[5]; // read meminfo buffer
    fscanf(meminfo, "%lu %lu %lu %lu %lu", &(buf[0]), &(buf[1]), &(buf[2]), &(buf[3]), &(buf[4]));
    memory = buf[0] * buf[1] / 1024;
    freemem = buf[0] * buf[2] / 1024;
    cachedmem = buf[0] * buf[4] / 1024;
    printf("main memory: %luK total, %luK free, %luK cached\n", memory, freemem, cachedmem);

    /* Read kernal info */
    unsigned proc_count;
    unsigned jobs_count;
    fscanf(kinfo, "%u %u", &proc_count, &jobs_count);
    if (proc_count >= PROC_MAX)
    {
        printf("too many processes!\n");
        return;
    }
    printf("%u processes, %u jobs\n", proc_count, jobs_count);


    /* Read process info */
    for (dir = readdir(proc), i = 0; dir != NULL && i < proc_count; dir = readdir(proc))
    {
        if (dir->d_type == DT_DIR && strcmp(dir->d_name, ".") && strcmp(dir->d_name, ".."))
        {
            process[i].pid = strtol(dir->d_name, NULL, 10);
            process_info(dir->d_name, &process[i]);
            i++;
        }
    }
    /* Caculate cpu ticks in all */
    for (int j = 0; j < i; j++)
    {
        cpu_ticks += process[j].ticks;
    }

    /* Sort the results */
    qsort(process, i, sizeof(proc), pcmpcpu);

    /* Print head */
    printf("PID USERNAME PRI NICE    SIZE STATE   TIME     CPU COMMAND\n");
    /* Print proc info */
    for (int j = 0; j < PAGE_MAX; j++)
    {
        printf("%3d %-8s %1d %4d %6luK %7c %3lu      %2.2lf%% %s\n", process[j].pid, process[j].username, process[j].priority, process[j].nice, process[j].memory / 1000, process[j].state, process[j].ticks/60, process[j].ticks / (double)cpu_ticks, process[j].name);
    }
}

int main()
{
    top();
    return 0;
}