#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

#define BOOTBLOCK_RESERVED 52
#define BOOTBLOCK_SIZE 64   //6B
#define DIR_ENTRIES_SIZE 64
#define DIR_ENTRIES_RESERVED 24
#define BLOCK_SIZE 4096 //4kB
#define MAXIMUM_LENGTH_FILENAME 32

#include "types.h"

/*directory entry struct*/

    typedef struct dentry_t{
    uint8_t file_name[MAXIMUM_LENGTH_FILENAME];
    uint32_t file_type;
    uint32_t inode_index;
    uint8_t reserved_bytes[24];
    }dentry_t;

/*boot block struct*/
    typedef struct bootblock_t{
       uint32_t number_dir_entries;
       uint32_t number_inode;
       uint32_t number_data_blocks;
       uint8_t reserved[BOOTBLOCK_RESERVED];
       dentry_t dir_entries[DIR_ENTRIES_SIZE];

    }bootblock_t;


/*inode struct*/
    typedef struct inode_t{
        uint32_t length;
        uint32_t data_blocks[DIR_ENTRIES_RESERVED];

    }inode_t;

/*datablock struct*/
    typedef struct datablock_t{
        uint8_t file_data[BLOCK_SIZE];
    }datablock_t;


void fs_init();

// variables to keep track of position in file
bootblock_t* bootblock_ptr;
uint32_t inode_ptr;
uint32_t datablock_ptr;
uint32_t number_inode;
uint32_t number_data_blocks;
uint32_t number_dir;

// file operation functions
dentry_t file_dentry;
int32_t fbytes_read;
int32_t file_open(const uint8_t* filename);
int32_t file_close(int32_t fd);
int32_t file_read(int32_t fd, void* buf, int32_t nbytes);
int32_t file_write(int32_t fd, const void* buf, int32_t nbytes);

// directory operation functions
dentry_t dir_dentry;
int32_t dbytes_read;
int32_t directory_open(const uint8_t* filename);
int32_t directory_close(int32_t fd);
int32_t directory_read(int32_t fd, void* buf, int32_t nbytes);
int32_t directory_write(int32_t fd, const void* buf, int32_t nbytes);

// read data operations
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);


void test_dir_read();
void test_file_read(const uint8_t* filename, int32_t nbytes);
#endif
