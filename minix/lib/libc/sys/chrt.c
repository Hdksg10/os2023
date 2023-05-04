#include <sys/cdefs.h>
#include "namespace.h"
#include <unistd.h>
#include "lib.h"

#include <string.h>
#include <unistd.h>


int chrt(long deadline)
{
    message m;

    memset(&m, 0, sizeof(m));
    m.m_m4.m4l1 = deadline; // save deadline in message
    alarm(deadline); // kill process when deadline reach
    return (_syscall(PM_PROC_NR, PM_CHRT, &m));
}