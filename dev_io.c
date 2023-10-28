#include "dev_io.h"

// 写超级块
void update_fs_super_block(void) {
    fp = fopen("./lpuefs", "r+");
    fseek(fp, DISK_START, SEEK_SET);
    fwrite(sb_buf, SB_SIZE, 1, fp);
    fflush(fp); // 立刻将缓冲区的内容输出，保证磁盘内存数据的一致性
}

// 读超级块
void reload_fs_super_block(void) {
    fseek(fp, DISK_START, SEEK_SET); // 打开文件并文件开头开始写
    fread(sb_buf, SB_SIZE, 1, fp); // 读取内容到超级块缓冲区中
}

// 写组描述符
void update_fs_group_desc(void) {
    fp = fopen("./lpuefs", "r+");
    fseek(fp, GDT_START, SEEK_SET);
    fwrite(gd_buf, GD_SIZE, 1, fp);
    fflush(fp);
}

// 读组描述符
void reload_fs_group_desc(void) {
    fseek(fp, GDT_START, SEEK_SET);
    fread(gd_buf, GD_SIZE, 1, fp);
}

// 写第i个inode
void update_inode_info(unsigned short i) {
    fp = fopen("./lpuefs", "r+");
    fseek(fp, INODE_TABLE + (i - 1) * INODE_SIZE, SEEK_SET);
    fwrite(inode_workspace, INODE_SIZE, 1, fp);
    fflush(fp);
}

// 读第i个inode
void reload_inode_info(unsigned short i) {
    fseek(fp, INODE_TABLE + (i - 1) * INODE_SIZE, SEEK_SET);
    fread(inode_workspace, INODE_SIZE, 1, fp);
}

// 写目录项
void update_dir(void) {
    fp = fopen("./lpuefs", "r+");
    fseek(fp, DIR_START, SEEK_SET);
    fwrite(dir, DIR_SIZE, 1, fp); //原先是读入一整块，修改后读入等于目录项缓冲区
    fflush(fp);
}

//清理目录项
void clear_dir(void) {
    char dir_buf[1024];
    memset(dir_buf, -1, 1024);
    fp = fopen("./lpuefs", "r+");
    fseek(fp, DIR_START, SEEK_SET);
    fwrite(dir_buf, DIR_SIZE, 1, fp); 
    fflush(fp);
}

// 读目录项
void reload_dir(void) {
    fseek(fp, DIR_START, SEEK_SET);
    fread(dir, DIR_SIZE, 1, fp);
    // fclose(fp);
}


// 写block位图
void update_group_block_bitmap(void) {
    fp = fopen("./lpuefs", "r+");
    fseek(fp, BLOCK_BITMAP, SEEK_SET);
    fwrite(bit_buf, BITMAP_SIZE, 1, fp);
    fflush(fp);
}

// 读block位图
void reload_group_block_bitmap(void) {
    fseek(fp, BLOCK_BITMAP, SEEK_SET);
    fread(bit_buf, BITMAP_SIZE, 1, fp);
}

// 写inode位图
void update_inode_bitmap(void) {
    fp = fopen("./lpuefs", "r+");
    fseek(fp, INODE_BITMAP, SEEK_SET);
    fwrite(ibuf, BITMAP_SIZE, 1, fp);
    fflush(fp);
}

// 读inode位图
void reload_inode_bitmap(void) {
    fseek(fp, INODE_BITMAP, SEEK_SET);
    fread(ibuf, BITMAP_SIZE, 1, fp);
}

// 写第i个数据块
void update_block(unsigned short i) {
    fp = fopen("./lpuefs", "r+");
    fseek(fp, DATA_BLOCK + i * BLOCK_SIZE, SEEK_SET);
    fwrite(Buffer, BLOCK_SIZE, 1, fp);
    fflush(fp);
}

// 读第i个数据块
void reload_block(unsigned short i) {
    fseek(fp, DATA_BLOCK + i * BLOCK_SIZE, SEEK_SET);
    fread(Buffer, BLOCK_SIZE, 1, fp);
}

// 分配一个数据块,返回数据块号
int alloc_block(void) {
    int fist_free_block = 0;
    if (gd_buf[0].bg_free_blocks_count == 0) {
        printf("There is no block to be alloced!\n");
        return (0);
    }
    reload_group_block_bitmap();

    fist_free_block = find_first_free_bit(bit_buf, BITMAP_SIZE);
    printf("find first free block is %d \n", fist_free_block);
    set_bit(bit_buf, fist_free_block);

    update_group_block_bitmap();
    gd_buf[0].bg_free_blocks_count--;
    update_fs_group_desc();
    return fist_free_block;
}

// 申请一个inode
int request_inode(void) {
    int first_free_inode = 0; 
    int flag = 0;
    if (gd_buf[0].bg_free_inodes_count == 0) {
        printf("There is no Inode to be alloced!\n");
        return 0;
    }
    reload_inode_bitmap();
    first_free_inode = find_first_free_bit(ibuf, BITMAP_SIZE);
    printf("find first free inode is %d \n", first_free_inode);
    set_bit(ibuf, first_free_inode);

    update_inode_bitmap();
    gd_buf[0].bg_free_inodes_count--;
    update_fs_group_desc();
    printf("alloced inode number is %d\n", first_free_inode);
    return first_free_inode;
}

// 删除一个块号
void remove_block(unsigned short del_num) {
    reload_group_block_bitmap();

    clear_bit(bit_buf, del_num); // 位图置为0释放指定块

    update_group_block_bitmap();

    gd_buf[0].bg_free_blocks_count ++;
    update_fs_group_desc();
}

// 删除一个inode 号
void remove_inode(unsigned short del_num) {

    reload_inode_bitmap();

    clear_bit(ibuf, del_num); // 位图置为0释放指定的inode
    printf("rm %d inode sucess\n", del_num);

    update_inode_bitmap();

    gd_buf[0].bg_free_inodes_count++;
    update_fs_group_desc();
}