#include <types.h>
#include <lib.h>
#include <machine/trapframe.h>
#include <kern/callno.h>
#include <kern/errno.h>
#include <array.h>
#include <vnode.h>
#include <uio.h>
#include <addrspace.h>
#include <thread.h>
#include <curthread.h>
#include <process.h>
#include <curproc.h>
#include <machine/coremap.h>
#include <machine/vpage.h>
#include <syscall.h>

extern paddr_t firstpaddr;

// for some mysterious reasons _DB_MODE_ flag is not sensitive in this file
#define _DB_SBRK_ 0

int sys_sbrk(intptr_t amount, int *retval){
	#ifdef _DB_SBRK_
	if (_DB_SBRK_) {	
		kprintf("SYSCALL: _sbrk.\n");
	}
	#endif

	assert(curproc != NULL);
	assert(curproc -> p_vmspace != NULL);

	struct array * heaptable = curproc->p_vmspace->heap;
	vaddr_t heaptop = curproc->p_vmspace->as_heaptop;
	vaddr_t heapbase = curproc->p_vmspace->as_heapbase;

	if(amount == 0) {
		*retval = heaptop;
		#ifdef _DB_SBRK_
		if (_DB_SBRK_) {	
			kprintf("_sbrk: Amount is 0. Return heaptop: %04x.\n", *retval);
		}
		#endif
		return 0;
	}

	// if unaligned
	if(amount % 4 != 0){
		*retval = ((void *) -1);
		#ifdef _DB_SBRK_
		if (_DB_SBRK_) {	
			kprintf("_sbrk: Error on unaligned.\n");
		}
		#endif
		return EINVAL;
	}

	// // align
	// amount = ROUNDUP(amount, 16);

	// if sufficient virtual memory to satisfy the request was not available, or the process has reached the limit of the memory it is allowed to allocate.
	if ((int)(heaptop+amount) > (USERTOP - 1024 * PAGE_SIZE)){
		*retval = ((void *) -1);
		#ifdef _DB_SBRK_
		if (_DB_SBRK_) {	
			kprintf("_sbrk: Error on no sufficient virtual memory. amount = %d.\n", amount);
		}
		#endif
		return ENOMEM;
	}

	// if the request would move the "break" below its initial value.
	if ((int)(heaptop+amount) < (int)heapbase){
		*retval = ((void *) -1);
		#ifdef _DB_SBRK_
		if (_DB_SBRK_) {	
			kprintf("_sbrk: Error on below initial heapbase. amount = %d.\n", amount);
		}
		#endif
		return EINVAL;
	}

	// if amount > 0, we may need to allocate new pages.
	if (amount > 0){
		assert(heaptop == curproc->p_vmspace->as_heaptop);


		/***************************************************************
		 	Quote this part if creating page in vm_fault is not quoted.
		 	These codes are for passing malloctest 5.
		 ***************************************************************/



		// int num, i;
		// num = array_getnum(heaptable);

		// for(i=0;i<num;i++){
		// 	struct vpage *vp = array_getguy(heaptable, i);

		// 	if((heaptop + amount) <= (vp->vaddr + vp->pages * PAGE_SIZE)){

		// 		#ifdef _DB_SBRK_
		// 		if (_DB_SBRK_) {	
		// 			kprintf("_sbrk: increased heaptop %04x is inside a created vpage. No need to allocate new one.\n", curproc->p_vmspace->as_heaptop + amount);
		// 		}
		// 		#endif

		// 		curproc->p_vmspace->as_heaptop += amount;

		// 		*retval = heaptop;

		// 		return 0;
		// 	}
		// }

		// assert(heaptop == curproc->p_vmspace->as_heaptop);
		// // now allocate new page(s)
		// int nnewpages = ROUNDUP(amount, PAGE_SIZE) / PAGE_SIZE;
		// struct vpage * nvp = vpage_create(heaptop, nnewpages);
		// if(nvp == NULL){
		// 	#ifdef _DB_SBRK_
		// 	if (_DB_SBRK_) {	
		// 		kprintf("_sbrk: Error on allocate new pages. amount = %d.\n", amount);
		// 	}
		// 	#endif

		// 	*retval = ((void *) -1);
		// 	return ENOMEM;
		// }

		// int result;
		// result = array_add(heaptable, nvp);
		// if(result){

		// 	#ifdef _DB_SBRK_
		// 	if (_DB_SBRK_) {	
		// 		kprintf("_sbrk: Error on update heaptable. amount = %d.\n", amount);
		// 	}
		// 	#endif

		// 	kfree(nvp);
		// 	*retval = ((void *) -1);
		// 	return ENOMEM;
		// }

		// #ifdef _DB_SBRK_
		// if (_DB_SBRK_) {	
		// 	kprintf("_sbrk: nvp added to heaptable. vaddr = %04x, pages = %d, heaptop = %04x.\n", nvp-> vaddr, nvp-> pages, heaptop);
		// }
		// #endif

		curproc->p_vmspace->as_heaptop += amount;

		*retval = heaptop;

		#ifdef _DB_SBRK_
		if (_DB_SBRK_) {	
			kprintf("_sbrk: Successfully returned on extend. amount = %d, returned heaptop = %04x, current heaptop = %04x.\n", amount, *retval, curproc->p_vmspace->as_heaptop);
		}
		#endif
		
		return 0;
	}
	// else we can move the top down and free tables no longer used.
	else {
		assert(heaptop == curproc->p_vmspace->as_heaptop);

		curproc->p_vmspace->as_heaptop += amount;

		int num, i;
		num = array_getnum(heaptable);

		for(i=0;i<num;i++){
			struct vpage *vp = array_getguy(heaptable, i);

			if(curproc->p_vmspace->as_heaptop <= vp->vaddr){
				#ifdef _DB_SBRK_
				if (_DB_SBRK_) {	
					kprintf("_sbrk: Removed one vpage in heap.\n");
				}
				#endif
				paddr_t paddr = vp-> paddr;
				int page_index = PADDR_TO_NPAGE(paddr);

				array_remove(heaptable, i);
				i--;
				num--;
				kfree(vp);

				coremap_free(page_index);
			}
		}

		curproc->p_vmspace->as_heaptop += amount;

		*retval = heaptop;

		#ifdef _DB_SBRK_
		if (_DB_SBRK_) {	
			kprintf("_sbrk: Successfully returned on decrease. amount = %d, returned heaptop = %04x, current heaptop = %04x.\n", amount, *retval, curproc->p_vmspace->as_heaptop);
		}
		#endif

		return 0;
	}

}
