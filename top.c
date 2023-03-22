#include "top.h"

/* terminal control variable*/
static struct termios save_termios;
static int ttysavefd = -1;
static enum{RESET, CBREAK} ttystate = RESET;



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

/* put terminal into cbreak mode */
static int tty_cbreak(int fd)
{
    int err;
    struct termios buf;

    /* Check function is called corectly*/
    if (ttystate != RESET){
        errno = EINVAL;
        return -1;
    }
    /* Open terminal arrtibutes*/
    if (tcgetattr(fd, &buf) < 0)
        return -1;
    save_termios = buf; //copy of current attr;

    /* Echo off, canonical mode off */
    buf.c_lflag &= ~(ECHO| ICANON);

    /* Terminal return 1 byte at time, no timer*/
    buf.c_cc[VMIN] = 1;
    buf.c_cc[VTIME] = 0;
    if (tcsetattr(fd, TCSAFLUSH, &buf) < 0)
        return -1;
    
    /* Verify all the changes success */
    if (tcgetattr(fd, &buf) < 0){
        err = errno;
        tcsetattr(fd, TCSAFLUSH, &save_termios);
        errno = err;
        return -1;
    }
    if ((buf.c_lflag & (ECHO| ICANON)) || buf.c_cc[VMIN] != 1 || buf.c_cc[VTIME] != 0){
        // partial success
        tcsetattr(fd, TCSAFLUSH, &save_termios);
        errno = EINVAL;
        return -1;
    }

    ttystate = CBREAK;
    ttysavefd = fd;
    return 0;
}

static int tty_reset(int fd)
{
    if (ttystate == RESET) // doesnot need
        return 0;
    if (tcsetattr(fd, TCSAFLUSH, &save_termios) < 0)
        return -1;
    ttystate = RESET;
    return 0;
}

static void tty_atexit(void) // restore when exit
{
    if (ttysavefd >= 0)
        tty_reset(ttysavefd);
}

static int pcmpcpu(const void* p1, const void* p2)
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

static int pcmpmem(const void* p1, const void* p2)
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

void run_top(int sortbycpu)
{
    /* file and dir pointer */
    FILE* meminfo;
    FILE* kinfo;
    DIR* proc;
    meminfo = fopen("/proc/meminfo", "r");
    kinfo = fopen("/proc/kinfo", "r");
    proc = opendir("/proc");
    static struct dirent * dir;
    static struct proc process[PROC_MAX];
    static char displayorder[2][7] = {"memory", "CPU"};
    memset(process, 0, PROC_MAX * sizeof(struct proc)); //init
    unsigned long cpu_ticks = 0;
    int i = 0;

    /* Read memeory info */
    unsigned long memory;
    unsigned long freemem;
    unsigned long cachedmem;
    unsigned long buf[5]; // read meminfo buffer
    fscanf(meminfo, "%lu %lu %lu %lu %lu", &(buf[0]), &(buf[1]), &(buf[2]), &(buf[3]), &(buf[4]));
    memory = buf[0] * buf[1] / 1024;
    freemem = buf[0] * buf[2] / 1024;
    cachedmem = buf[0] * buf[4] / 1024;
    printf("TOP START\n");
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
    printf("%u processes, %u jobs; sort order ('o' to cycle): %s\n", proc_count, jobs_count, displayorder[sortbycpu]);
    printf("press 'q' to quit\n");

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
    if (sortbycpu)
        qsort(process, i, sizeof(struct proc), pcmpcpu);
    else
        qsort(process, i, sizeof(struct proc), pcmpmem);

    /* Print head */
    printf("PID USERNAME PRI NICE    SIZE STATE   TIME     CPU COMMAND\n");
    /* Print proc info */
    for (int j = 0; j < PAGE_MAX; j++)
    {
        printf("%3d %-8s %1d %4d    %6luK %7c %3lu      %2.2lf%% %s\n", process[j].pid, process[j].username, process[j].priority, process[j].nice, process[j].memory / 1000, process[j].state, process[j].ticks/60, process[j].ticks / (double)cpu_ticks, process[j].name);
    }
}

void top()
{
    char ctrlchar;
    int charnum;
    int order = 1;
    atexit(tty_atexit);
    if (tty_cbreak(STDIN_FILENO) < 0)
    {
        printf("cannot set terminal state, run top failed\n");
        return;
    }
    run_top(order);
    while ((charnum = read(STDIN_FILENO, &ctrlchar, 1)) == 1)
    {
        switch (ctrlchar)
        {
        case 'o':
            order = !order;
            run_top(order);
            break;
        case 'q':
            tty_reset(STDIN_FILENO);
            return;
            break;
        default:
            run_top(order);
            break;
        }
    }
    tty_reset(STDIN_FILENO); // won't reach
    return;
}

int main()
{
    top();
    return 0;
}