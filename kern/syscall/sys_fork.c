#include <types.h>
#include <lib.h>
#include <machine/trapframe.h>
#include <kern/callno.h>
#include <kern/errno.h>
#include <array.h>
#include <vnode.h>
#include <uio.h>
#include <addrspace.h>
#include <thread.h>
#include <curthread.h>
#include <process.h>
#include <curproc.h>
#include <syscall.h>

int sys_fork(struct trapframe *tf, pid_t * retval){
	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		kprintf("SYSCALL: _fork.\n");
	}
	#endif

	int result;

	struct process *child;
	child = process_fork(curproc->p_name);
	if(child == NULL){
		return EAGAIN;
	}

	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		kprintf("Child process id=%d created.\n", child->p_id);
	}
	#endif

	/*
	 * Since:
	 * " The two processes do not share memory or open file tables; 
	 * this state is copied into the new process, and subsequent 
	 * modification in one process does not affect the other. "
	 * I think I should copy a trapframe for child.
	 */

	struct trapframe *child_tf = kmalloc(sizeof(struct trapframe));
	if (child_tf == NULL){
		process_destroy(child);
		return ENOMEM;
	}

	// memcpy will not fail
	memcpy(child_tf,tf, sizeof(struct trapframe));

	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		kprintf("Child trapframe copied.\n");
	}
	#endif

	result = thread_fork(curthread->t_name, child, child_tf, 0, &md_forkentry, NULL);
	if(result){
		#ifdef _DB_MODE_
		if (_DB_MODE_) {	
			kprintf("thread_fork failed.\n");
		}
		#endif
		process_destroy(child);
		kfree(child_tf);
		return ENOMEM;
	}

	*retval = child->p_id;

	return 0;


}

