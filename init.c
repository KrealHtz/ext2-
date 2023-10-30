#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "init.h"
#include <time.h>
#include "datetime.h"
#include "dev_io.h"

 unsigned short current_dir;   // ��ǰĿ¼�Ľڵ�� */
 unsigned short last_alloc_dir_no;   // ��ǰ���䵽��Ŀ¼��ڵ�� */
 unsigned short current_dirlen; // ��ǰ·������ */
 short fopen_table[16]; // �ļ��򿪱� */
 struct super_block sb_buf[1];	// �����黺����
 struct group_desc gd_buf[1];	// ��������������
 struct inode inode_workspace[1];  // inode������
 unsigned char bit_buf[512] = {0}; // λͼ������
 unsigned char ibuf[512] = {0};
 struct dir_entry dir[DIR_LEN];   // Ŀ¼����� 64*16=1024
 char *Buffer;  // ������ݿ�Ļ�����
 char tempbuf[4096];	// �ļ�д�뻺����
 HashTable* dir_table; //��ϣ����Ŀ¼��
 FILE *fp;	// �������ָ��
char current_path[256];    // ��ǰ·���� */

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

/*��ʼ��inode��Ϣ*/
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

// ��ʼ������
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
        fopen_table[i] = 0; // ��ջ����
    }
    if (fp != NULL) {
        fclose(fp);
    }
    fp = fopen("./lpuefs", "w+");      // ���ļ���С��4612*512=2361344B���ô��ļ���ģ���ļ�ϵͳ
    fseek(fp, DISK_START, SEEK_SET); // ���ļ�ָ���0��ʼ
    // ��ʼ������������
    reload_fs_super_block();
    strcpy(sb_buf[0].sb_volume_name, VOLUME_NAME);
    sb_buf[0].sb_disk_size = DISK_SIZE;
    sb_buf[0].sb_blocks_per_group = BLOCKS_PER_GROUP;
    sb_buf[0].sb_size_per_block = BLOCK_SIZE;
    update_fs_super_block();

    // reset dir_entry
    clear_dir();
    reload_dir();
    strcpy(current_path, "[lpue_fs@ /"); // �޸�·����Ϊ��Ŀ¼
    // ��ʼ��������������
    reload_fs_group_desc();

    gd_buf[0].bg_block_bitmap = BLOCK_BITMAP;            // ��һ����λͼ����ʼ��ַ
    gd_buf[0].bg_inode_bitmap = INODE_BITMAP;            // ��һ��inodeλͼ����ʼ��ַ
    gd_buf[0].bg_inode_table = INODE_TABLE;              // inode�����ʼ��ַ
    gd_buf[0].bg_free_blocks_count = DATA_BLOCK_COUNTS;  // �������ݿ���
    gd_buf[0].bg_free_inodes_count = INODE_TABLE_COUNTS; // ����inode��
    gd_buf[0].bg_used_dirs_count = 0;                    // ��ʼ�����Ŀ¼�Ľڵ�����0
    update_fs_group_desc();                              // ����������������

    reload_group_block_bitmap();
    reload_inode_bitmap();

    printf("The lpue file system has been installed!\n");
    check_disk();
    fclose(fp);
}

// ��ʼ���ڴ�
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

// ������״̬
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
