#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <thread.h>
#include <curthread.h>
#include <addrspace.h>
#include <array.h>
#include <vm.h>
#include <machine/spl.h>
#include <machine/tlb.h>
#include <machine/vpage.h>
#include <machine/coremap.h> /* for coremap */


/* under notdumbvm, always have 48k of user stack */
#define VM_STACKPAGES    512

extern struct coremap_info coremap_fact;
extern struct coremap_entry *coremap;
extern paddr_t firstpaddr;
extern paddr_t lastpaddr;

void
vm_bootstrap(void)
{
	#ifdef _DB_MODE_
		if (_DB_MODE_) {	
			int i;
			for(i=0;i<NUM_TLB;i++){
				// kprintf("Check coremap TLB entry: coremap[%d] = %d.\n", i, coremap[i].cm_tlb_entry);
				// kprintf("Check coremap allocated: coremap[%d] = %d.\n", i, coremap[i].cm_allocated);
			}
		}
	#endif

	// swap_bootstrap();
}

static
paddr_t
getppages(unsigned long npages)
{
	int spl;
	paddr_t addr;
	unsigned int page_index;

	spl = splhigh();

	if(npages > coremap_fact.cm_free){
		splx(spl);
		return 0;
	}

	page_index = 0; // WEIRD: page_index cannot be 0 here, otherwise it will return 0 twice. I can't understand why, the only is to set it from 1 and use page_index-1 later.

	// #ifdef _DB_MODE_
	// 	if (_DB_MODE_) {	
	// 		kprintf("getppages check point 1. firstpaddr = %04x. cm_base = %04x. lastpaddr = %04x.\n",firstpaddr,coremap_fact.cm_base, lastpaddr);
	// 	}
	// #endif

	// #ifdef _DB_MODE_
	// 	if (_DB_MODE_) {	
	// 		kprintf("getppages check point 1. cm_total = %d. npages = %d. cm_free = %d.\n",coremap_fact.cm_total, npages, coremap_fact.cm_free);
	// 	}
	// #endif

	for(;;){
		if (page_index > coremap_fact.cm_total) {
			splx(spl);
			return 0;
		}

		if (coremap[page_index].cm_allocated == 1) {
			page_index++;
			continue;
		}

		// are there enough consecutive pages?
		// TODO: evict pages to obtain consecutive space.
		unsigned int j;
		unsigned int noConsecutive = 0;

		for (j=0; j < npages;j++){
			if (page_index+j > coremap_fact.cm_total){
				splx(spl);
				return 0;
			}
			if (coremap[page_index+j].cm_allocated == 1){
				page_index = page_index+j+1;
				noConsecutive = 1;
				break;
			}
		}
		if(noConsecutive) continue;

		// WEIRD: the very first page allocation in main() will not enter this loop. WHY????
		j = 0;
		do {
			coremap[page_index+j].cm_allocated = 1;
			coremap[page_index+j].cm_next = 1;
			j++;
		} while(j<npages);
		coremap[page_index+npages-1].cm_next = 0;
		break;
	}

	addr = firstpaddr + page_index * PAGE_SIZE;
	coremap_fact.cm_free -= npages;

	// addr = ram_stealmem(npages);

	// #ifdef _DB_MODE_
	// 	if (_DB_MODE_) {	
	// 		kprintf("getppages check point 2. addr = %04x. firstpaddr = %04x. ram_stealmem = %04x. page_index = %d.\n", firstpaddr + page_index * PAGE_SIZE, firstpaddr, addr, page_index);
	// 	}
	// #endif

	// #ifdef _DB_MODE_
	// 	if (_DB_MODE_) {	
	// 		kprintf("getppages check point 3. cm_total = %d. cm_base = %04x. firstpaddr-cm_base = %d pages.\n", coremap_fact.cm_total, coremap_fact.cm_base, (firstpaddr-coremap_fact.cm_base)/PAGE_SIZE);
	// 	}
	// #endif

	// #ifdef _DB_MODE_
	// 	if (_DB_MODE_) {	
	// 		kprintf("totalmem = %d. sizeof coremap_entry: %d. totalpages: %d.\n", ROUNDUP(coremap_fact.cm_total * sizeof(struct coremap_entry), PAGE_SIZE), sizeof(struct coremap_entry), coremap_fact.cm_total);
	// 	}
	// #endif

	// #ifdef _DB_MODE_
	// 	if (_DB_MODE_) {	
	// 		kprintf("coremap address = %04x. cm_base = %04x. coremap[1]: %04x.\n", coremap, coremap_fact.cm_base, coremap[1]);
	// 	}
	// #endif


	splx(spl);
	return addr;
}

