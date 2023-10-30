#ifndef DEV_IO_H
#define DEV_IO_H

#include <stdio.h>
#include <string.h>
#include "init.h"

void update_fs_super_block(void);
void reload_fs_super_block(void);
void update_fs_group_desc(void);
void reload_fs_group_desc(void);
void update_inode_info(unsigned short i);
void reload_inode_info(unsigned short i);
void update_dir(void);
void clear_dir(void);
void reload_dir(void);
void update_group_block_bitmap(void);
void reload_group_block_bitmap(void);
void update_inode_bitmap(void);
void reload_inode_bitmap(void);
void update_block(unsigned short i);
void reload_block(unsigned short i);
int alloc_block(void);
int request_inode(void);
void remove_block(unsigned short del_num);
void remove_inode(unsigned short del_num);

#endif /* DEV_IO_H */