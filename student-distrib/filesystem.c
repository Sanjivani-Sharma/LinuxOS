#include "filesystem.h"
#include "lib.h"
#include "Sys_Calls.h"

int test_flag = 0;

/*
*   fs_init
*   DESCRIPTION: initializes all pointers and block structures
*   INPUTS: boot_addr -- address of the boot block
*   OUTPUT: NONE
*   SIDE EFFECTS: NONE
*/
void fs_init(uint32_t boot_addr) {
    // initializing all pointers and data blocks
    bootblock_ptr = (bootblock_t*) boot_addr;                                   //beginning of filesystem
    number_dir = bootblock_ptr->number_dir_entries;
    number_inode = bootblock_ptr->number_inode;
    number_data_blocks = bootblock_ptr->number_data_blocks;
    inode_ptr = boot_addr + BLOCK_SIZE;
    datablock_ptr = boot_addr + (number_inode + 1) * BLOCK_SIZE;
}

/* 
 * read_dentry_by_name
 *   DESCRIPTION: find the file in the file system with the same name as fname and
 *                call read_dentry_by_index to copy everything into dentry object
 *   INPUTS: fname -- file name to search for 
 *           dentry -- dentry object
 * 	 OUTPUTS: none
 *   SIDE EFFECTS: return 0 -- success
 *                 return -1 -- failure 
 *  
 */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry){
    int i;
    // iterating through directories to find directories that matches in name
    for(i = 0; i < number_dir; i++)
    {
        // checking if file name matches fname
        int8_t temp[MAXIMUM_LENGTH_FILENAME + 1];
        strncpy(temp, (int8_t*) bootblock_ptr->dir_entries[i].file_name, MAXIMUM_LENGTH_FILENAME);
        temp[MAXIMUM_LENGTH_FILENAME] = '\0';
        if(strncmp(temp, (int8_t*) fname, (uint32_t) (MAXIMUM_LENGTH_FILENAME + 1)) == 0)
        {
            // read dentry by index i
            read_dentry_by_index(i, dentry);
            return 0;
        }
    }
    return -1;
}

/* 
 * read_dentry_by_index
 *   DESCRIPTION: copy everything into dentry
 *   INPUTS: index -- index of dentry to copy
 *           dentry -- dentry object to copy into
 * 	 OUTPUTS: none
 *   SIDE EFFECTS: return 0 -- success
 *                 return -1 -- failure 
 */
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){
    //null ptr error or index outside filesys
    if(dentry == NULL || index > bootblock_ptr->number_dir_entries)
        return -1;

    //read file_name, file_type and inode_num
    strncpy((int8_t*) dentry->file_name, (int8_t*) bootblock_ptr->dir_entries[index].file_name, MAXIMUM_LENGTH_FILENAME);
    dentry->file_type = bootblock_ptr->dir_entries[index].file_type;
    dentry->inode_index = bootblock_ptr->dir_entries[index].inode_index;
    return 0;
}

/* 
 * read_data
 *   DESCRIPTION: copy data from datablocks to buf
 *   INPUTS: inode -- index of inode datablock info is in
 *           offset -- offset from start of data start to read from
 *           buf -- buffer array to transfer data into from data blocks
 *           length -- length of buffer
 * 	 OUTPUTS: number of data bytes read
 *   SIDE EFFECTS: NONE
 */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    int res = 0;                                                                // number of bytes read
    int buf_it = 0;                                                             // used to iterate through buffer

    // check if inode is valid
    if(inode >= bootblock_ptr->number_inode)
        return -1; 
    
    // getting pointer to inodeblock
    inode_t* inodeblock_ptr = (inode_t*)(inode_ptr + inode * BLOCK_SIZE);
    int num_data_blocks = inodeblock_ptr->length;                               // number of data blocks in file

    // check if offset is within range of data blocks
    if(offset >= num_data_blocks)
        return 0;

    // check if length is too high and if so only read available data
    if(length > inodeblock_ptr->length)
        length = inodeblock_ptr->length;

    int it = offset;                                                            // iterates through data in data blocks
    int data_length_left = length;                                              // data left to read
    uint32_t i = it / BLOCK_SIZE;

    // read data while there is data left to read
    while(data_length_left > 0)
    {
        int read_blk_len;                                                       // amount of data there to read in that block
        int block_off = it % BLOCK_SIZE;

        // if this is first data block to read check for offset
        if(i == 0)
            read_blk_len = BLOCK_SIZE - block_off;
        else
            read_blk_len = BLOCK_SIZE;
        
        // if there is more data requested to read than there is actually present, truncate request
        if(read_blk_len > data_length_left)
            read_blk_len = data_length_left;

        // get data block number
        int data_block_num = inodeblock_ptr->data_blocks[i];

        // check validity of data block number
        if(data_block_num > bootblock_ptr->number_data_blocks)
            return -1;

        int c;
        // read data block and store into buf
        for(c = 0; c < read_blk_len; c++)
        {
            *((int8_t*) (buf + buf_it)) = *((int8_t*) (datablock_ptr + data_block_num * BLOCK_SIZE + c + block_off));
            buf_it++;
        }

        // updating data point tracker variables
        data_length_left-=read_blk_len;
        it+=read_blk_len;
        res+=read_blk_len;
        i++;
    }

    return res;
}

