/*
 * Sample/test code for running a user program.  You can use this for
 * reference when implementing the execv() system call. Remember though
 * that execv() needs to do more than this function does.
 */

#include <types.h>
#include <kern/unistd.h>
#include <kern/errno.h>
#include <lib.h>
#include <addrspace.h>
#include <thread.h>
#include <curthread.h>
#include <vm.h>
#include <vfs.h>
#include <test.h>
#include <process.h>
#include <curproc.h>
#include <syscall.h>

#define DIVROUNDUP(a,b) (((a)+(b)-1)/(b))
#define ROUNDUP(a,b)    (DIVROUNDUP(a,b)*b)


/*
 * Load program "progname" and start running it in usermode.
 * Does not return except on error.
 *
 * Calls vfs_open on progname and thus may destroy it.
 * Note that args[0] is program name
 */
int
runprogram(char *progname, unsigned int nargs, char **args)
{
	struct vnode *v;
	vaddr_t entrypoint, stackptr;
	int result;

	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		kprintf("At runprogram(). nargs now: %u.\n",nargs);
		kprintf("Current process: %s\n", curproc->p_name);
		kprintf("Current thread: %s\n", curthread->t_name);
		kprintf("Args addresss: %u\n",(vaddr_t)args);
		unsigned count;
		for(count=0;count<nargs;count++)
			kprintf("Printing args[%d]:%s\n",count,args[count]);
	}
	#endif

	/* Open the file. */
	result = vfs_open(progname, O_RDONLY, &v);
	if (result) {
		return result;
	}

	/* We should be a new thread. */
	// assert(curthread->t_vmspace == NULL);

	/* new address space was created in process. */
	// /* Create a new address space. */
	// curthread->t_vmspace = as_create();
	// if (curthread->t_vmspace==NULL) {
	// 	vfs_close(v);
	// 	return ENOMEM;
	// }

	/* Dealing with the passed arguments. */
	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		kprintf("vfs_open finished successfully.\n");
	}
	#endif

	/* This should be a new thread */
	/* Set current thread address space. */
	if (curthread -> t_vmspace != curproc -> p_vmspace){
		struct addrspace *as = curthread -> t_vmspace;
		as_destroy(as);
	}
	curthread -> t_vmspace = curproc->p_vmspace;

	/* Activate it. */
	as_activate(curthread->t_vmspace);


	/* Load the executable. */
	result = load_elf(v, &entrypoint);
	if (result) {
		/* thread_exit destroys curthread->t_vmspace */
		vfs_close(v);
		return result;
	}

	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		kprintf("load_elf finished successfully.\n");
	}
	#endif

	/* Done with the file now. */
	vfs_close(v);

	/* Dealing with the passed arguments. */
	/* Define the user stack in the address space */
	result = as_define_stack(curthread->t_vmspace, &stackptr);
	if (result) {
		return result;
	}

	if (nargs == 1){
		#ifdef _DB_MODE_
		if (_DB_MODE_) {	
			kprintf("About to enter md_usermode().\n\n");
		}
		#endif
		/* Warp to user mode. */
		curproc->usermode = 1;
		md_usermode(nargs /*argc*/, NULL /*userspace addr of argv*/,
			    stackptr, entrypoint);
	}
	else{
		// uargv = (userptr_t)copy_args(nargs, args, stackptr);
		// if (uargv == NULL){
		// 	return ECPY;
		// }

		// Make space for userspace arguments.
		// Why ?????
		while ((stackptr % 8) != 0) {
			stackptr--;
		}
		stackptr -= stackptr % 8;

		vaddr_t arg_ptr[nargs + 1];
		int i;
	    for (i = nargs - 1; i >= 0; i--) {
            stackptr -= strlen(args[i]) + 1;
            result = copyoutstr(args[i], (userptr_t) stackptr, strlen(args[i]) + 1, NULL);
            if (result) {
                    return result;
            }
            arg_ptr[i] = stackptr;
	    }

	    while ((stackptr % 4) != 0) {
            stackptr--;
	    }

	    arg_ptr[nargs] = 0;

	    for (i = nargs; i >= 0; i--) {
            stackptr -= ROUNDUP(sizeof(vaddr_t), 4);
            result = copyout(&arg_ptr[i], (userptr_t) stackptr, sizeof(vaddr_t));
            if (result) {
                return result;
            }
	    }
	    //????????

		#ifdef _DB_MODE_
		if (_DB_MODE_) {
			kprintf("Arg copied.\n");
			kprintf("About to md_usermode().\n\n");
		}
		#endif
		/* Warp to user mode. */
		curproc->usermode = 1;
		md_usermode(nargs /*argc*/, (userptr_t)stackptr /*userspace addr of argv*/,
			    stackptr, entrypoint);
	} 

	
	/* md_usermode does not return */
	panic("md_usermode returned\n");
	return EINVAL;
}









// char** copy_args(unsigned int nargs, char **args, vaddr_t stackptr) {
// 	/* 
// 	 * Notice:
// 	 * Size of Char**: 4.
// 	 * Size of Char*: 4.
// 	 * Size of userptr_t: 4.
// 	 * They are the same.
// 	 */
// 	assert(args != NULL);

