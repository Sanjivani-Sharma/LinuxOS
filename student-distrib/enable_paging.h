#ifndef _ENABLE_PAGING_H
#define _ENABLE_PAGING_H
// loads cr3 with pointer in parameter
extern void loadPageDirectory(unsigned int*);

// enable paging
extern void enablePaging();

// flush TLB
extern void flush_TLB();

#endif
