#include <types.h>
#include <kern/errno.h>
#include <lib.h>
#include <array.h>
#include <vm.h>
#include <curthread.h>
#include <thread.h>
#include <curproc.h>
#include <process.h>
#include <addrspace.h>
#include <machine/coremap.h>
#include <machine/vpage.h>
#include <uio.h>

extern paddr_t firstpaddr;
extern paddr_t lastpaddr;



struct vpage * vpage_create(vaddr_t vaddr, size_t npages){
	struct vpage *newpage = kmalloc(sizeof(struct vpage));
	if (newpage == NULL){
		return NULL;
	}

	if (vaddr % PAGE_SIZE !=0 ){
		vaddr = ROUNDUP(vaddr, PAGE_SIZE)-PAGE_SIZE;
	}

	newpage->vaddr = vaddr;

	int page_index;
	page_index = coremap_getnew(npages);

	if(page_index == 0){
		kfree(newpage);
		return NULL;
	}
	newpage->paddr = NPAGE_TO_PADDR(page_index);
	newpage->pages = npages;

	return newpage;

}


void vpage_table_walk(void){

}


void free_vpagetable(struct array * vpagetable){
	
	if (vpagetable == NULL) return;

	int num, i;
	num = array_getnum(vpagetable);

	for(i=num-1;i>=0;i--){
		struct vpage *vp = array_getguy(vpagetable, i);

		vpage_destroy(vp);

		array_remove(vpagetable, i);
		kfree(vp);
	}
}

void vpage_destroy(struct vpage *vp){
	paddr_t paddr = vp-> paddr;
	int page_index = PADDR_TO_NPAGE(paddr);
	coremap_free(page_index);
}

struct array * copy_vpagetable(struct array * vpagetable){
	assert(vpagetable!=NULL);

	struct array *ret = array_create();
	if (ret == NULL){
		return NULL;
	}

	int num, i;
	num = array_getnum(vpagetable);

	for(i=0;i<num;i++){
		struct vpage *vp = array_getguy(vpagetable, i);
		struct vpage *nvp = vpage_create(vp->vaddr, vp->pages);

		assert(vp->vaddr % PAGE_SIZE ==0);
		assert(nvp->vaddr % PAGE_SIZE ==0);

		if (nvp ==NULL){
			vpage_destroy(nvp);
			kfree(nvp);
			array_destroy(ret);
			return NULL;
		}

		assert(vp->vaddr % PAGE_SIZE ==0);
		assert(nvp->vaddr % PAGE_SIZE ==0);

		int result = array_add(ret, nvp);
		if(result){
			kprintf("Panic: out of mem in copy_vpagetable().\n");
		}

	}

	return ret;
}
