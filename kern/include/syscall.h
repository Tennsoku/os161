#ifndef _SYSCALL_H_
#define _SYSCALL_H_

/*
 * Prototypes for IN-KERNEL entry points for system call implementations.
 */

int sys_reboot(int code);
int sys_write(int fd, userptr_t buf, size_t nbytes, int * retval);
int sys_read(int fd, userptr_t buf, size_t buflen, int * retval);
int sys_fork(struct trapframe *tf, pid_t * retval);

// If waitpid is called from kernel level, use retval. If from user level,
// use status.
int sys_waitpid(pid_t pid, int *status , int options, pid_t *retval);
int sys_getpid(pid_t * retval);
void sys__exit(int exitcode);
int sys_execv(const char *progname, char **args, int *retval);
int sys___time(time_t *seconds, unsigned long *nanoseconds, int *retval);
int sys_sbrk(intptr_t amount, int *retval);


// Check printer validity for bad call
int ptrcheck(vaddr_t ptr);

#endif /* _SYSCALL_H_ */
