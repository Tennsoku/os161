#include <types.h>
#include <kern/errno.h>
#include <kern/limits.h>
#include <lib.h>
#include <machine/trapframe.h>
#include <kern/callno.h>
#include <syscall.h>
#include <array.h>
#include <thread.h>
#include <curthread.h>
#include <process.h>
#include <curproc.h>
#include <vm.h>

int 
sys_waitpid(pid_t pid, int *status , int options, pid_t *retval) 
{

	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		kprintf("SYSCALL: _waitpid.\n");
		kprintf("_waitpid check point: pid=%d,status=%u,options=%d.\n",pid,(vaddr_t)status,options);
	}
	#endif

	if (pid > PID_MAX || pid == 0 || pid<0){
		#ifdef _DB_MODE_
		if (_DB_MODE_) {	
			kprintf("_waitpid returns on pid fault.\n");
		}
		#endif
		return EINVAL;
	}

	if((unsigned)status % sizeof(int) != 0){
		return EFAULT;
	}

	if (curproc->usermode == 0 && (vaddr_t)status>=USERTOP){
		(void)retval;
	}
	else if (ptrcheck((vaddr_t)status)){
		#ifdef _DB_MODE_
		if (_DB_MODE_) {
			kprintf("_waitpid returns on status fault: %lu, %lu\n",(vaddr_t)status,(int)0x80000000);
		}
		#endif
		return EFAULT;
	}

	int result;

	if (options != 0){
		#ifdef _DB_MODE_
		if (_DB_MODE_) {	
			kprintf("_waitpid returns on options fault.\n");
		}
		#endif
		return EINVAL;
	}

	struct process * proc = proc_list_find_by_pid(pid);
	if (proc == NULL){
		#ifdef _DB_MODE_
		if (_DB_MODE_) {	
			kprintf("_waitpid returns on EPROC fault:%d.\n",pid);
		}
		#endif
		return EPROC;
	}

	if (proc == curproc || pid == curproc->p_parent){
		#ifdef _DB_MODE_
		if (_DB_MODE_) {	
			kprintf("_waitpid returns on ESELF fault.\n");
		}
		#endif
		return EINVAL;
	}


	result = process_addinterest(proc, curproc);
	if(result){
		return result;
	}

	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		// kprintf("_waitpid check point 2.\n");
	}
	#endif

	while(proc->exited == 0){
		#ifdef _DB_MODE_
		if (_DB_MODE_ && curproc->p_id !=0) {	
			kprintf("_waitpid check point 3. Curproc %d waits for not exited proc %d. Thread yielded.\n", curproc->p_id, proc->p_id);
		}
		#endif
		thread_yield();
	}

	*status = proc->exitcode;


	*retval = proc->p_id;

	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		kprintf("_waitpid check point 4. Proc %d passed loop. Exitcode: %d.\n", proc->p_id, proc->exitcode);
	}
	#endif

	result = process_rminterest(proc, curproc);
	if(result){
		return result;
	}

	check_interest(proc);

	(void)options;

	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		kprintf("_waitpid check point 5. Proc %d is exited. Exitcode: %d.\n", *retval, *status);
	}
	#endif

	return 0;
}
