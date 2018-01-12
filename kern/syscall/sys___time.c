#include <types.h>
#include <kern/errno.h>
#include <clock.h>
#include <lib.h>
#include <machine/trapframe.h>
#include <syscall.h>


int sys___time(time_t *seconds,unsigned long *nanoseconds, int *retval){
	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		kprintf("SYSCALL: __time. \n");
	}
	#endif
	
	// if (ptrcheck(seconds)|ptrcheck(nanoseconds)){
	// 	*retval = -1;
	// 	return EFAULT;
	// }

	time_t kseconds;
	u_int32_t knanoseconds;
	int result;

	gettime(&kseconds,&knanoseconds);

	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		kprintf("kseconds: %d, knanoseconds: %d.\n",kseconds,knanoseconds);
	}
	#endif

	if(seconds != NULL){
		result = copyout(&kseconds, (userptr_t)seconds, sizeof(time_t));
		if(result){
			return result;
		}
	}

	if(nanoseconds != NULL){
		result = copyout(&knanoseconds, (userptr_t)nanoseconds, sizeof(u_int32_t));
		if(result){
			return result;
		}
	}

	*retval = kseconds;
	return 0;
}
