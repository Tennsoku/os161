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

	/* Open the file. */
	result = vfs_open(progname, O_RDONLY, &v);
	if (result) {
		return result;
	}

	/* We should be a new thread. */
	assert(curthread->t_vmspace == NULL);

	/* new address space was created in process. */
	// /* Create a new address space. */
	// curthread->t_vmspace = as_create();
	// if (curthread->t_vmspace==NULL) {
	// 	vfs_close(v);
	// 	return ENOMEM;
	// }

	/* Load the executable. */
	result = load_elf(v, &entrypoint);
	if (result) {
		/* thread_exit destroys curthread->t_vmspace */
		vfs_close(v);
		return result;
	}

	/* Done with the file now. */
	vfs_close(v);

	/* Define the user stack in the address space */
	result = as_define_stack(curthread->t_vmspace, &stackptr);
	if (result) {
		/* thread_exit destroys curthread->t_vmspace */
		return result;
	}

	/* Dealing with the passed arguments. */
	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		kprintf("At runprogram(). nargs now: %d.\n",nargs);
		kprintf("Current process: %s\n", curproc->p_name);
		kprintf("Current thread: %s\n", curthread->t_name);
		unsigned count;
		for(count=0;count<nargs;count++)
			kprintf("Printing args[%d]:%s\n",count,args[count]);

	}
	#endif

	/* drop program name */
	nargs--;
	args++;

	// start point of userspace addr
	userptr_t uargv; 

	if (nargs == 0){
		#ifdef _DB_MODE_
		if (_DB_MODE_) {	
			kprintf("About to enter md_usermode().\n\n");
		}
		#endif
		/* Warp to user mode. */
		md_usermode(nargs /*argc*/, NULL /*userspace addr of argv*/,
			    stackptr, entrypoint);
	}
	else{
		uargv = (userptr_t)copy_args(nargs, args, stackptr);
		if (uargv == NULL){
			return ECPY;
		}

		#ifdef _DB_MODE_
		if (_DB_MODE_) {	
			kprintf("\nAbout to md_usermode().\n");
		}
		#endif
		/* Warp to user mode. */
		md_usermode(nargs /*argc*/, uargv /*userspace addr of argv*/,
			    stackptr, entrypoint);
	} 

	
	/* md_usermode does not return */
	panic("md_usermode returned\n");
	return EINVAL;
}

char** copy_args(unsigned int nargs, char **args, vaddr_t stackptr) {
	/* 
	 * Notice:
	 * Size of Char**: 4.
	 * Size of Char*: 4.
	 * Size of userprt_t: 4.
	 * They are the same.
	 */
	assert(args != NULL);

	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		kprintf("At copy_args().\n");
		kprintf("curthread t_vmspace vbase = %d.\n", curthread->t_vmspace->as_vbase1);
		kprintf("curproc p_vmspace vbase = %d.\n", curproc->p_vmspace->as_vbase1);
	}
	#endif

	int offset = 0; // for calculating string length and computing size
	unsigned int len, i;
	int result;
	userptr_t uargv; 

	for (i=0;i<nargs;i++){
		len = strlen(args[i]) + 1;
		offset += len;
	}
	/* At this point, offset equals length of entire args */
	/* Now calculate new mem space needed for copied user stack */
	/* Notice that char ** also takes space. */
	size_t userptr_size = sizeof(userptr_t);
	size_t prtspace = (nargs + 1) * sizeof(char **);
	prtspace += (prtspace % userptr_size) == 0 ? 0 : userptr_size;
	size_t valspace = offset * sizeof(char);
	valspace += (valspace % userptr_size) == 0 ? 0 : userptr_size;

	size_t total = prtspace + valspace;

	/* move stackptr */
	stackptr = stackptr - total;
	uargv = (userptr_t)stackptr;
	userptr_t uargv_val = (userptr_t)(uargv + prtspace); // start point of actual value started

	size_t got;
	size_t strcpy_offset = 0;
	char *argsval[nargs+1]; // for safety. // there's a \0 at the end

	// start copying str passed for args in kernel mode to userspace.
	unsigned int j;
	for (j=0;j<nargs;j++){
		userptr_t dest = uargv_val+strcpy_offset;
		result = copyoutstr(args[j], dest, strlen(args[j])+1, &got); //here!!
		if (result){
			return NULL;
		}
		argsval[j] = (char *) dest;
		kprintf("argsval[%d]: %s\n", j, argsval[j]);
		strcpy_offset += got;
	}

	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		kprintf("Argsval[] copied.\n");
	}
	#endif

	argsval[nargs] = NULL;
	result = copyout(argsval, uargv, (nargs+1)*sizeof(char**));
	if(result){
		return NULL;
	}

	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		kprintf("Return from copy_args().\n");
	}
	#endif

	return (char**)uargv;
}
