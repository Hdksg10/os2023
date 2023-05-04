#include "kernel/system.h"
#include "kernel/vm.h"

#include <minix/endpoint.h>
#include <minix/u64.h>

/*===========================================================================*
 *				do_chrt				     *
 *===========================================================================*/
int do_chrt(struct proc * caller, message * m_ptr)
{
/* Handle sys_chrt. set p_deadline attribute in process table.
 */
    long deadline;
    int proc_nr;
    struct proc * rc;

    if (!isokendpt(m_ptr->m_m4.m4l2, &proc_nr))
        /* get current process number */
        return EINVAL;
    
    rc = proc_addr(proc_nr);
    deadline = m_ptr->m_m4.m4l1;
    if (deadline) rc->p_priority = 6;
    else rc->p_priority = 7;
    rc->p_deadline = deadline;
    return 0;
}