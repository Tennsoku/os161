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
sys_write(int fd, userptr_t buf, size_t nbytes, int * retval)
{
	struct iovec iov;
	struct uio u;
	int result;

	assert(curproc != NULL);
	assert(curproc -> console != NULL);
	
	// #ifdef _DB_MODE_
	// if (_DB_MODE_) {	
	// 	kprintf("SYSCALL: sys_write.\n");
	// }
	// #endif

	if(fd != STDOUT_FILENO && fd != STDERR_FILENO){
		return EBADF;
	}

	// if(fd == STDERR_FILENO){
	// 	return EFAULT;
	// }

	if(buf == NULL){
		return EFAULT;
	}

	// Use VOP_WRITE()
	u.uio_iovec = iov;

	mk_uuio(&u, buf, nbytes, 0, UIO_WRITE);

	result = VOP_WRITE(curproc -> console, &u);
	if(result){
		return result;
	}


	*retval = nbytes - u.uio_resid;
  	assert(*retval >= 0);

	return 0;
}
