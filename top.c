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

static void process_info(char* pid)
{
    DIR * process;
    struct dirent * dir;

    char buf[NAME_MAX + 1] = "/proc/";
    strcpy(buf+6, pid);
    process = opendir(buf);
    while ((dir = readdir(process)))
    {
        
    }
}

void top()
{
    DIR * proc = opendir("/proc");
    struct dirent * dir;
    struct stat st;
    char buf[256] = "/proc/";
    while ((dir = readdir(proc)))
    {
        if (dir->d_type == DT_DIR && strcmp(dir->d_name, ".") && strcmp(dir->d_name, ".."))
        {
            strcpy(buf + 6, dir->d_name);
            printf("%s\n", buf);
        }
    }
}

int main()
{
    top();
    return 0;
}