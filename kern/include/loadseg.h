#ifndef _LOAD_SEG_H_
#define _LOAD_SEG_H_

struct vnode;

int load_segment(struct vnode *v, off_t offset, vaddr_t vaddr, 
	     size_t memsize, size_t filesize,
	     int is_executable);


#endif /* _LOAD_SEG_H_ */
