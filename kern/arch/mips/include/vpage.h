#ifndef _VM_PAGE_H
#define _VM_PAGE_H


struct vpage {
	paddr_t				paddr;		/* the current physical address of this page */
	vaddr_t				vaddr;		/* the current virtual address of this page */
	size_t				pages;		/* how many pages start from this vaddr and paddr */
};

struct as_segment
{
	vaddr_t vbase;			// virtual address base for this segment
	vaddr_t vtop;			// virtual address top for this segment
	size_t nr_pages;		// pages in this segment
	int perm;				// permission
	paddr_t pbase;
	// int id;
};

// vpage related functions
struct vpage * vpage_create(vaddr_t vaddr, size_t pages);
void vpage_destroy(struct vpage *vp);

// vpage table related functions
struct vpage * vpage_table_aquire(vaddr_t vaddr);
void vpage_table_walk(void);
int vpage_table_insert(struct vpage *vpage);
void free_vpagetable(struct array * vpagetable);

struct array * copy_vpagetable(struct array * vpagetable);

// segment related functions
void as_segment_define(struct as_segment * seg, vaddr_t base, size_t npages, int perm);

#endif

