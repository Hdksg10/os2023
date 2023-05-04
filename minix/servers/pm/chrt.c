#include "pm.h"

/*===========================================================================*
 *				do_chrt()			     * 
 *===========================================================================*/
int do_chrt()
{
    long deadline = m_in.m_m4.m4l1;
    int result = sys_chrt(who_e, deadline);
    return result;
}