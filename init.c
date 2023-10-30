#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "init.h"
#include <time.h>
#include "datetime.h"


// 写超级块
static void update_super_block(void) {
    fp = fopen("./lpuefs", "r+");
    fseek(fp, DISK_START, SEEK_SET);
    fwrite(sb_buf, SB_SIZE, 1, fp);
    fflush(fp); // 立刻将缓冲区的内容输出，保证磁盘内存数据的一致性
}

// 读超级块
static void reload_super_block(void) {
    fseek(fp, DISK_START, SEEK_SET); // 打开文件并文件开头开始写
    fread(sb_buf, SB_SIZE, 1, fp); // 读取内容到超级块缓冲区中
}

// 写组描述符
static void update_group_desc(void) {
    fp = fopen("./lpuefs", "r+");
    fseek(fp, GDT_START, SEEK_SET);
    fwrite(gd_buf, GD_SIZE, 1, fp);
    fflush(fp);
}

// 读组描述符
static void reload_group_desc(void) {
    fseek(fp, GDT_START, SEEK_SET);
    fread(gd_buf, GD_SIZE, 1, fp);
}

// 写第i个inode
static void update_inode_info(unsigned short i) {
    fp = fopen("./lpuefs", "r+");
    fseek(fp, INODE_TABLE + (i - 1) * INODE_SIZE, SEEK_SET);
    fwrite(inode_workspace, INODE_SIZE, 1, fp);
    fflush(fp);
}

// 读第i个inode
static void reload_inode_info(unsigned short i) {
    fseek(fp, INODE_TABLE + (i - 1) * INODE_SIZE, SEEK_SET);
    fread(inode_workspace, INODE_SIZE, 1, fp);
}

// 写目录项
static void update_dir(void) {
    fp = fopen("./lpuefs", "r+");
    fseek(fp, DIR_START, SEEK_SET);
    fwrite(dir, DIR_SIZE, 1, fp); //原先是读入一整块，修改后读入等于目录项缓冲区
    fflush(fp);
}

//清理目录项
static void clear_dir(void) {
    char dir_buf[1024];
    memset(dir_buf, -1, 1024);
    fp = fopen("./lpuefs", "r+");
    fseek(fp, DIR_START, SEEK_SET);
    fwrite(dir_buf, DIR_SIZE, 1, fp); 
    fflush(fp);
}

// 读目录项
static void reload_dir(void) {
    fseek(fp, DIR_START, SEEK_SET);
    fread(dir, DIR_SIZE, 1, fp);
    // fclose(fp);
}

// dir_to_hash
static void dir_to_hash(void) {
    for(int i = 0; i < 64; i ++){
        if (dir[i].inode == -1){
            last_alloc_dir_no = i;
            break;
        } else{
            insert(dir_table, dir[i].name, dir[i].inode);
        }
    }
}

// 写block位图
static void update_block_bitmap(void) {
    fp = fopen("./lpuefs", "r+");
    fseek(fp, BLOCK_BITMAP, SEEK_SET);
    fwrite(bit_buf, BITMAP_SIZE, 1, fp);
    fflush(fp);
}

// 读block位图
static void reload_block_bitmap(void) {
    fseek(fp, BLOCK_BITMAP, SEEK_SET);
    fread(bit_buf, BITMAP_SIZE, 1, fp);
}

// 写inode位图
static void update_inode_bitmap(void) {
    fp = fopen("./lpuefs", "r+");
    fseek(fp, INODE_BITMAP, SEEK_SET);
    fwrite(ibuf, BITMAP_SIZE, 1, fp);
    fflush(fp);
}

// 读inode位图
static void reload_inode_bitmap(void) {
    fseek(fp, INODE_BITMAP, SEEK_SET);
    fread(ibuf, BITMAP_SIZE, 1, fp);
}

// 写第i个数据块
static void update_block(unsigned short i) {
    fp = fopen("./lpuefs", "r+");
    fseek(fp, DATA_BLOCK + i * BLOCK_SIZE, SEEK_SET);
    // fseek(fp,0,SEEK_SET);
    fwrite(Buffer, BLOCK_SIZE, 1, fp);
    fflush(fp);
}

// 读第i个数据块
static void reload_block(unsigned short i) {
    fseek(fp, DATA_BLOCK + i * BLOCK_SIZE, SEEK_SET);
    fread(Buffer, BLOCK_SIZE, 1, fp);
}