/* Allocate/free some kernel-space virtual pages */
vaddr_t 
alloc_kpages(int npages)
{
	paddr_t pa;
	pa = getppages(npages);
	if (pa==0) {
		return 0;
	}
	return PADDR_TO_KVADDR(pa);
}

void 
free_kpages(vaddr_t addr)
{
	/* nothing */

	paddr_t paddr = KADDR_TO_PADDR(addr);
	assert(paddr % PAGE_SIZE == 0); // must be the address of a page

	// convert physical address to page number
	unsigned int page_index = PADDR_TO_NPAGE(paddr);

	// assert(page_index < coremap_fact.cm_total);
	if (page_index > coremap_fact.cm_total){
		kprintf("panic: Segmentation Fault on free_kpages.\n");
		return;
	}

	// #ifdef _DB_MODE_
	// 	if (_DB_MODE_) {	
	// 		kprintf("free_kpages check point 1. page_index = %d.\n", page_index);
	// 	}
	// #endif
	if (coremap[page_index].cm_allocated == 0) return; // Return if this is an unused page

	do {
		// Free the page
		coremap[page_index].cm_allocated = 0;
		coremap_fact.cm_free += 1;
		if(coremap[page_index].cm_next){
			coremap[page_index].cm_next = 0;
			page_index++;
			continue;
		}
		break;

	} while(1);
}

int
vm_fault(int faulttype, vaddr_t faultaddress)
{
	// if (faulttype < 0) return EFAULT;
	vaddr_t vbase1, vtop1, vbase2, vtop2, stackbase, stacktop, heapbase, heaptop;
	paddr_t paddr;
	u_int32_t ehi, elo;
	struct addrspace *as;
	int spl;

	spl = splhigh();

	faultaddress &= PAGE_FRAME;

	// #ifdef _DB_MODE_
	// 	if (_DB_MODE_) {	
	// 		kprintf("vm_fault on 0x%04x.\n", faultaddress);
	// 	}
	// #endif

	switch (faulttype) {
	    case VM_FAULT_READONLY:
		/* We always create pages read-write, so we can't get this */
		panic("dumbvm: got VM_FAULT_READONLY\n");
	    case VM_FAULT_READ:
	    case VM_FAULT_WRITE:
		break;
	    default:
		splx(spl);
		return EINVAL;
	}

	as = curthread->t_vmspace;
	if (as == NULL) {
		/*
		 * No address space set up. This is probably a kernel
		 * fault early in boot. Return EFAULT so as to panic
		 * instead of getting into an infinite faulting loop.
		 */
		kprintf("panic: as == NULL.\n");
		splx(spl);
		return EFAULT;
	}

	/* Assert that the address space has been set up properly. */
	assert(as->as_vbase1 != 0);
	assert(as->as_pbase1 != 0);
	assert(as->as_npages1 != 0);
	// assert(as->as_vbase2 != 0);
	// assert(as->as_pbase2 != 0);
	// assert(as->as_npages2 != 0);
	// kprintf("as->as_pbase1: %04x.\n",as->as_pbase1);

	// assert((as->as_vbase1 & PAGE_FRAME) == as->as_vbase1);
	// assert((as->as_pbase1 & PAGE_FRAME) == as->as_pbase1);
	
	// assert((as->as_vbase2 & PAGE_FRAME) == as->as_vbase2);
	// assert((as->as_pbase2 & PAGE_FRAME) == as->as_pbase2);
	// assert((as->as_stackpbase & PAGE_FRAME) == as->as_stackpbase);

	vbase1 = as->as_vbase1;
	vtop1 = vbase1 + as->as_npages1 * PAGE_SIZE;
	vbase2 = as->as_vbase2;
	vtop2 = vbase2 + as->as_npages2 * PAGE_SIZE;
	stackbase = USERSTACK - VM_STACKPAGES * PAGE_SIZE;
	stacktop = USERSTACK;

	heapbase = as-> as_heapbase;
	heaptop = as-> as_heaptop;

	if (faultaddress >= vbase1 && faultaddress < vtop1) {
		paddr = (faultaddress - vbase1) + as->as_pbase1;
	}
	else if (faultaddress >= vbase2 && faultaddress < vtop2) {
		paddr = (faultaddress - vbase2) + as->as_pbase2;
	}
	// get stack page
	else if (faultaddress >= stackbase && faultaddress < stacktop) {
		struct array * stack = as-> stack;
		int num, i;
		num = array_getnum(stack);

		paddr = 0;

		for(i=0;i<num;i++){
			struct vpage *vp = array_getguy(stack, i);

			if(faultaddress >= vp->vaddr && faultaddress < vp->vaddr+vp->pages * PAGE_SIZE){
				paddr = vp-> paddr + (faultaddress-vp->vaddr);
			}
		}

		if(paddr == 0){
			struct vpage *nvp = vpage_create(faultaddress, 1);
			array_add(stack,nvp);
			paddr = nvp->paddr;
		}

	}
	else if (faultaddress >= heapbase && faultaddress < heaptop) {
		struct array * heap = as-> heap;
		int num, i;
		num = array_getnum(heap);

		for(i=0;i<num;i++){
			struct vpage *vp = array_getguy(heap, i);

			if(faultaddress >= vp->vaddr && faultaddress < vp->vaddr+vp->pages * PAGE_SIZE){
				paddr = vp-> paddr + (faultaddress-vp->vaddr);
			}
		}
	}
	else {
		kprintf("panic: Segmentation fault on vm_fault().\n");
		splx(spl);
		return EFAULT;
	}

	/* make sure it's page-aligned */
	assert((paddr & PAGE_FRAME)==paddr);


	// Handle TLB miss
	u_int32_t TLB_Entry = TLB_Get_Slot();
	ehi = faultaddress;
	elo = paddr | TLBLO_DIRTY | TLBLO_VALID;
	u_int32_t page_index = PADDR_TO_NPAGE(paddr);

	// #ifdef _DB_MODE_
	// 	if (_DB_MODE_) {	
	// 		kprintf("vm_fault check point 0. Fault on paddr 0x%04x. Available TLB_Entry: %d.\n", paddr, TLB_Entry);
	// 	}
	// #endif

	// #ifdef _DB_MODE_
	// 	if (_DB_MODE_) {	
	// 		kprintf("vm_fault check point 1. page_index = %d. cm_tlb_entry = %d.\n", page_index, coremap[page_index].cm_tlb_entry);
	// 	}
	// #endif

	TLB_Write(ehi, elo, TLB_Entry);
	// assert(coremap[page_index].cm_tlb_entry == -1);
	coremap[page_index].cm_tlb_entry = TLB_Entry;

	splx(spl);
	return 0;

}

