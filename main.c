#include <stdio.h>

#include "shell.h"


int main() {
    struct command cmd;
    char cmdline[] = " run a  b  c   d    e > thm";
    parse(cmdline);
    check_buffer();
    return 0;
}