// 分配一个数据块,返回数据块号
static int alloc_block(void) {
    int fist_free_block = 0;
    if (gd_buf[0].bg_free_blocks_count == 0)
    {
        printf("There is no block to be alloced!\n");
        return (0);
    }
    reload_block_bitmap();

    fist_free_block = find_first_free_bit(bit_buf, BITMAP_SIZE);
    printf("find first free block is %d \n", fist_free_block);
    set_bit(bit_buf, fist_free_block);

    update_block_bitmap();
    gd_buf[0].bg_free_blocks_count--;
    update_group_desc();
    return fist_free_block;
}

// 申请一个inode
static int request_inode(void) {
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
    update_group_desc();
    printf("alloced inode number is %d\n", first_free_inode);
    return first_free_inode;
}

// 当前目录中查找文件tmp，并得到该文件的 inode 号，失败返回-1
static int reserch_file(char file_name[12]) {
    int ret = 0;
    reload_dir();
    ret = lookup(dir_table, file_name);
    if (ret == -1) {
        printf("not find file %s\n", file_name);
    }
    return ret;
}

/*初始化inode信息*/
static void inode_init(unsigned short file_name) {
    reload_inode_info(file_name);

    inode_workspace[0].i_size = 0;
    inode_workspace[0].i_blocks = 0;
    inode_workspace[0].i_mode = 0407;
    inode_workspace[0].i_ctime = time(NULL);
    inode_workspace[0].i_mtime = time(NULL);
    inode_workspace[0].i_atime = time(NULL);
    update_inode_info(file_name);
}

// 删除一个块号
static void remove_block(unsigned short del_num) {
    reload_block_bitmap();

    clear_bit(bit_buf, del_num); // 位图置为0释放指定块

    update_block_bitmap();

    gd_buf[0].bg_free_blocks_count ++;
    update_group_desc();
}

// 删除一个inode 号
static void remove_inode(unsigned short del_num) {

    reload_inode_bitmap();

    clear_bit(ibuf, del_num); // 位图置为0释放指定的inode
    printf("rm %d inode sucess\n", del_num);

    update_inode_bitmap();

    gd_buf[0].bg_free_inodes_count++;
    update_group_desc();
}

// 在打开文件表中查找是否已打开文件
static unsigned short search_file(unsigned short Inode) {
    unsigned short fopen_table_point = 0;
    while (fopen_table_point < 16 && fopen_table[fopen_table_point++] != Inode)
        ;
    if (fopen_table_point == 16)
    {
        return 0;
    }
    return 1;
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
    reload_super_block();
    strcpy(sb_buf[0].sb_volume_name, VOLUME_NAME);
    sb_buf[0].sb_disk_size = DISK_SIZE;
    sb_buf[0].sb_blocks_per_group = BLOCKS_PER_GROUP;
    sb_buf[0].sb_size_per_block = BLOCK_SIZE;
    update_super_block();

    // reset dir_entry
    clear_dir();
    reload_dir();
    strcpy(current_path, "[lpue_fs@ /"); // 修改路径名为根目录
    // 初始化组描述符内容
    reload_group_desc();

    gd_buf[0].bg_block_bitmap = BLOCK_BITMAP;            // 第一个块位图的起始地址
    gd_buf[0].bg_inode_bitmap = INODE_BITMAP;            // 第一个inode位图的起始地址
    gd_buf[0].bg_inode_table = INODE_TABLE;              // inode表的起始地址
    gd_buf[0].bg_free_blocks_count = DATA_BLOCK_COUNTS;  // 空闲数据块数
    gd_buf[0].bg_free_inodes_count = INODE_TABLE_COUNTS; // 空闲inode数
    gd_buf[0].bg_used_dirs_count = 0;                    // 初始分配给目录的节点数是0
    update_group_desc();                              // 更新组描述符内容

    reload_block_bitmap();
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
    reload_super_block();
    if (strcmp(sb_buf[0].sb_volume_name, VOLUME_NAME)) {
        printf("The File system [%s] is not suppoted yet!\n", sb_buf[0].sb_volume_name);
        printf("The File system loaded error!\n");
        initialize_disk();
        return;
    }
    dir_table = create_table();
    reload_dir();
    dir_to_hash();
    reload_group_desc();
}

// 格式化文件系统
void format(void) {
    initialize_disk();
    initialize_lpuefs();
}

