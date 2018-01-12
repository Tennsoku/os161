#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <thread.h>
#include <machine/spl.h>
#include <curthread.h>
#include <machine/coremap.h>
#include <vm.h>
#include <addrspace.h>
#include <machine/tlb.h>

extern paddr_t firstpaddr;
extern struct coremap_info coremap_fact;
extern struct coremap_entry *coremap;


void TLB_Clear(void){
	u_int32_t i;

	for(i = 0; i < NUM_TLB; i++)
		TLB_Invalid(i);
}

u_int32_t TLB_Get_Slot(void){
	int i;
	u_int32_t entryhi;
	u_int32_t entrylo;
	int	spl;

	//loop over all possible tlb entries.
	spl = splhigh();
	for( i = 0; i < NUM_TLB; ++i ) {
		//read the current entry.
		TLB_Read( &entryhi, &entrylo, i );	

		//if it is a valid entry, continue.
		if( entrylo & TLBLO_VALID )
			continue;
		
		//return interrupts level to what they were.
		splx(spl);

		//found a good entry.
		return  i;
	}
	
	splx(spl);
	return TLB_Evict();
}

u_int32_t TLB_Evict(void){
	u_int32_t victim;
	
	victim = random() % NUM_TLB;
	TLB_Invalid(victim);
	
	return victim;
}

void TLB_Invalid(u_int32_t index){
	u_int32_t		entrylo;
	u_int32_t		entryhi;
	paddr_t			paddr;
	u_int32_t		page_index;

	assert(index < NUM_TLB);

	//read the TLB entry given by index.
	TLB_Read( &entryhi, &entrylo, index );

	//check to see that the entry retrieved was valid.
	if( entrylo & TLBLO_VALID ) {
		//get the physical address mapped to it.
		paddr = entrylo & TLBLO_PPAGE;
	
		//get coremap index.
		page_index = PADDR_TO_NPAGE(paddr);
		
		assert(coremap[page_index].cm_tlb_entry == index);

		//invalidate it.
		coremap[page_index].cm_tlb_entry = -1;

		TLB_Write(TLBHI_INVALID(index), TLBLO_INVALID(), index);
	}
}

void TLB_Unmap(vaddr_t vaddr) {
	int index;
	// u_int32_t entryhi;
	// u_int32_t entrylo;
	
	// probe the tlb for the given vaddr.
	index = TLB_Probe( vaddr, 0 );

	// if it does not exist, then there's nothing to unmap.
	if(index < 0 )
		return;
	
	// //read the tlb entry.
	// TLB_Read(&entryhi, &entrylo, index);

	// // make sure it is a valid mapping.
	// assert(entrylo & TLBLO_VALID);

	// invalidate the entry.
	TLB_Invalid(index);
}
