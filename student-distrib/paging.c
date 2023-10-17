#include "paging.h"
#include "enable_paging.h"
#include "lib.h"

//page directory entry has PS flag
//PS = 0, page directory points to page table that points to 4kB pages
//PS and PSE = 1, page directory directly points to 4MB page


//page directory-> page table-> page
//index in page directory = ptr to a page table
//index in page table = phys mem addr to which page should be mapped

//page directory entry(PDE): bits 31:22 give offset to entry in page directory to get addr of page table
//page table entry (PTE): bits 21:12 give offset to entry in page table to get addr of page in phys memory
//page offset: bits 11:0 offset to phys addr in page

/* init_paging - Example
 * 
 * Sets paging table and directory
 * Inputs: None
 * Outputs: None
 * Side Effects: sets cr3, cr4, and cr0
 */
/* code inspired from https://wiki.osdev.org/Setting_Up_Paging */
void init_paging(){
    int i;
    int j;
    for (i = 0; i < VMEM_NUMBER_PAGES; i++)
    {
        // This sets the following flags to the pages:
        //   Supervisor: Only kernel-mode can access them
        //   Write Enabled: It can be both read from and written to
        //   Not Present: The page table is not present
        //
        page_directory[i] = 0x00000000;         // setting default to 0
    }
    
    for(j = 0; j < VMEM_NUMBER_PAGES; j++)
    {
        // As the address is page aligned, it will always leave 12 bits zeroed.
        // Those bits are used by the attributes
        if(j == VIDEO_OFFSET)
        {
            page_table[j] = (j * VMEM_PAGE_SIZE) | EN_SUPERVISER; // attributes: user level, read/write, present.
            j+=2;
            page_table[j] = (j * VMEM_PAGE_SIZE) | EN_SUPERVISER;
            j+=2;
            page_table[j] = (j * VMEM_PAGE_SIZE) | EN_SUPERVISER;
            j+=2;
            page_table[j] = (j * VMEM_PAGE_SIZE) | EN_SUPERVISER;
            
        }
    }
    
    page_directory[0] = ((unsigned int) page_table) | EN_SUPERVISER; // attributes: user level, read/write, present
    page_directory[1] = KERNEL_PAGE_SIZE_OFFSET | KERNEL_FLAGS;

    // load the page directory to cr3
    loadPageDirectory(page_directory);

    // enable paging
    enablePaging();
}



