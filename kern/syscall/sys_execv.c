#include <types.h>
#include <kern/errno.h>
#include <kern/limits.h>
#include <lib.h>
#include <thread.h>
#include <curthread.h>
#include <process.h>
#include <curproc.h>
#include <machine/trapframe.h>
#include <kern/callno.h>
#include <addrspace.h>
#include <syscall.h>
#include <array.h>
#include <test.h>
#include <vm.h>

#define EXECV_ARG_MAX ARG_MAX
#define EXECV_PROGNAME_MAX PATH_MAX

int 
sys_execv(const char *progname, char **args, int *retval) {
	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		kprintf("SYSCALL: _execv.\n");
	}
	#endif

	int result;
	unsigned long nargs = 0;

	if(ptrcheck((vaddr_t)progname)||ptrcheck((vaddr_t)args)){
		*retval = -1;
		#ifdef _DB_MODE_
		if (_DB_MODE_) {	
			kprintf("Returns on ptrcheck 2:0x%04x, 0x%04x.\n",(vaddr_t)progname ,(vaddr_t)args);
		}
		#endif
		return EFAULT;
	}

	size_t progname_len = strlen(progname)+1; // 0-terminated
	if (progname_len>EXECV_PROGNAME_MAX){
		*retval = -1;
		return E2BIG;
	}

	if (progname_len==1){
		*retval = -1;
		return EISDIR;
	}
	size_t totalargs_len = 0;

	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		kprintf("SYS_execv: got progname_len.\n");
	}
	#endif

	while (args[nargs]!=NULL){
		if(ptrcheck((vaddr_t)args[nargs])){
			#ifdef _DB_MODE_
			if (_DB_MODE_) {	
				kprintf("Returns on ptrcheck 2:0x%04x\n",(vaddr_t)args[nargs]);
			}
			#endif
			
			*retval = -1;
			return EFAULT;
		}
		totalargs_len += strlen(args[nargs])+1; // 0-terminated
		#ifdef _DB_MODE_
		if (_DB_MODE_) {	
			kprintf("args[%d]:%s\n",nargs,args[nargs]);
		}
		#endif
		nargs++;
	}

	if(nargs > EXECV_ARG_MAX || totalargs_len > EXECV_PROGNAME_MAX){
		*retval = -1;
		return E2BIG;
	}

	char kprogname[progname_len];
	char kargsval[totalargs_len];
	char *kargs[nargs];
	size_t got;
	size_t strcpy_offset = 0;

	result = copyinstr((userptr_t)progname, kprogname, progname_len, NULL);
	if(result){
		*retval = -1;
		return result;
	}

	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		kprintf("SYS_execv: kprogname copied.\n");
	}
	#endif

	unsigned long i;
	for(i=0; i<nargs;i++){
		char * dest;
		dest = kargsval+strcpy_offset;
		result = copyinstr((userptr_t)args[i], dest, strlen(args[i])+1, &got); 
		if (result){
			*retval = -1;
			return result;
		}
		kargs[i] = dest;
		#ifdef _DB_MODE_
		if (_DB_MODE_) {	
			kprintf("kargsval[%lu]: %s\n", i, kargs[i]);
		}
		#endif
		strcpy_offset += got;
	}

	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		kprintf("SYS_execv: kargsval[] copied.\n");
		kprintf("SYS_execv: Entering runprogram().\n\n");
	}
	#endif

	/* Create a new address space. */

	struct addrspace *as = curproc->p_vmspace;
	as_destroy(as);
	curproc->p_vmspace = as_create();
	if (curproc->p_vmspace == NULL) {
		*retval = -1;
		return ENOMEM;
	}

	result = runprogram(kprogname, nargs, kargs);

	// Runprogram returns on error;
	*retval = -1;
	return result;

}
