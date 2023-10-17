#include "types.h"
#include "terminal_switch.h"


#define KERNEL_PHYS_ADDR 0x400000 
#define VIDEO_OFFSET       0xB8

#define VMEM_NUMBER_PAGES 1024 //number of pages to map vmem
#define VMEM_PAGE_SIZE 4096 //4*1024 = 4kB
#define KERNEL_PAGE_SIZE_OFFSET 0x400000 //4MB
#define EN_SUPERVISER   3                       // paging for following flags: superviser, read/write, and present
#define SUPERVISER_EN   7                       // paging for following flags: user, read/write, and present
#define KERNEL_FLAGS    0x183                   // paging for following flags for page directory: global, 4MB, read/write, and present
#define FOURMB_USER_FLAGS      0x87             // paging for following flags: 4MB, user, read/write, and present




 uint32_t page_directory[VMEM_NUMBER_PAGES] __attribute__((aligned(VMEM_PAGE_SIZE))); //all paging structures need to be page aligned

 uint32_t page_table[VMEM_NUMBER_PAGES] __attribute__((aligned(VMEM_PAGE_SIZE))); //all paging structures need to be page aligned

 uint32_t vid_page_table[MAX_TERMINAL_COUNT][VMEM_NUMBER_PAGES] __attribute__((aligned(VMEM_PAGE_SIZE))); //all paging structures need to be page aligned


//to initialize paging
extern void init_paging();