struct addrspace *
as_create(void)
{
	struct addrspace *as = kmalloc(sizeof(struct addrspace));
	if (as==NULL) {
		return NULL;
	}

	as->as_vbase1 = 0;
	as->as_pbase1 = 0;
	as->as_npages1 = 0;
	as->as_vbase2 = 0;
	as->as_pbase2 = 0;
	as->as_npages2 = 0;
	as->as_heapbase = 0;
	as->as_heaptop = 0;

	as->heap = array_create();
	if(as-> heap==NULL){
		kfree(as);
		return NULL;
	} 

	as->stack = array_create();
	if(as-> stack==NULL){
		array_destroy(as->heap);
		kfree(as);
		return NULL;
	} 

	// as->as_useg1 = kmalloc(sizeof(struct as_segment));
	// if (as->as_useg1==NULL) {
	// 	return NULL;
	// };

	// as->as_useg2 = kmalloc(sizeof(struct as_segment));
	// if (as->as_useg2==NULL) {
	// 	kfree(as->as_useg1);
	// 	return NULL;
	// };

	// as->as_useg_stack = kmalloc(sizeof(struct as_segment));
	// if (as->as_useg_stack==NULL) {
	// 	kfree(as->as_useg1);
	// 	kfree(as->as_useg2);
	// 	return NULL;
	// };

	// as->vaddr_table = array_create();
	// if (as->vaddr_table==NULL) {
	// 	kfree(as->as_useg1);
	// 	kfree(as->as_useg2);
	// 	kfree(as->as_useg_stack);
	// 	return NULL;
	// };

	return as;
}