/* 
 * file_open
 *   DESCRIPTION: initialize temporary structures
 *   INPUTS: filename -- file name
 * 	 OUTPUTS: none
 *   SIDE EFFECTS: return 0
 */
int32_t file_open(const uint8_t* filename){
    // set file_dentry and define fbytes_read as 0
    read_dentry_by_name(filename, &file_dentry);
    fbytes_read = 0;
    return 0;
}

/* 
 * file_close
 *   DESCRIPTION: undo open function
 *   INPUTS: fd -- 
 * 	 OUTPUTS: none
 *   SIDE EFFECTS: return 0
 */
int32_t file_close(int32_t fd){
    return 0;
}

/* 
 * file_read
 *   DESCRIPTION: reads count bytes of data from file into buffer
 *   INPUTS: fd -- 
 *           buf -- buffer to copy into
 *           nbytes -- number of bytes of data to read
 * 	 OUTPUTS: none
 *   SIDE EFFECTS: return 0 -- success
 *                 return -1 -- failure 
 */
int32_t file_read(int32_t fd, void* buf, int32_t nbytes){
   uint32_t pcb_addr = EIGHT_MEGABYTE - EIGHT_KILOBYTES*(process_num + 1);
   pcb_t* program_pcb = (pcb_t *) pcb_addr;

   int32_t res = read_data(program_pcb->file_desc_array[fd].inode, fbytes_read, buf, nbytes);
   fbytes_read+=res;
   return res;
}

/* 
 * file_write
 *   DESCRIPTION: does nothing
 *   INPUTS: fd -- 
 *           buf -- 
 *           nbytes -- 
 * 	 OUTPUTS: none
 *   SIDE EFFECTS: return -1
 */
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}

int32_t directory_open(const uint8_t* filename){
    read_dentry_by_name(filename, &dir_dentry);
    dbytes_read = 0;
    return 0;
}

/* 
 * directory_close
 *   DESCRIPTION: probably does nothing
 *   INPUTS: fd -- 
 * 	 OUTPUTS: none
 *   SIDE EFFECTS: return 0
 */
int32_t directory_close(int32_t fd){
    return 0;
}

/* 
 * directory_read
 *   DESCRIPTION: read files filename by filename
 *   INPUTS: fd -- 
 *           buf -- 
 *           nbytes -- 
 * 	 OUTPUTS: 
 *   SIDE EFFECTS: return 0 -- success
 *                 return -1 -- failure 
 */
int32_t directory_read(int32_t fd, void* buf, int32_t nbytes){
    
    read_dentry_by_index(dbytes_read, &dir_dentry);
    strncpy((int8_t*)  buf, (int8_t*) dir_dentry.file_name, MAXIMUM_LENGTH_FILENAME);
    ((char*)buf)[MAXIMUM_LENGTH_FILENAME] = '\0';
    int res = strlen(buf);
    dbytes_read++;
    return res;
}

/* 
 * directory_write
 *   DESCRIPTION: should do nothing
 *   INPUTS: fd -- file descriptor for file to write 
 *           buf -- buffer to write data from
 *           nbytes --  how much data to write into file
 * 	 OUTPUTS: none
 *   SIDE EFFECTS: return -1
 */
int32_t directory_write(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}

/* 
 * test_dir_read
 *   DESCRIPTION: helper function to test directory read functionality
 *   INPUTS: NONE
 * 	 OUTPUTS: NONE
 *   SIDE EFFECTS: NONE
 */
void test_dir_read()
{
    printf("testing directory reading..\n");
    int32_t n = bootblock_ptr->number_dir_entries;
    directory_open((uint8_t*) ".");
    int i;

    for(i = 0; i < n; i++)
    {
        uint8_t buf[MAXIMUM_LENGTH_FILENAME];
        directory_read(0, (void*) buf, MAXIMUM_LENGTH_FILENAME);
        puts((int8_t* ) buf);
        putc('\n');
    }
}

/* 
 * test_file_read
 *   DESCRIPTION: helper function to test file read functionality
 *   INPUTS: file name -- name of file to read
 *           nbytes -- how many bytes to read from file
 * 	 OUTPUTS: NONE
 *   SIDE EFFECTS: NONE
 */
void test_file_read(const uint8_t* filename, int32_t nbytes)
{
    uint8_t buf[nbytes];
    printf("testing file reading..\n");
    file_open(filename);
    int res = file_read(0, (void*) buf, nbytes);
    if(res == -1)
        printf("error code -1\n");
    else
    {
        printf("%d bytes read\n", res);
        int i;
        for(i =0; i < res; i++)
            putc(buf[i]);
        putc('\n');
    }
}

