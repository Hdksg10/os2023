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
    sprintf(buf, "/proc/%s/psinfo", pid);
    fp = fopen(buf, "r");
    p->name[0] = '\0';
    p->username[0] = '\0';
    fscanf(fp, "%d %c %d"); // ignore version, type, endpt;
    fscanf(fp, "%255s %c", p->name, &p->state);
    fscanf(fp, "%d"); // ignore p_blocked
    fscanf(fp, "%d %lu", &p->priority, &p->ticks);
    fscanf(fp, "%*u %lu %lu"); // ignore cycles
    fscanf(fp, "%lu", &p->memory);
    fscanf(fp, "%*u %*u %*c %*d %*u"); 
    fscanf(fp, "%u %*u", &p->uid);//ignore until p_nice
    fscanf(fp, "%d", &p->nice);
    fclose(fp);
    strcpy(p->username, getpwuid(p->uid)->pw_name);
}

void top()
{
    DIR * proc = opendir("/proc");
    struct dirent * dir;
    struct proc process[PAGE_MAX];
    char buf[256] = "/proc/";
    unsigned long cpu_ticks = 0;
    int i = 0;
    printf("TOP START\n");
    for (dir = readdir(proc), i = 0; dir != NULL && i < PAGE_MAX; dir = readdir(proc), i++);
    {
        if (dir->d_type == DT_DIR && strcmp(dir->d_name, ".") && strcmp(dir->d_name, ".."))
        {
            process_info(dir->d_name, &process[i]);
            process[i].pid = atoi(dir->d_name);
            i++;
            //printf("%d %16s %d %luK %c %d %lf%% %s", atoi(dir->d_name), p.username, p.priority, p.memory / 1000, p.state, p.ticks/60, p.ticks);
        }
    }
    for (int j = 0; j < i; j++)
    {
        cpu_ticks += process[j].ticks;
    }
    for (int j = 0; j < i; j++)
    {
        printf("%d %16s %d %luK %c %d %lf%% %s\n", process[j].pid, process[j].username, process[j].priority, process[j].memory / 1000, process[j].state, process[j].ticks/60, process[j].ticks / (double) cpu_ticks, process[j].name);
    }

}

int main()
{
    top();
    return 0;
}