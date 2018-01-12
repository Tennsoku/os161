#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <vm.h>
#include <machine/spl.h>
#include <machine/coremap.h>

extern paddr_t firstpaddr;
extern paddr_t lastpaddr;

struct coremap_entry *coremap;
struct coremap_info coremap_fact;

void coremap_bootstrap(void){
	u_int32_t nframes;		// total number of physical frames
	// uint32_t size;
	size_t totalmem;		// total memory coremap needed for bootstrap
	u_int32_t i;

	// get total physical frames number
	nframes = (lastpaddr - firstpaddr)/PAGE_SIZE;

	// calculate coremap size
	totalmem = ROUNDUP(nframes * sizeof(struct coremap_entry), PAGE_SIZE);
	totalmem += ROUNDUP(sizeof(struct coremap_info), PAGE_SIZE);
	assert(totalmem % PAGE_SIZE == 0);

	// since coremap_bootstrap() is right after ram_bootstrap(),
	// we set coremap in physical address and will reduce total page number

	coremap_fact.cm_base = firstpaddr;
	firstpaddr = coremap_fact.cm_base + totalmem;

	// actual physical frame reduces because some of them are used for coremap
	coremap_fact.cm_total = (lastpaddr - firstpaddr)/PAGE_SIZE;
	coremap_fact.cm_free = coremap_fact.cm_total;

	coremap = PADDR_TO_KVADDR(coremap_fact.cm_base);

	for(i=0;i<coremap_fact.cm_total;i++){
		coremap[i].cm_uk = 0;			// 0 kernel; 1 user
		coremap[i].cm_allocated = 0;	// allocated or not
		coremap[i].cm_next = 0;			// if next page is malloced
		coremap[i].cm_vpage = 0;		// virtual page
		coremap[i].cm_tlb_entry = -1;	// TLB entry
	}
}


// same as getppages. 
int coremap_getnew(size_t npages){
	int spl;
	unsigned int page_index;

	spl = splhigh();

	if(npages > coremap_fact.cm_free){
		splx(spl);
		return 0;
	}

	page_index = 0; 

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

		j = 0;
		do {
			coremap[page_index+j].cm_allocated = 1;
			coremap[page_index+j].cm_next = 1;
			j++;
		} while(j<npages);
		coremap[page_index+npages-1].cm_next = 0;
		break;
	}

	coremap_fact.cm_free -= npages;

	splx(spl);
	return page_index;

}

void coremap_free(int page_index){
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
