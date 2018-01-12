#include <types.h>
#include <kern/errno.h>
#include <kern/limits.h>
#include <lib.h>
#include <machine/trapframe.h>
#include <kern/callno.h>
#include <syscall.h>
#include <addrspace.h>
#include <array.h>
#include <thread.h>
#include <curthread.h>
#include <process.h>
#include <curproc.h>

void sys__exit(int exitcode){

	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		kprintf("SYSCALL: __exit. Curproc: %d, Exitcode: %d\n",curproc->p_id, exitcode);
		kprintf("curthread t_proc: %d.\n",curthread->t_proc->p_id);
	}
	#endif

	// if has parent, remove
	if(curproc->p_parent<PID_MAX){
		process_rmchild(curproc->p_parent, curproc);
	}


	// while(array_getnum(curproc->p_children)!= 0){
	// 	//wait for children's exits
	// 	#ifdef _DB_MODE_
	// 	if (_DB_MODE_) {
	// 		kprintf("SYS__exit check point 1. Curproc %d has running child exists, cannot exit.\n", curproc->p_id);
	// 		int i;
	// 		for(i=0;i<array_getnum(curproc->p_children);i++){
	// 			struct process *p = array_getguy(curproc->p_children, i);
	// 			kprintf("Curproc %d has child: %d.\n",curproc->p_id, p->p_id);
	// 		}
	// 	}
	// 	#endif
	// 	thread_yield();
	// }

	// set exitcode and exited flag
	curproc->exitcode = exitcode;
	curproc->exited = 1;

	// destroy vmspace
	struct addrspace *as = curproc->p_vmspace;
	curproc->p_vmspace = NULL;
	as_destroy(as);

	// if this process is system process, destroy itself
	if(curproc->p_id == 0){
		process_destroy(curproc);
	}

	// infinite loop and wait other process destroy this process.
	while(1){
		// if process is destroyed, curthread exit.
		if (curthread-> t_proc == NULL){
			thread_exit();
			panic("thread_exit() returns.\n");
		}

		#ifdef _DB_MODE_
		if (_DB_MODE_) {	
			kprintf("SYS__exit check point 2. Curproc %d waiting to be destroyed. Exited = %d.\n", curproc->p_id, curproc->exited);
		}
		#endif
		thread_yield();
	}
}
