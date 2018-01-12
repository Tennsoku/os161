#include <types.h>
#include <lib.h>
#include <thread.h>
#include <curthread.h>
#include <process.h>
#include <curproc.h>


int sys_getpid(pid_t * retval){
	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		// kprintf("SYSCALL: _getpid.\n");
	}
	#endif
	
	*retval = curproc->p_id;
	return 0;
}
