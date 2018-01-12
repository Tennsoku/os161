#ifndef _VM_COREMAP_
#define _VM_COREMAP_

#define PADDR_TO_NPAGE(paddr) ((paddr - firstpaddr)/PAGE_SIZE)
#define NPAGE_TO_PADDR(page) ((firstpaddr)+page*PAGE_SIZE)

struct coremap_info {
	paddr_t cm_base;		// base physical address
	u_int32_t cm_total;		// total physical frame number
	u_int32_t cm_free;		// current free frame number
};

struct coremap_entry {
	u_int32_t cm_uk:1;			// 0 kernel; 1 user
	u_int32_t cm_allocated:1;	// allocated or not
	u_int32_t cm_next:1; 		// if next page is malloced
	u_int32_t cm_vpage:1;		// virtual page
	int cm_tlb_entry:7;			// TLB entry; -1 on miss, 6+1 bit since -1
};

void coremap_bootstrap(void);
int coremap_getnew(size_t npages);
void coremap_free(int page_index);

#endif
