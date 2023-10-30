#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "init.h"
#include <time.h>
#include "datetime.h"
#include "dev_io.h"

 unsigned short current_dir;   // 当前目录的节点号 */
 unsigned short last_alloc_dir_no;   // 当前分配到的目录项节点号 */
 unsigned short current_dirlen; // 当前路径长度 */
 short fopen_table[16]; // 文件打开表 */
 struct super_block sb_buf[1];	// 超级块缓冲区
 struct group_desc gd_buf[1];	// 组描述符缓冲区
 struct inode inode_workspace[1];  // inode缓冲区
 unsigned char bit_buf[512] = {0}; // 位图缓冲区
 unsigned char ibuf[512] = {0};
 struct dir_entry dir[DIR_LEN];   // 目录项缓冲区 64*16=1024
 char *Buffer;  // 针对数据块的缓冲区
 char tempbuf[4096];	// 文件写入缓冲区
 HashTable* dir_table; //哈希表存放目录项
 FILE *fp;	// 虚拟磁盘指针
char current_path[256];    // 当前路径名 */

// dir_to_hash
static void dir_to_hash(void) {
    for(int i = 0; i < DIR_LEN; i ++){
        if (dir[i].inode == -1){
            last_alloc_dir_no = i;
            break;
        } else{
            insert(dir_table, dir[i].name, dir[i].inode);
        }
    }
}

/*初始化inode信息*/
void inode_init(unsigned short file_name) {
    reload_inode_info(file_name);

    inode_workspace[0].i_size = 0;
    inode_workspace[0].i_blocks = 0;
    inode_workspace[0].i_mode = 0407;
    inode_workspace[0].i_ctime = time(NULL);
    inode_workspace[0].i_mtime = time(NULL);
    inode_workspace[0].i_atime = time(NULL);
    update_inode_info(file_name);
}

// 初始化磁盘
void initialize_disk(void) {
    int i = 0;

    printf("Creating the ext2 file system\n");
    printf("Please wait ");
    while (i < 1) {
        printf("... ");
        ++i;
    }
    printf("\n");
    last_alloc_dir_no = 0;
    for (i = 0; i < 16; i++) {
        fopen_table[i] = 0; // 清空缓冲表
    }
    if (fp != NULL) {
        fclose(fp);
    }
    fp = fopen("./lpuefs", "w+");      // 此文件大小是4612*512=2361344B，用此文件来模拟文件系统
    fseek(fp, DISK_START, SEEK_SET); // 将文件指针从0开始
    // 初始化超级块内容
    reload_fs_super_block();
    strcpy(sb_buf[0].sb_volume_name, VOLUME_NAME);
    sb_buf[0].sb_disk_size = DISK_SIZE;
    sb_buf[0].sb_blocks_per_group = BLOCKS_PER_GROUP;
    sb_buf[0].sb_size_per_block = BLOCK_SIZE;
    update_fs_super_block();

    // reset dir_entry
    clear_dir();
    reload_dir();
    strcpy(current_path, "[lpue_fs@ /"); // 修改路径名为根目录
    // 初始化组描述符内容
    reload_fs_group_desc();

    gd_buf[0].bg_block_bitmap = BLOCK_BITMAP;            // 第一个块位图的起始地址
    gd_buf[0].bg_inode_bitmap = INODE_BITMAP;            // 第一个inode位图的起始地址
    gd_buf[0].bg_inode_table = INODE_TABLE;              // inode表的起始地址
    gd_buf[0].bg_free_blocks_count = DATA_BLOCK_COUNTS;  // 空闲数据块数
    gd_buf[0].bg_free_inodes_count = INODE_TABLE_COUNTS; // 空闲inode数
    gd_buf[0].bg_used_dirs_count = 0;                    // 初始分配给目录的节点数是0
    update_fs_group_desc();                              // 更新组描述符内容

    reload_group_block_bitmap();
    reload_inode_bitmap();

    printf("The lpue file system has been installed!\n");
    check_disk();
    fclose(fp);
}

// 初始化内存
void initialize_lpuefs(void) {
    int i = 0;
    for (i = 0; i < 16; i++) {
        fopen_table[i] = 0;
    }
    strcpy(current_path, "[lpue_fs@ /");
    fp = fopen("./lpuefs", "r+");
    if (fp == NULL) {
        printf("The File system does not exist!\n");
        initialize_disk();
    }
    reload_fs_super_block();
    if (strcmp(sb_buf[0].sb_volume_name, VOLUME_NAME)) {
        printf("The File system [%s] is not suppoted yet!\n", sb_buf[0].sb_volume_name);
        printf("The File system loaded error!\n");
        initialize_disk();
        return;
    }
    dir_table = create_table();
    reload_dir();
    dir_to_hash();
    reload_fs_group_desc();
}


void test_inode(void)
{
    request_inode();
}

void rm_inode(unsigned short del_num)
{
    remove_inode(del_num);
}


void test_dir(void){
    for(int i = 0; i < 32; i ++){
        printf("dir[%d], dir[i].name = %s, dir[i].inode = %d \n", i, dir[i].name, dir[i].inode);
    }
    printf("\n");
}

// 检查磁盘状态
void check_disk(void)
{
    reload_fs_super_block();
    printf("volume name       : %s\n", sb_buf[0].sb_volume_name);
    printf("disk size         : %d(blocks)\n", sb_buf[0].sb_disk_size);
    printf("blocks per group  : %d(blocks)\n", sb_buf[0].sb_blocks_per_group);
    printf("ext2 file size    : %ld(kb)\n", sb_buf[0].sb_disk_size * sb_buf[0].sb_size_per_block / 1024);
    printf("block size        : %ld(kb)\n", sb_buf[0].sb_size_per_block);
    printf("free block        : %d\n", gd_buf[0].bg_free_blocks_count);
    printf("free inode        : %d\n", gd_buf[0].bg_free_inodes_count);
}
