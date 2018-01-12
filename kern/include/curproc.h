#ifndef _CURPROC_H_
#define _CURPROC_H_

/*
 * The current process & system process.
 *
 * This is in its own header file (instead of process.h) to reduce the
 * number of things that get recompiled when you change process.h.
 */

struct process;

/*
 * Definition of curproc.
 *
 * curproc is always the current thread's process.
 */

#define curproc (curthread->t_proc)

//extern struct process *curproc;
extern struct process *sysproc;

#endif /* _CURPROC_H_ */