void
as_destroy(struct addrspace *as)
{

	free_kpages(PADDR_TO_KVADDR(as->as_pbase1));
	free_kpages(PADDR_TO_KVADDR(as->as_pbase2));
	free_vpagetable(as->heap);
	free_vpagetable(as->stack);
	array_destroy(as->heap);
	array_destroy(as->stack);

	// kfree(as->as_useg1);
	// kfree(as->as_useg2);
	// kfree(as->as_useg_stack);
	// kfree(as->vaddr_table);

	kfree(as);
}

void
as_activate(struct addrspace *as)
{
	int spl;

	(void)as;

	spl = splhigh();

	TLB_Clear();

	splx(spl);
}

int
as_define_region(struct addrspace *as, vaddr_t vaddr, size_t sz,
		 int readable, int writeable, int executable)
{
	size_t npages; 

	// TODO: define segments here. Also initialize vpage table. 

	/* Align the region. First, the base... */
	sz += vaddr & ~(vaddr_t)PAGE_FRAME;
	vaddr &= PAGE_FRAME;

	/* ...and now the length. */
	sz = (sz + PAGE_SIZE - 1) & PAGE_FRAME;

	npages = sz / PAGE_SIZE;

	/* We don't use these - all pages are read-write */
	(void)readable;
	(void)writeable;
	(void)executable;

	if (as->as_vbase1 == 0) {
		as->as_vbase1 = vaddr;
		as->as_npages1 = npages;
		as->as_heapbase = vaddr + npages*PAGE_SIZE;
		as->as_heaptop = as->as_heapbase;
		// as_segment_define(as->as_useg1, vaddr, npages, 0);

		return 0;
	}

	if (as->as_vbase2 == 0) {
		as->as_vbase2 = vaddr;
		as->as_npages2 = npages;

		assert(as->as_heaptop == as->as_heapbase);

		as->as_heapbase = vaddr + npages*PAGE_SIZE;
		as->as_heaptop = as->as_heapbase;
		// as_segment_define(as->as_useg2, vaddr, npages, 0);

		return 0;
	}

	/*
	 * Support for more than two regions is not available.
	 */
	kprintf("notdumbvm: Warning: too many regions\n");
	return EUNIMP;
}

int
as_prepare_load(struct addrspace *as)
{
	assert(as->as_pbase1 == 0);
	assert(as->as_pbase2 == 0);

	as->as_pbase1 = getppages(as->as_npages1);
	if (as->as_pbase1 == 0) {
		return ENOMEM;
	}

	as->as_pbase2 = getppages(as->as_npages2);
	if (as->as_pbase2 == 0) {
		return ENOMEM;
	}

	return 0;
}

int
as_complete_load(struct addrspace *as)
{
	(void)as;
	return 0;
}

int
as_define_stack(struct addrspace *as, vaddr_t *stackptr)
{

	// TODO: when coremap->vpage installed, find vpage by as->stackpbase and 
	// return vaddr in vpage to stackptr here
	(void)as;

	*stackptr = USERSTACK;
	return 0;
}

int
as_copy(struct addrspace *old, struct addrspace **ret)
{
	struct addrspace *new;

	new = as_create();
	if (new==NULL) {
		return ENOMEM;
	}

	new->as_vbase1 = old->as_vbase1;
	new->as_npages1 = old->as_npages1;
	new->as_vbase2 = old->as_vbase2;
	new->as_npages2 = old->as_npages2;

	if (as_prepare_load(new)) {
		as_destroy(new);
		return ENOMEM;
	}

	assert(new->as_pbase1 != 0);
	assert(new->as_pbase2 != 0);

	memmove((void *)PADDR_TO_KVADDR(new->as_pbase1),
		(const void *)PADDR_TO_KVADDR(old->as_pbase1),
		old->as_npages1*PAGE_SIZE);

	memmove((void *)PADDR_TO_KVADDR(new->as_pbase2),
		(const void *)PADDR_TO_KVADDR(old->as_pbase2),
		old->as_npages2*PAGE_SIZE);

	// memmove((void *)(new->as_stackpbase),
	// 	(const void *)(old->as_stackpbase),
	// 	VM_STACKPAGES*PAGE_SIZE);

	// array_destroy(new->heap);
	// new->heap = copy_vpagetable(old->heap);
	// if(new->heap ==NULL){
	// 	as_destroy(new);
	// 	return ENOMEM;
	// }

	// array_destroy(new->stack);
	// new->stack = copy_vpagetable(old->stack);
	// if(new->stack ==NULL){
	// 	as_destroy(new);
	// 	return ENOMEM;
	// }

	
	*ret = new;
	return 0;
}
