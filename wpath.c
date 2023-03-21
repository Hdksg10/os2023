#include "wpath.h"

void cd(char* argv[])
{
    if (Chdir(argv[1]) < 0)
        printf("cd: \"%s\" No such file or directory\n", argv[1]);
}

void pwd()
{
    char buffer[MAX_PATH_LENGTH];

    Getcwd(buffer, MAX_PATH_LENGTH);
    printf("%s\n", buffer);
}