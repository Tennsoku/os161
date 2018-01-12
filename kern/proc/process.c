/*
 * Core process system.
 */

#include <types.h>
#include <lib.h>
#include <kern/errno.h>
#include <kern/unistd.h>
#include <array.h>
#include <machine/spl.h>
#include <machine/pcb.h>
#include <thread.h>
#include <curthread.h>
#include <scheduler.h>
#include <addrspace.h>
#include <vnode.h>
#include <process.h>
#include <curproc.h>
#include <synch.h>
#include <vfs.h>
#include <kern/limits.h>

/* Global variable for the process currently executing at any given time. */
struct process *sysproc;
//struct process *curproc;

// gen_pid.
int *pid_list;


/*
 * Process track list. Used for searching, deleting, etc.
 * Contains all processes.
 */
struct array *proc_list;

/*
 * Since process list is critical region for all process, there should be 
 * a lock.
 */
struct lock *proc_list_lk;



/*	
 *	Create:
 * 	p_name		Generated
 *	p_id 		Generated	
 *	p_threads	Generated
 *	p_children	Generated
 *	p_vmspace	NULL
 *	p_cwd		NULL
 *	p_lock		Generated
 *	console 	NULL
 * 	p_parent 	PID_MAX+1
 *	exited 		0
 *	exitcode	0
 *	usermode	0
 *	interest_src Generated
 */

static struct process * process_create(const char * name){
	struct process *proc = kmalloc(sizeof(struct process));
	if (proc == NULL) return NULL;

	proc->p_name = kstrdup(name);
	if (proc->p_name==NULL) {
		kfree(proc);
		return NULL;
	}

	// Get pid 
	proc->p_id = process_gen_pid();
	if(proc->p_id > PID_MAX){
		kfree(proc->p_name);
		kfree(proc);
		return NULL;
	}

	// Create thread list
	proc->p_threads = array_create();
	if(proc->p_threads == NULL){
		pid_list[proc->p_id] = 0;
		kfree(proc->p_name);
		kfree(proc);
		return NULL;
	}

	// Initialize 
	proc->p_vmspace = NULL;
	proc->p_cwd = NULL;
	proc->console = NULL;
	proc->p_parent = PID_MAX+1;
	proc->exited = 0;
	proc->exitcode = 0;
	proc->usermode = 0;

	// Create child list
	proc->p_children = array_create();
	if(proc->p_children == NULL){
		pid_list[proc->p_id] = 0;
		kfree(proc->p_name);
		array_destroy(proc->p_threads);
		kfree(proc);
		return NULL;
	}

	// Create process lock
	proc->p_lock = lock_create("p_lock");
	if (proc->p_lock==NULL) {
		pid_list[proc->p_id] = 0;
		kfree(proc->p_name);
		array_destroy(proc->p_threads);
		array_destroy(proc->p_children);
		kfree(proc);
		return NULL;
	}

	// Create interest list
	proc->interest_src = array_create();
	if (proc->interest_src==NULL) {
		pid_list[proc->p_id] = 0;
		kfree(proc->p_name);
		array_destroy(proc->p_threads);
		array_destroy(proc->p_children);
		lock_destroy(proc->p_lock);
		kfree(proc);
		return NULL;
	}

	// If global process list is not existed, create one. This part should
	// be used and only be used during system bootstrap
	if(proc_list == NULL){
		proc_list = array_create();
		proc_list_lk = lock_create("proc_list");
	}

	// Add newly created process to global tracking list
	if(array_add(proc_list, proc)){
		pid_list[proc->p_id] = 0;
		kfree(proc->p_name);
		array_destroy(proc->p_threads);
		array_destroy(proc->p_children);
		array_destroy(proc->interest_src);
		lock_destroy(proc->p_lock);
		lock_destroy(proc_list_lk);
		kfree(proc);
		panic("New process has valid pid but cannot be added to proc list.\n");
		return NULL;
	}

	assert(proc_list != NULL);

	return proc;
}

/*
 * Create highest system process
 */
