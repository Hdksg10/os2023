#include "builtincmd.h"

void builtin_exit()
{
    free_list();
    exit(0);
}