// 创建文件
void create(char file_name[12], int type) {
    unsigned short tmpno;
    // printf("reserch_file-%s, ret = %d", file_name, reserch_file(file_name));
    int ret = reserch_file(file_name);
    // printf("ret = %d\n",ret);
    if (ret == -1) {
        if (inode_workspace[0].i_size == 4096) {
            printf("Directory has no room to be alloced!\n");
            return;
        }
        reload_dir();
        tmpno = dir[last_alloc_dir_no].inode = request_inode(); // 分配一个新的inode项
        strcpy(dir[last_alloc_dir_no].name, file_name);
        last_alloc_dir_no ++;
        insert(dir_table, file_name, tmpno);
        update_dir();
        inode_init(tmpno);
    } else {
        printf("File has already existed!\n");
    }
}

// 删除文件
void del(char file_name[12]) {
    unsigned short m = 0;
    int del_fopen_no = 0; //要删除的文件在fopen_table中的位置
    int ret_inode = -1;
    ret_inode = reserch_file(file_name);
    if (ret_inode != -1) {
        // 若文件 file_name 已打开, 则将对应的 fopen_table 项清0
        while (fopen_table[del_fopen_no] != ret_inode && del_fopen_no < 16) {
            del_fopen_no ++;
        }
        if (del_fopen_no < 16) {
            fopen_table[del_fopen_no] = 0;
        }
        reload_inode_info(ret_inode); // 加载删除文件 inode
        // 删除文件对应的数据块
        while (m < inode_workspace[0].i_blocks) {
            remove_block(inode_workspace[0].i_block[m++]);
        }
        inode_workspace[0].i_blocks = 0;
        inode_workspace[0].i_size = 0;
        remove_inode(ret_inode);
        // 更新父目录
        reload_dir();
        for(m = 0; m < last_alloc_dir_no; m ++) {
            if (dir[m].inode == ret_inode) {
                break;
            }
        }
        // 将最后一项元素移动到删除项位置
        dir[m].inode = dir[last_alloc_dir_no - 1].inode;
        strcpy(dir[m].name, dir[last_alloc_dir_no - 1].name);
        // printf("m = %d, last_alloc_dir_no = %d\n", m, last_alloc_dir_no);
        last_alloc_dir_no --;

        update_dir();    
        // 从hashtable中移除该元素
        remove_node(dir_table, file_name);
    } else {
        printf("The file %s not exists!\n", file_name);
    }
}

// 打开文件
int open_file(char file_name[12]) {
    unsigned short flag, i, j, k;
    int ret_inode = reserch_file(file_name);
    if (ret_inode != -1) {
        if (search_file(ret_inode)) {
            printf("The file %s has opened!\n", file_name);
        } else {
            flag = 0;
            while (fopen_table[flag]) {
                flag ++;
            }
            fopen_table[flag] = ret_inode;
            // printf("File %s opened!\n", file_name);
            // printf("ret_inode = %d\n", ret_inode);
            return ret_inode;
        }
    } else {
        printf("The file %s does not exist!\n", file_name);
        return -1;
    }
}

// 关闭文件
void close_file(char file_name[12]) {
    unsigned short flag, i, j, k;
    int ret_inode = reserch_file(file_name);
    printf("ERRROR %d\n",__LINE__);
    if (ret_inode) {
        if (search_file(ret_inode)) {
            printf("ERRROR %d\n",__LINE__);

            flag = 0;
            while (fopen_table[flag] != ret_inode) {
                flag++;
            }
            printf("ERRROR %d\n",__LINE__);
            fopen_table[flag] = 0;
            printf("File %s closed!\n", file_name);
        } else {
            printf("The file %s has not been opened!\n", file_name);
        }
    } else {
        printf("The file %s does not exist!\n", file_name);
    }
}

// 读文件
void read_file(char file_name[12]) {
    unsigned short flag, i, j, k, t;
    int ret_inode = reserch_file(file_name);
    if (ret_inode != -1) {
        // 读文件的前提是该文件已经打开
        if (search_file(ret_inode)) {
            reload_inode_info(ret_inode);
            // 判断是否有读的权限
            // i_mode:111b:读,写,执行
            if (!(inode_workspace[0].i_mode & 4)) {
                printf("The file %s can not be read!\n", file_name);
                return;
            }
            for (flag = 0; flag < inode_workspace[0].i_blocks; flag++) {
                reload_block(inode_workspace[0].i_block[flag]);
                for (t = 0; t < inode_workspace[0].i_size - flag * 512; ++t) {
                    printf("%c", Buffer[t]);
                }
            }
            if (flag == 0) {
                printf("The file %s is empty!\n", file_name);
            } else {
                printf("\n");
            }
        } else {
            printf("The file %s has not been opened!\n", file_name);
        }
    } else
        printf("The file %s not exists!\n", file_name);
}