void process_bootstrap(void){
	int i;

	pid_list = kmalloc(sizeof(int) * PID_MAX);
	if(pid_list == NULL)
	{
		panic("Failed to create pid list.\n");
	}
	for(i=0;i<PID_MAX;i++){
		pid_list[i] = 0;
	}

	sysproc = process_create("System");
	if (sysproc == NULL){
		panic("System process creation failed. Boom.\n");
	}

	// Create a new address space for it since it is a brand new process
	struct addrspace *as = as_create();
	if (as == NULL){
		panic("Failed to initialize vmspace for system process.\n");
	}
	sysproc->p_vmspace = as;

	if (sysproc->p_id>0){
		panic("Someone came even before system!!\n");
	}

}


// Fork a new process
struct process * process_fork(const char * name){
	int result;
	char *console_path;
	struct process *proc;

	proc = process_create(name);
	if (proc == NULL){
		return NULL;
	}

	// Create a new address space for it since it is a brand new process
	// and the new addrspace should be a copy of parent proc
	result = as_copy(curproc->p_vmspace,&(proc->p_vmspace));
	if (result) {
		process_destroy(proc);
		return NULL;
	}

	result = process_addchild(curproc, proc);
	if (result) {
		process_destroy(proc);
		return NULL;
	}
	proc->p_parent = curproc->p_id;

	/* Inherit the current directory */
	if (curproc->p_cwd != NULL) {
		VOP_INCREF(curproc->p_cwd);
		proc->p_cwd = curproc->p_cwd;
	}

	/* Inherit the current mode */
	proc->usermode = curproc->usermode;

	/* Activate it when we get in. */
	// /* Activate it. */
	// as_activate(proc->p_vmspace);

	// Console should be installed here.
	// Open console for system process
	console_path = kstrdup("con:"); // metioned in man/dev/console
	if (console_path == NULL){
		panic("Console access failed. Boom.\n");
	}
	// Write & Read available
	result = vfs_open(console_path, O_RDWR, &(proc->console));
	if(result){
		panic("Error occurs during process opening console. Boom.\n");
	}

	// Almost forgot
	kfree(console_path);

	return proc;


}


/*
 * Destroy a process
 */

void process_destroy(struct process *proc){
	assert(proc != NULL);
	assert(proc != curproc);
	assert(proc != sysproc);

	// FIXME: how to destroy a vnode?
	if (proc->p_cwd){
		VOP_DECREF(proc->p_cwd);
		proc->p_cwd = NULL;
	}

	// FIXME: i dont know if this would explode...
	if (proc->p_vmspace){
		struct addrspace *as = proc->p_vmspace;
		// lock_acquire(curproc -> p_lock);
		proc->p_vmspace = NULL;
		as_destroy(as);
		// lock_release(curproc -> p_lock);
	}

	// FIXME: may explode
	/*
 	 * Should use vfs funcs since VOP_CLOSE should not be called directly from above
 	 * the VFS layer - use vfs_close() to close vnodes
 	 */
	if (proc->console){
		vfs_close(proc->console);
	}
	proc_list_remove(proc->p_id);
	pid_list[proc->p_id] = 0;

	// Remove all threads under this process. Should hold the lock during this operation.
	lock_acquire(proc->p_lock);
	int threadnum = array_getnum(proc->p_threads);
	int i;
	for(i=threadnum-1;i>=0;i--){
		struct thread *t = array_getguy(proc->p_threads, i);
		t->t_proc = NULL;
		array_remove(proc->p_threads, i);
	}
	lock_release(proc->p_lock);
	threadnum = array_getnum(proc->p_threads);
	assert(threadnum == 0);


	// If this process has parent, remove it from its parent's child list
	if(proc->p_parent<PID_MAX){
		process_rmchild(proc->p_parent, proc);
	}


	// Destroy objects
	lock_destroy(proc->p_lock);
	array_destroy(proc->p_threads);
	array_destroy(proc->p_children);
	array_destroy(proc->interest_src);

	kfree(proc->p_name);

	#ifdef _DB_MODE_
	if (_DB_MODE_) {	
		kprintf("Process %d is destroyed.\n",proc->p_id);
	}
	#endif

	kfree(proc);

}


// Add thread to process's thread list
int process_addthread(struct process *proc, struct thread *t){ 
	int result;

	assert(t->t_proc == NULL);
	assert(t->t_vmspace == NULL);
	assert(proc->p_vmspace != NULL);
	assert(proc != NULL);

	result = array_add(proc->p_threads, t);
	if(result){
		return result;
	}

	// t_proc
	t->t_proc = proc;

	// Inherite vmspace
	t->t_vmspace = proc->p_vmspace;

	return 0;
}

