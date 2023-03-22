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
    printf("%s:%d\n", p->name, p->pid);
    fclose(fp);
    strcpy(p->username, getpwuid(p->uid)->pw_name);
}

void top()
{
    DIR * proc = opendir("/proc");
    struct dirent * dir;
    struct proc process[PAGE_MAX];
    unsigned long cpu_ticks = 0;
    int i = 0;
    printf("TOP START\n");
    /* Read process info */
    for (dir = readdir(proc), i = 0; dir != NULL && i < PAGE_MAX; dir = readdir(proc))
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
    /* Print head */
    printf("PID USERNAME PRI NICE    SIZE STATE   TIME     CPU COMMAND\n");
    for (int j = 0; j < i; j++)
    {
        printf("%3d %-8s %1d %4d %6luK %7c %4.0lf   %2.2lf%% %s\n", process[j].pid, process[j].username, process[j].priority, process[j].nice, process[j].memory / 1000, process[j].state, process[j].ticks/(double)60, process[j].ticks /cpu_ticks, process[j].name);
    }

}

int main()
{
    top();
    return 0;
}