// 文件以覆盖方式写入
void write_file(char file_name[12]) // 写文件
{
    unsigned long j, size = 0, need_blocks, length;
    /*allocated memory ready for input*/
    Buffer = (char*)malloc(BLOCK_SIZE);
    memset(Buffer, 0, BLOCK_SIZE);

    int ret_inode = reserch_file(file_name);
    if (ret_inode != -1)
    {
        if (search_file(ret_inode))
        {
            reload_inode_info(ret_inode);
            // i_mode:111b:读,写,执行
            if (!(inode_workspace[0].i_mode & 2)) {
                printf("The file %s can not be writed!\n", file_name);
                return;
            }
            fflush(stdin);
            while (1) {
                tempbuf[size] = getchar();
                if (tempbuf[size] == '#') {
                    tempbuf[size] = '\0';
                    break;
                }
                if (size >= BLOCK_SIZE * 8) {
                    printf("Sorry,the max size of a file is 512MB!\n");
                    break;
                }
                size++;
            }
            if (size >= BLOCK_SIZE * 8) {
                length = BLOCK_SIZE * 8;
            } else {
                length = strlen(tempbuf);
            }
            // 计算需要的数据块数目
            need_blocks = length / BLOCK_SIZE;
            if (length % BLOCK_SIZE) {
                need_blocks++;
            }
            // 文件最大 8 个 blocks(512 bytes)
            if (need_blocks < 9) {
                // 分配文件所需块数目
                // 因为以覆盖写的方式写，要先判断原有的数据块数目
                if (inode_workspace[0].i_blocks <= need_blocks) {
                    while (inode_workspace[0].i_blocks < need_blocks)
                    {
                        inode_workspace[0].i_block[inode_workspace[0].i_blocks] = alloc_block();
                        inode_workspace[0].i_blocks++;
                    }
                }
                else {
                    while (inode_workspace[0].i_blocks > need_blocks) {
                        remove_block(inode_workspace[0].i_block[inode_workspace[0].i_blocks - 1]);
                        inode_workspace[0].i_blocks--;
                    }
                }
                j = 0;
                while (j < need_blocks) {
                    if (j != need_blocks - 1) {
                        reload_block(inode_workspace[0].i_block[j]);
                        memcpy(Buffer, tempbuf + j * BLOCK_SIZE, BLOCK_SIZE);
                        update_block(inode_workspace[0].i_block[j]);
                    }
                    else {
                        reload_block(inode_workspace[0].i_block[j]);
                        memcpy(Buffer, tempbuf + j * BLOCK_SIZE, length - j * BLOCK_SIZE);
                        inode_workspace[0].i_size = length;
                        update_block(inode_workspace[0].i_block[j]);
                    }
                    j++;
                }
                update_inode_info(ret_inode);
            } else {
                printf("Sorry,the max size of a file is 4KB!\n");
            }
        } else {
            printf("The file %s has not opened!\n", file_name);
        }
        free(Buffer);
    } else {
        printf("The file %s does not exist!\n", file_name);
    }
}
/*open 用于打开一个文件，返回inode节点号
  write 根据inode节点号去向该inode分配到的块中写入buf
*/
void write_file_append(int fd, const void *buf, unsigned long n) // 写文件
{
    unsigned short j, need_blocks;
    unsigned long block_offset;
    unsigned long offset = 0;
    Buffer = (char*)malloc(BLOCK_SIZE);

    if (search_file(fd)) {
        reload_inode_info(fd);
        // i_mode:111b:读,写,执行
        if (!(inode_workspace[0].i_mode & 2)) {
            printf("The file can not be writed!\n");
            return;
        }
        // 读取当前inode节点的偏移
        offset = inode_workspace[0].i_size;
        // 计算需要的数据块数目
        need_blocks = (n + offset) / BLOCK_SIZE;
        if(n % BLOCK_SIZE != 0) {
            need_blocks ++;
        }
        if (need_blocks == inode_workspace[0].i_blocks) {
            block_offset = 0;
            // 当前最后一块中还剩余的内存大小
            block_offset = offset % BLOCK_SIZE;
            reload_block(inode_workspace[0].i_block[inode_workspace[0].i_blocks]);
            memcpy(Buffer + block_offset, buf, n);
            update_block(inode_workspace[0].i_block[inode_workspace[0].i_blocks]);
            inode_workspace[0].i_size = n + offset;
        } else if (need_blocks > inode_workspace[0].i_blocks && need_blocks < 9) {
            // 申请数据块
            // 判断还需要增加多少数据块
            while (inode_workspace[0].i_blocks < need_blocks) {
                inode_workspace[0].i_block[inode_workspace[0].i_blocks] = alloc_block();
                inode_workspace[0].i_blocks ++;
            }
            block_offset = 0;
            // 当前最后一块中还剩余的内存大小
            // 读出未写之前的最后一块，然后将要写的内容拼接到一起往下写
            block_offset = offset % BLOCK_SIZE;
            reload_block(inode_workspace[0].i_block[inode_workspace[0].i_blocks]);
            memcpy(Buffer + block_offset, buf , BLOCK_SIZE - block_offset);
            update_block(inode_workspace[0].i_block[inode_workspace[0].i_blocks]);

            j = inode_workspace[0].i_blocks + 1;
            int t = 1;
            while (j < need_blocks) {
                if (j != need_blocks - 1) {
                    reload_block(inode_workspace[0].i_block[j]);
                    memcpy(Buffer, buf - block_offset + t * BLOCK_SIZE, BLOCK_SIZE);
                    update_block(inode_workspace[0].i_block[j]);
                } else {
                    reload_block(inode_workspace[0].i_block[j]);
                    memcpy(Buffer, buf - block_offset + t * BLOCK_SIZE, n - t * BLOCK_SIZE + block_offset);
                    inode_workspace[0].i_size = n + offset;
                    update_block(inode_workspace[0].i_block[j]);
                }
                j ++;
                t ++;
            }
        }
        else {
            printf("Sorry,the max size of a file is 512 MB!\n");
        }
        update_inode_info(fd);
        free(Buffer);
    } else {
        printf("The file has not opened!\n");
    }
}
  
