#include "syslib.h"

/*===========================================================================*
 *                                sys_chrt			     	     *
 *===========================================================================*/
int sys_chrt(proc_ep, deadline)
endpoint_t proc_ep;     /* which process calls chrt() */
long deadline;
{
    message m;
    int r;
    m.m_m4.m4l1 = deadline;
    m.m_m4.m4l2 = proc_ep;
    r = _kernel_call(SYS_CHRT, &m);
    return r;
}