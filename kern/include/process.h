#ifndef _PROCESS_H_
#define _PROCESS_H_

struct addrspace;
struct vnode;
struct process;
struct thread;
struct lock;

/*
 * Definition of a process.
 */


struct process {
	char * p_name; 					// process name
	pid_t p_id; 					// unique process id, 0 should always be system.

	struct array *p_threads;	// threads under this process
	struct array *p_children;	// child processes under this process
	pid_t p_parent;				// parent processes, if exist

	/*	
	 *	Nov 7th newly added
	 *	members used by waitpid and exit.
	 *	interest_src(interest source) stores an array of processes interested
	 *	in this process. Before the array is emptied, this process cannot be 
	 *  destroyed.
	 *	exited: this process is currently exited or not.
	 *	exitcode: if not exited, this will be 0. 
	 *	(Though most of exitcodes are 0 too, I cannot find a better initial val.)
	 */
	struct array *interest_src;
	int exited;
	int exitcode;

	/*	
	 *	Nov 10th newly added
	 *	Explain this process is currently under usermode or not.
	 */
	int usermode;

	/*
	 * Curthread's vmspace is exactly same as curproc. So actually vmspace is 
	 * defined under process, not thread.
	 */
	struct addrspace *p_vmspace;

	/*
	 * This is public because it isn't part of the process system,
	 * and is manipulated by the virtual filesystem (VFS) code.
	 */
	struct vnode *p_cwd;

	/*
	 * Locks is required for multiple user processes. Race condition may occur
	 * when multiple processes are accessing thread/child list.
	 */
	struct lock *p_lock;

	/*
	 * Vnode pointer to console. Used through curproc to I/O from/to
	 * console by user.
	 */
	struct vnode *console;
};
	
// Called by main.c to create highest system process
void process_bootstrap(void);

/*
 * Process Creator should be static.
 */
// static struct process * process_create(const char * name);

/*
 * Return a new process under current process. Vmspace is copied from parent, 
 * and console is connected in this function.
 */
struct process * process_fork(const char * name);

/*
 * Only when current process is system process, i.e. it is the last process in process
 * list, it can destroy itself. Otherwise it will be destroyed by other process who is
 * interested in its exitcode.
 */

void process_destroy(struct process *proc);

// Thread list operations. If function returns int, it returns errno.
int process_addthread(struct process *proc, struct thread *t);
void process_rmthread(struct thread *t);

// Child list operations. If function returns int, it returns errno.
int process_addchild(struct process *parent, struct process *child);
int process_rmchild(pid_t parent, struct process *child);

/*
 * Generates pid for new process. If pid list has no space, returns PID_MAX+1.
 */

pid_t process_gen_pid();

/*
 * Process list functions. Add/Remove process from global process tracking list
 */

struct process * proc_list_find_by_pid(pid_t pid);
int proc_list_remove(pid_t pid);

/*
 * Interest_src functions. check_interest() will check target's interest list, if list 
 * is empty, destroy this process.
 */
int process_addinterest(struct process *target, struct process *src);
int process_rminterest(struct process *target, struct process *src);
void check_interest(struct process *target);

#endif /* _PROCESS_H_ */