// 查看目录下的内容
void ls(void) {
    printf("items          time           mode           size\n"); /* 15*4 */
    reload_dir();
    int index = 0;
    char formattedTime[13];
    for (int i = 0; i < last_alloc_dir_no; i++) {
        if (dir[i].inode >= 0) {
            printf("%s", dir[i].name);
        }
        index = strlen(dir[i].name);
        // printf("index len %d\n", index);
        reload_inode_info(dir[i].inode);
        while(index < 15) {
            printf(" ");
            index ++;
        }
        formatTime(inode_workspace[0].i_ctime, formattedTime, sizeof(formattedTime));
        printf("%s", formattedTime);
        index += 13;
        while(index < 32) {
            printf(" ");
            index ++;
        }
        switch (inode_workspace[0].i_mode & 7) {
            case 1:
                printf("____x");
                break;
            case 2:
                printf("__w__");
                break;
            case 3:
                printf("__w_x");
                break;
            case 4:
                printf("r____");
                break;
            case 5:
                printf("r___x");
                break;
            case 6:
                printf("r_w__");
                break;
            case 7:
                printf("r_w_x");
                break;
        }
        index += 5;
        while(index < 47) {
            printf(" ");
            index ++;
        }
        printf("%ldbytes \n", inode_workspace[0].i_size);
    }
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
    reload_super_block();
    printf("volume name       : %s\n", sb_buf[0].sb_volume_name);
    printf("disk size         : %d(blocks)\n", sb_buf[0].sb_disk_size);
    printf("blocks per group  : %d(blocks)\n", sb_buf[0].sb_blocks_per_group);
    printf("ext2 file size    : %ld(kb)\n", sb_buf[0].sb_disk_size * sb_buf[0].sb_size_per_block / 1024);
    printf("block size        : %ld(kb)\n", sb_buf[0].sb_size_per_block);
    printf("free block        : %d\n", gd_buf[0].bg_free_blocks_count);
    printf("free inode        : %d\n", gd_buf[0].bg_free_inodes_count);
}
