#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <machine/pcb.h>
#include <machine/spl.h>
#include <machine/trapframe.h>
#include <kern/callno.h>
#include <syscall.h>
#include <thread.h>
#include <curthread.h>
#include <process.h>
#include <curproc.h>
#include <addrspace.h>

/*
 * System call handler.
 *
 * A pointer to the trapframe created during exception entry (in
 * exception.S) is passed in.
 *
 * The calling conventions for syscalls are as follows: Like ordinary
 * function calls, the first 4 32-bit arguments are passed in the 4
 * argument registers a0-a3. In addition, the system call number is
 * passed in the v0 register.
 *
 * On successful return, the return value is passed back in the v0
 * register, like an ordinary function call, and the a3 register is
 * also set to 0 to indicate success.
 *
 * On an error return, the error code is passed back in the v0
 * register, and the a3 register is set to 1 to indicate failure.
 * (Userlevel code takes care of storing the error code in errno and
 * returning the value -1 from the actual userlevel syscall function.
 * See src/lib/libc/syscalls.S and related files.)
 *
 * Upon syscall return the program counter stored in the trapframe
 * must be incremented by one instruction; otherwise the exception
 * return code will restart the "syscall" instruction and the system
 * call will repeat forever.
 *
 * Since none of the OS/161 system calls have more than 4 arguments,
 * there should be no need to fetch additional arguments from the
 * user-level stack.
 *
 * Watch out: if you make system calls that have 64-bit quantities as
 * arguments, they will get passed in pairs of registers, and not
 * necessarily in the way you expect. We recommend you don't do it.
 * (In fact, we recommend you don't use 64-bit quantities at all. See
 * arch/mips/include/types.h.)
 */

void
mips_syscall(struct trapframe *tf)
{
	int callno;
	int32_t retval;
	int err;

	assert(curspl==0);

	callno = tf->tf_v0;

	/*
	 * Initialize retval to 0. Many of the system calls don't
	 * really return a value, just 0 for success and -1 on
	 * error. Since retval is the value returned on success,
	 * initialize it to 0 by default; thus it's not necessary to
	 * deal with it except for calls that return other values, 
	 * like write.
	 */

	retval = 0;

	switch (callno) {
	    case SYS_reboot:
		err = sys_reboot(tf->tf_a0);
		break;

	    /* Add stuff here */
	    // Process fork
	    // return two pids
		// god bless me I can implement this
	    case SYS_fork:
	    err = sys_fork(tf,(pid_t *)&retval);
	    break;

	    case SYS_write:
	    err = sys_write((int)tf->tf_a0, (userptr_t)tf->tf_a1, (size_t)tf->tf_a2, (int *)&retval);
	    break;

	    case SYS_read:
	    err = sys_read((int)tf->tf_a0, (userptr_t)tf->tf_a1, (size_t)tf->tf_a2, (int *)&retval);
	    break;

	    case SYS_getpid:
	    err = sys_getpid((pid_t *)&retval);
	    break;


	    case SYS_waitpid:
	    err = sys_waitpid((pid_t)tf->tf_a0,(int *)tf->tf_a1, (int)tf->tf_a2, (pid_t *)&retval);
	    break;

	    case SYS__exit:
		sys__exit((int)tf->tf_a0);
		/* sys__exit does not return, execution should not get here */
		panic("unexpected return from sys__exit");
		break;

		case SYS___time:
		err = sys___time((time_t *)tf->tf_a0,(unsigned long *)tf->tf_a1, (int *)&retval);
		break;

		case SYS_sbrk:
		err = sys_sbrk((intptr_t)tf->tf_a0, (int *)&retval);
		break;

		case SYS_execv:
		/* sys_execv only returns on error. */
		err = sys_execv((char *)tf->tf_a0, (char **)tf->tf_a1, (int *)&retval);
		break;

	    default:
		kprintf("Unknown syscall %d\n", callno);
		err = ENOSYS;
		break;
	}


	if (err) {
		/*
		 * Return the error code. This gets converted at
		 * userlevel to a return value of -1 and the error
		 * code in errno.
		 */
		tf->tf_v0 = err;
		tf->tf_a3 = 1;      /* signal an error */
	}
	else {
		/* Success. */
		tf->tf_v0 = retval;
		tf->tf_a3 = 0;      /* signal no error */
	}
	
	/*
	 * Now, advance the program counter, to avoid restarting
	 * the syscall over and over again.
	 */
	
	tf->tf_epc += 4;

	/* Make sure the syscall code didn't forget to lower spl */
	assert(curspl==0);
}

void
md_forkentry(struct trapframe *tf)
{
	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		kprintf("At md_forkentry().\n");
	}
	#endif
	/*
	 * This function is provided as a reminder. You need to write
	 * both it and the code that calls it.
	 *
	 * Thus, you can trash it and do things another way if you prefer.
	 */

	// Duplicate the trap frame so that it's on this stack
	assert(curproc->p_vmspace != NULL);

	struct trapframe * ktf = tf; // copy over the trap frame onto the kernel stack
	assert(ktf != NULL);

	// memcpy(&ktf, tf, sizeof(struct trapframe));
	struct trapframe newtf = *ktf;

	newtf.tf_v0 = 0; // Return value PID should be 0
	newtf.tf_a3 = 0; // No errors ocurred

	// Advance program counter
	newtf.tf_epc += 4;

	kfree(tf); // no longer needed

	as_activate(curproc->p_vmspace);

	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		kprintf("md_forkentry(): as_activate passed. newtf ptr: %04x.\n", &newtf);
	}
	#endif

	// Go back into user mode!
	curproc->usermode = 1;
	mips_usermode(&newtf);
	panic("Unexpected return from mips_usermode().\n");

	(void)tf;
}

int ptrcheck(vaddr_t ptr){
	if (ptr == NULL || (unsigned)ptr>=USERTOP || (int)ptr == 0x40000000){
		return 1;
	}
	return 0;
}