// 	#ifdef _DB_MODE_
// 	if (_DB_MODE_) {	
// 		kprintf("At copy_args(). nargs now: %d.\n",nargs);
// 		kprintf("curthread t_vmspace vbase = %d.\n", curthread->t_vmspace->as_vbase1);
// 		kprintf("curproc p_vmspace vbase = %d.\n", curproc->p_vmspace->as_vbase1);
// 	}
// 	#endif

// 	int offset = 0; // for calculating string length and computing size
// 	unsigned int len, i;
// 	int result;
// 	userptr_t uargv; 

// 	for (i=0;i<nargs;i++){
// 		len = strlen(args[i]) + 1;
// 		offset += len;
// 	}
// 	/* At this point, offset equals length of entire args */
// 	/* Now calculate new mem space needed for copied user stack */
// 	/* Notice that char ** also takes space. */
// 	size_t userptr_size = sizeof(userptr_t);
// 	size_t ptrspace = (nargs + 1) * sizeof(char **);
// 	ptrspace += (ptrspace % userptr_size) == 0 ? 0 : userptr_size;
// 	size_t valspace = offset * sizeof(char);
// 	valspace += (valspace % userptr_size) == 0 ? 0 : userptr_size;

// 	size_t total = ptrspace + valspace;

// 	/* move stackptr */
// 	stackptr = stackptr - total;
// 	uargv = (userptr_t)stackptr;
// 	userptr_t uargv_val = (userptr_t)(uargv + ptrspace); // start point of actual value started

// 	size_t got;
// 	size_t strcpy_offset = 0;
// 	char *argsval[nargs+1]; // for safety. // there's a \0 at the end

// 	// start copying str passed for args in kernel mode to userspace.
// 	unsigned int j;
// 	for (j=0;j<nargs;j++){
// 		userptr_t dest = uargv_val+strcpy_offset;
// 		result = copyoutstr(args[j], dest, strlen(args[j])+1, &got); 
// 		if (result){
// 			return NULL;
// 		}
// 		argsval[j] = (char *) dest;
// 		kprintf("argsval[%d]: %s\n", j, argsval[j]);
// 		strcpy_offset += got;
// 	}

// 	#ifdef _DB_MODE_
// 	if (_DB_MODE_) {	
// 		kprintf("Argsval[] copied.\n");
// 	}
// 	#endif

// 	argsval[nargs] = NULL;
// 	result = copyout(argsval, uargv, (nargs+1)*sizeof(char*));
// 	if(result){
// 		return NULL;
// 	}
// 	// uargv = (userptr_t)argsval;

// 	#ifdef _DB_MODE_
// 	if (_DB_MODE_) {
// 		kprintf("Return from copy_args().\n");
// 	}
// 	#endif

// 	return (char**)uargv;
// }


// char** copy_args(unsigned int nargs, char **args, vaddr_t stackptr) {
// 	/* 
// 	 * Notice:
// 	 * Size of Char**: 4.
// 	 * Size of Char*: 4.
// 	 * Size of userptr_t: 4.
// 	 * They are the same.
// 	 */
// 	assert(args != NULL);

// 	#ifdef _DB_MODE_
// 	if (_DB_MODE_) {	
// 		kprintf("At copy_args(). nargs now: %d.\n",nargs);
// 	}
// 	#endif

// 	size_t totalargs_len = 0;
// 	unsigned int i;
// 	int result;

// 	for(i=0;i<nargs;i++){
// 		totalargs_len += strlen(args[i])+1; // 0-terminated
// 	}

// 	#ifdef _DB_MODE_
// 	if (_DB_MODE_) {	
// 		kprintf("copy_args check point 1. totalargs_len = %d.\n", totalargs_len);
// 	}
// 	#endif

// 	/* At this point, offset equals length of entire args */
// 	/* Now calculate new mem space needed for copied user stack */
// 	/* Notice that char ** also takes space. */

// 	char *uargsval;
// 	char **uargs;

// 	uargs = (char**)((userptr_t)stackptr);
// 	size_t ptrspace = (nargs + 1) * sizeof(char *);
// 	kprintf("ptrspace:%d\n",ptrspace);
// 	uargsval = (char *)((userptr_t)uargs + ptrspace);

// 	size_t got;
// 	size_t strcpy_offset = 0;

// 	#ifdef _DB_MODE_
// 	if (_DB_MODE_) {	
// 		kprintf("copy_args check point 2. stackptr=%lu. uargs=%lu. uargsval=%lu.\n", (userptr_t)stackptr, (userptr_t)uargs,(userptr_t)uargsval);
// 	}
// 	#endif

// 	unsigned int j;
// 	for(j=0; j<nargs;j++){
// 		userptr_t dest;
// 		dest = uargsval+strcpy_offset;
// 		result = copyoutstr(args[j], (userptr_t)dest, strlen(args[i])+1, &got); 
// 		if (result){
// 			panic("Copyoutstr failed in runprogram().\n");
// 		}
// 		//uargs[i] = dest;
// 		kprintf("uargs[%u]:%s",j,((char *)dest)[j]);
// 		strcpy_offset += got;
// 	}

// 	uargs[nargs] = NULL;

// 	#ifdef _DB_MODE_
// 	if (_DB_MODE_) {	
// 		kprintf("copy_args: uargsval[] copied.\n");
// 		kprintf("Return from copy_args().\n");
// 	}
// 	#endif

// 	return (char**)uargs;
// }