void process_rmthread(struct thread *t){
	int i;
	struct process *proc;

	proc = t->t_proc;

	assert(proc != NULL);
	assert(proc->p_threads != NULL);

	lock_acquire(proc->p_lock);
	for (i=0;i<array_getnum(proc->p_threads);i++){
		if(array_getguy(proc->p_threads, i) == t){
			array_remove(proc->p_threads, i);
			t->t_proc = NULL;
			lock_release(proc->p_lock);
			return;
		}
	}
	lock_release(proc->p_lock);

	panic("Thread escaped from its process before removed.");

}

int process_addchild(struct process *parent, struct process *child){ 
	int result;

	assert(child->p_parent == PID_MAX+1);
	assert(parent != NULL);
	assert(child != NULL);

	lock_acquire(parent->p_lock);

	result = array_add(parent->p_children, child);
	if(result){
		lock_release(parent->p_lock);
		return result;
	}

	child->p_parent = parent->p_id;

	lock_release(parent->p_lock);

	return 0;
}

int process_rmchild(pid_t parent_id, struct process *child){
	assert(parent_id < PID_MAX);
	assert(child != NULL);
	assert(child -> p_parent < PID_MAX);
	int i;

	struct process *parent = proc_list_find_by_pid(parent_id);
	if (parent == NULL){
		return EPROC;
	}

	lock_acquire(parent->p_lock);

	for (i=0;i<array_getnum(parent->p_children);i++){
		if(array_getguy(parent->p_children, i) == child){
			array_remove(parent->p_children, i);
			child->p_parent = PID_MAX+1;
			lock_release(parent->p_lock);
			return 0;
		}
	}
	lock_release(parent->p_lock);
	panic("Process escaped from its parent before removed.");
	return EPROC;

}

pid_t process_gen_pid(){
	int spl = splhigh();
	int i;
	for(i=0;i<PID_MAX; i++){
		if(pid_list[i] == 0){
			pid_list[i] = 1;
			splx(spl);
			return i;
		}
	}

	splx(spl);
	
	return PID_MAX+1;
}

// proc_list

struct process * proc_list_find_by_pid(pid_t pid){
	assert(pid < PID_MAX);

	int i;
	struct process *item;
	lock_acquire(proc_list_lk);
	// kprintf("proc_list_find_by_pid checkpoint 1.num: %d, PID_MAX: %d\n",num,PID_MAX);
	for (i=0;i<array_getnum(proc_list);i++){
		item = array_getguy(proc_list, i);
		// kprintf("proc_list_find_by_pid checkpoint 2. i: %d\n",i);
		if(item -> p_id == pid){
			lock_release(proc_list_lk);
			return item;
		}
	}
	lock_release(proc_list_lk);
	return NULL;
}

int proc_list_remove(pid_t pid){

	assert(pid < PID_MAX);

	lock_acquire(proc_list_lk);
	int i;
	struct process *item;
	for (i=0;i<array_getnum(proc_list);i++){
		item = array_getguy(proc_list, i);
		if(item-> p_id == pid){
			array_remove(proc_list, i);
			lock_release(proc_list_lk);
			return 0;
		}
	}
	lock_release(proc_list_lk);
	return EPROC;
}

// interest_src

int process_addinterest(struct process *target, struct process *src){ 
	int result;

	assert(src != NULL);
	assert(target != NULL);

	result = array_add(target->interest_src, src);
	if(result){
		return result;
	}

	return 0;
}

int process_rminterest(struct process *target, struct process *src){
	assert(src != NULL);
	assert(target != NULL);
	assert(target -> interest_src != NULL);
	int i;

	lock_acquire(target->p_lock);
	for (i=0;i<array_getnum(target->interest_src);i++){
		if(array_getguy(target->interest_src, i) == src){
			array_remove(target->interest_src, i);
			lock_release(target->p_lock);
			return 0;
		}
	}
	lock_release(target->p_lock);

	panic("Interest_src escaped before removed.");
	return EPROC;

}

void check_interest(struct process *target){
	assert(target != NULL);
	if(array_getnum(target->interest_src)==0){
		process_destroy(target);
	}

	return;
}
