#include <types.h>
#include <kern/errno.h>
#include <kern/unistd.h> //read/write const
#include <lib.h>
#include <machine/trapframe.h>
#include <kern/callno.h>
#include <syscall.h>

#include <vnode.h>
#include <uio.h>
#include <addrspace.h>
#include <thread.h>
#include <curthread.h>
#include <process.h>
#include <curproc.h>

/*
 * printf() and other user level write I/O system call.
 *
 * Note: 
 */

int
sys_read(int fd, userptr_t buf, size_t buflen, int * retval)
{
	struct iovec iov;
	struct uio u;
	int result;

	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		// kprintf("SYSCALL: sys_read.\n");
	}
	#endif

	if(fd != STDIN_FILENO && fd != STDERR_FILENO){
		return EBADF;
	}

	if(fd == STDERR_FILENO){
		return EBADF;
	}

	if(buf == NULL){
		return EFAULT;
	}

	// Use VOP_READ()
	u.uio_iovec = iov;

	mk_uuio(&u, buf, buflen, 0, UIO_READ);

	result = VOP_READ(curproc -> console, &u); 
	if(result){
		return result;
	}

	*retval = buflen - u.uio_resid;
  	assert(*retval >= 0);

	return 0;
}

