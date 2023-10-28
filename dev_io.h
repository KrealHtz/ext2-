#ifndef DEV_IO_H
#define DEV_IO_H

#include <stdio.h>
#include <string.h>
#include "init.h"

extern unsigned short current_dir;   // 当前目录的节点号 */
extern unsigned short last_alloc_dir_no;   // 当前分配到的目录项节点号 */

extern unsigned short current_dirlen; // 当前路径长度 */

extern short fopen_table[16]; // 文件打开表 */

extern struct super_block sb_buf[1];	// 超级块缓冲区
extern struct group_desc gd_buf[1];	// 组描述符缓冲区
extern struct inode inode_workspace[1];  // inode缓冲区
extern unsigned char bit_buf[512] = {0}; // 位图缓冲区
extern unsigned char ibuf[512] = {0};
extern struct dir_entry dir[DIR_LEN];   // 目录项缓冲区 64*16=1024
extern char *Buffer;  // 针对数据块的缓冲区
extern char tempbuf[4096];	// 文件写入缓冲区
extern HashTable* dir_table; //哈希表存放目录项
extern FILE *fp;	// 虚拟磁盘指针

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