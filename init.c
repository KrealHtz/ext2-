#include <stdio.h>
#include <string.h>
#include "init.h"


// д������
static void update_super_block(void) {
    fp = fopen("./Ext2", "r+");
    fseek(fp, DISK_START, SEEK_SET);
    fwrite(sb_block, SB_SIZE, 1, fp);
    fflush(fp); // ���̽��������������������֤�����ڴ����ݵ�һ����
}

// ��������
static void reload_super_block(void) {
    fseek(fp, DISK_START, SEEK_SET); // ���ļ����ļ���ͷ��ʼд
    fread(sb_block, SB_SIZE, 1, fp); // ��ȡ���ݵ������黺������
}

// д��������
static void update_group_desc(void) {
    fp = fopen("./Ext2", "r+");
    fseek(fp, GDT_START, SEEK_SET);
    fwrite(gdt, GD_SIZE, 1, fp);
    fflush(fp);
}

// ����������
static void reload_group_desc(void) {

    fseek(fp, GDT_START, SEEK_SET);
    fread(gdt, GD_SIZE, 1, fp);
}

// д��i��inode
static void update_inode_entry(unsigned short i) {
    fp = fopen("./Ext2", "r+");
    fseek(fp, INODE_TABLE + (i - 1) * INODE_SIZE, SEEK_SET);
    fwrite(inode_area, INODE_SIZE, 1, fp);
    fflush(fp);
}

// ����i��inode
static void reload_inode_entry(unsigned short i) {
    fseek(fp, INODE_TABLE + (i - 1) * INODE_SIZE, SEEK_SET);
    fread(inode_area, INODE_SIZE, 1, fp);
}

// дĿ¼��
static void update_dir(void) {
    fp = fopen("./Ext2", "r+");
    fseek(fp, DIR_START, SEEK_SET);
    fwrite(dir, DIR_SIZE, 1, fp); //ԭ���Ƕ���һ���飬�޸ĺ�������Ŀ¼�����
    fflush(fp);
}

//����Ŀ¼��
static void clear_dir(void) {
    char dir_buf[1024];
    memset(dir_buf, 0, 1024);
    fp = fopen("./Ext2", "r+");
    fseek(fp, DIR_START, SEEK_SET);
    fwrite(dir_buf, DIR_SIZE, 1, fp); //ԭ���Ƕ���һ���飬�޸ĺ�������Ŀ¼�����
    fflush(fp);
}

// ��Ŀ¼��
static void reload_dir(void) {
    fseek(fp, DIR_START, SEEK_SET);
    fread(dir, DIR_SIZE, 1, fp);
    // fclose(fp);
}

// dir_to_hash
static void dir_to_hash(void) {
    for(int i = 0; i < 64; i ++){
        if (dir[i].inode == 0){
            last_alloc_dir_no = i;
            break;
        }
        else{
            insert(dir_table, dir[i].name, dir[i].inode);
        }
    }
}

// дblockλͼ
static void update_block_bitmap(void) {
    fp = fopen("./Ext2", "r+");
    fseek(fp, BLOCK_BITMAP, SEEK_SET);
    fwrite(bitbuf, BITMAP_SIZE, 1, fp);
    fflush(fp);
}

// ��blockλͼ
static void reload_block_bitmap(void) {
    fseek(fp, BLOCK_BITMAP, SEEK_SET);
    fread(bitbuf, BITMAP_SIZE, 1, fp);
}

// дinodeλͼ
static void update_inode_bitmap(void) {
    fp = fopen("./Ext2", "r+");
    fseek(fp, INODE_BITMAP, SEEK_SET);
    fwrite(ibuf, BITMAP_SIZE, 1, fp);
    fflush(fp);
}

// ��inodeλͼ
static void reload_inode_bitmap(void) {
    fseek(fp, INODE_BITMAP, SEEK_SET);
    fread(ibuf, BITMAP_SIZE, 1, fp);
}

// д��i�����ݿ�
static void update_block(unsigned short i) {
    fp = fopen("./Ext2", "r+");
    fseek(fp, DATA_BLOCK + i * BLOCK_SIZE, SEEK_SET);
    // fseek(fp,0,SEEK_SET);
    fwrite(Buffer, BLOCK_SIZE, 1, fp);
    fflush(fp);
}

// ����i�����ݿ�
static void reload_block(unsigned short i) {
    fseek(fp, DATA_BLOCK + i * BLOCK_SIZE, SEEK_SET);
    fread(Buffer, BLOCK_SIZE, 1, fp);
}

// ����һ�����ݿ�,�������ݿ��
static int alloc_block(void) {

    unsigned short cur = last_alloc_block;
    int fist_free_block = 0;
    if (gdt[0].bg_free_blocks_count == 0)
    {
        printf("There is no block to be alloced!\n");
        return (0);
    }
    reload_block_bitmap();

    fist_free_block = find_first_free_bit(bitbuf, BITMAP_SIZE);
    printf("find first free block is %d \n", fist_free_block);
    set_bit(bitbuf, fist_free_block);

    last_alloc_inode = fist_free_block;

    update_block_bitmap();
    gdt[0].bg_free_blocks_count--;
    update_group_desc();
    return last_alloc_block;
}

// ����һ��inode
static int get_inode(void) {
    unsigned short cur = last_alloc_inode;
    unsigned char con = 128;
    int first_free_inode = 0; // only use tet
    int flag = 0;
    if (gdt[0].bg_free_inodes_count == 0)
    {
        printf("There is no Inode to be alloced!\n");
        return 0;
    }
    reload_inode_bitmap();
    first_free_inode = find_first_free_bit(ibuf, BITMAP_SIZE);
    printf("find first free inode is %d \n", first_free_inode);
    set_bit(ibuf, first_free_inode);

    last_alloc_inode = first_free_inode;
    update_inode_bitmap();
    gdt[0].bg_free_inodes_count--;
    update_group_desc();
    printf("alloced inode number is %d\n", last_alloc_inode);
    return last_alloc_inode;
}

// ��ǰĿ¼�в����ļ�tmp�����õ����ļ��� inode �ţ�ʧ�ܷ���-1
static int reserch_file(char tmp[12])
{
    int ret = 0;
    reload_dir();
    ret = lookup(dir_table, tmp);
    if (ret == -1) {
        printf("not find file %s\n", tmp);
    }
    return ret;
}

/*Ϊ����Ŀ¼���ļ����� dir_entry
���������ļ���ֻ�����һ��inode��
��������Ŀ¼������inode���⣬����Ҫ�����������洢.��..����Ŀ¼��*/
static void dir_prepare(unsigned short tmp)
{
    reload_inode_entry(tmp);

    inode_area[0].i_size = 0;
    inode_area[0].i_blocks = 0;
    inode_area[0].i_mode = 0407;
    update_inode_entry(tmp);
}

// ɾ��һ�����
static void remove_block(unsigned short del_num) {
    reload_block_bitmap();

    clear_bit(bitbuf, del_num); // λͼ��Ϊ0�ͷ�ָ����

    update_block_bitmap();

    gdt[0].bg_free_blocks_count++;
    update_group_desc();
}

// ɾ��һ��inode ��
static void remove_inode(unsigned short del_num) {

    reload_inode_bitmap();

    clear_bit(ibuf, del_num); // λͼ��Ϊ0�ͷ�ָ����inode
    printf("rm %d inode sucess\n", del_num);

    update_inode_bitmap();

    gdt[0].bg_free_inodes_count++;
    update_group_desc();
}

// �ڴ��ļ����в����Ƿ��Ѵ��ļ�
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

// ��ʼ������
void initialize_disk(void) {
    int i = 0;

    printf("Creating the ext2 file system\n");
    printf("Please wait ");
    while (i < 1)
    {
        printf("... ");
        ++i;
    }
    printf("\n");
    last_alloc_inode = 1;
    last_alloc_block = 0;
    last_alloc_dir_no = 0;
    for (i = 0; i < 16; i++)
    {
        fopen_table[i] = 0; // ��ջ����
    }

    if (fp != NULL)
    {
        fclose(fp);
    }
    fp = fopen("./Ext2", "w+");      // ���ļ���С��4612*512=2361344B���ô��ļ���ģ���ļ�ϵͳ
    fseek(fp, DISK_START, SEEK_SET); // ���ļ�ָ���0��ʼ
    // ��ʼ������������
    reload_super_block();
    strcpy(sb_block[0].sb_volume_name, VOLUME_NAME);
    sb_block[0].sb_disk_size = DISK_SIZE;
    sb_block[0].sb_blocks_per_group = BLOCKS_PER_GROUP;
    sb_block[0].sb_size_per_block = BLOCK_SIZE;
    update_super_block();

    // ��Ŀ¼��inode��Ϊ1
    // reload_inode_entry(1);

    // read dir form disk ȱ�����Ŀ¼��Ĳ���
    // reset dir_entry
    clear_dir();
    reload_dir();
    
    strcpy(current_path, "[root@ /"); // �޸�·����Ϊ��Ŀ¼
    // ��ʼ��������������
    reload_group_desc();

    gdt[0].bg_block_bitmap = BLOCK_BITMAP;            // ��һ����λͼ����ʼ��ַ
    gdt[0].bg_inode_bitmap = INODE_BITMAP;            // ��һ��inodeλͼ����ʼ��ַ
    gdt[0].bg_inode_table = INODE_TABLE;              // inode�����ʼ��ַ
    gdt[0].bg_free_blocks_count = DATA_BLOCK_COUNTS;  // �������ݿ���
    gdt[0].bg_free_inodes_count = INODE_TABLE_COUNTS; // ����inode��
    gdt[0].bg_used_dirs_count = 0;                    // ��ʼ�����Ŀ¼�Ľڵ�����0
    update_group_desc();                              // ����������������

    reload_block_bitmap();
    reload_inode_bitmap();

    printf("The ext2 file system has been installed!\n");
    check_disk();
    fclose(fp);
}

// ��ʼ���ڴ�
void initialize_memory(void) {
    int i = 0;
    last_alloc_inode = 1;
    last_alloc_block = 0;
    for (i = 0; i < 16; i++)
    {
        fopen_table[i] = 0;
    }
    strcpy(current_path, "[root@ /");
    // current_dir = 1;
    fp = fopen("./Ext2", "r+");
    if (fp == NULL)
    {
        printf("The File system does not exist!\n");
        initialize_disk();
        return;
    }
    reload_super_block();
    if (strcmp(sb_block[0].sb_volume_name, VOLUME_NAME))
    {
        printf("The File system [%s] is not suppoted yet!\n", sb_block[0].sb_volume_name);
        printf("The File system loaded error!\n");
        initialize_disk();
        return;
    }
    dir_table = create_table();
    reload_dir();
    dir_to_hash();

    reload_group_desc();
}

// ��ʽ��
void format(void) {
    initialize_disk();
    initialize_memory();
}

// �����ļ�
// ȱ�������ж� TODO
void create(char tmp[12], int type) {
    unsigned short tmpno;
    // reload_inode_entry(current_dir); //��ȡ��ǰĿ¼��inode
    // printf("reserch_file-%s, ret = %d", tmp, reserch_file(tmp));
    int ret = reserch_file(tmp);
    // printf("ret = %d\n",ret);
    if (ret == -1) {
        if (inode_area[0].i_size == 4096) {
            printf("Directory has no room to be alloced!\n");
            return;
        }
        reload_dir();
        tmpno = dir[last_alloc_dir_no].inode = get_inode(); // ����һ���µ�inode��
        strcpy(dir[last_alloc_dir_no].name, tmp);
        last_alloc_dir_no ++;
        insert(dir_table, tmp, tmpno);
        update_dir();
        dir_prepare(tmpno);
    }    
    else {
        printf("File has already existed!\n");
    }
}

// ɾ���ļ�
void del(char tmp[12]) {
    unsigned short i, j, k, m, n, flag;
    int del_fopen_no = 0; //Ҫɾ�����ļ���fopen_table�е�λ��
    int ret_inode;
    m = 0;
    ret_inode = reserch_file(tmp);
    if (ret_inode != -1) {
        // ���ļ� tmp �Ѵ�, �򽫶�Ӧ�� fopen_table ����0
        while (fopen_table[del_fopen_no] != ret_inode && del_fopen_no < 16) {
            del_fopen_no ++;
        }
        if (del_fopen_no < 16) {
            fopen_table[del_fopen_no] = 0;
        }
        reload_inode_entry(ret_inode); // ����ɾ���ļ� inode
        // ɾ���ļ���Ӧ�����ݿ�
        while (m < inode_area[0].i_blocks) {
            remove_block(inode_area[0].i_block[m++]);
        }
        inode_area[0].i_blocks = 0;
        inode_area[0].i_size = 0;
        remove_inode(ret_inode);
        // ���¸�Ŀ¼
        reload_dir();
        for(m = 0; m < last_alloc_dir_no; m ++) {
            if (dir[m].inode == ret_inode) {
                break;
            }
        }
        for(m; m < last_alloc_block; m ++) {
            dir[m].inode = dir[m + 1].inode;
            strcpy(dir[m].name, dir[m + 1].name);
        }
        last_alloc_dir_no --;

        update_dir();    

        remove_node(dir_table, tmp);
    }
    else {
        printf("The file %s not exists!\n", tmp);
    }
}

// ���ļ�
void open_file(char tmp[12]) {
    unsigned short flag, i, j, k;
    int ret_inode = reserch_file(tmp);
    if (ret_inode != -1) {
        if (search_file(ret_inode)) {
            printf("The file %s has opened!\n", tmp);
        }
        else {
            flag = 0;
            while (fopen_table[flag]) {
                flag ++;
            }
            fopen_table[flag] = ret_inode;
            printf("File %s opened!\n", tmp);
        }
    }
    else
        printf("The file %s does not exist!\n", tmp);
}

// �ر��ļ�
void close_file(char tmp[12]) {
    unsigned short flag, i, j, k;
    int ret_inode = reserch_file(tmp);
    if (ret_inode) {
        if (search_file(ret_inode)) {
            flag = 0;
            while (fopen_table[flag] != dir[k].inode) {
                flag++;
            }
            fopen_table[flag] = 0;
            printf("File %s closed!\n", tmp);
        }
        else {
            printf("The file %s has not been opened!\n", tmp);
        }
    }
    else {
        printf("The file %s does not exist!\n", tmp);
    }
}

// ���ļ�
void read_file(char tmp[12]) {
    unsigned short flag, i, j, k, t;
    int ret_inode = reserch_file(tmp);
    if (ret_inode != -1) {
        // ���ļ���ǰ���Ǹ��ļ��Ѿ���
        if (search_file(ret_inode)) {
            reload_inode_entry(ret_inode);
            // �ж��Ƿ��ж���Ȩ��
            // i_mode:111b:��,д,ִ��
            if (!(inode_area[0].i_mode & 4)) {
                printf("The file %s can not be read!\n", tmp);
                return;
            }
            for (flag = 0; flag < inode_area[0].i_blocks; flag++) {
                reload_block(inode_area[0].i_block[flag]);
                for (t = 0; t < inode_area[0].i_size - flag * 512; ++t) {
                    printf("%c", Buffer[t]);
                }
            }
            if (flag == 0) {
                printf("The file %s is empty!\n", tmp);
            }
            else {
                printf("\n");
            }
        }
        else {
            printf("The file %s has not been opened!\n", tmp);
        }
    }
    else
        printf("The file %s not exists!\n", tmp);
}

// �ļ��Ը��Ƿ�ʽд��
void write_file(char tmp[12]) // д�ļ�
{
    unsigned short flag, i, j, k, size = 0, need_blocks, length;
    int ret_inode = reserch_file(tmp);
    if (flag)
    {
        if (search_file(ret_inode))
        {
            reload_inode_entry(ret_inode);
            // i_mode:111b:��,д,ִ��
            if (!(inode_area[0].i_mode & 2)) {
                printf("The file %s can not be writed!\n", tmp);
                return;
            }
            fflush(stdin);
            while (1) {
                tempbuf[size] = getchar();
                if (tempbuf[size] == '#') {
                    tempbuf[size] = '\0';
                    break;
                }
                if (size >= 4095) {
                    printf("Sorry,the max size of a file is 4KB!\n");
                    break;
                }
                size++;
            }
            if (size >= 4095) {
                length = 4096;
            }
            else {
                length = strlen(tempbuf);
            }
            // ������Ҫ�����ݿ���Ŀ
            need_blocks = length / 512;
            if (length % 512) {
                need_blocks++;
            }
            // �ļ���� 8 �� blocks(512 bytes)
            if (need_blocks < 9) {
                // �����ļ��������Ŀ
                // ��Ϊ�Ը���д�ķ�ʽд��Ҫ���ж�ԭ�е����ݿ���Ŀ
                if (inode_area[0].i_blocks <= need_blocks) {
                    while (inode_area[0].i_blocks < need_blocks)
                    {
                        inode_area[0].i_block[inode_area[0].i_blocks] = alloc_block();
                        inode_area[0].i_blocks++;
                    }
                }
                else {
                    while (inode_area[0].i_blocks > need_blocks) {
                        remove_block(inode_area[0].i_block[inode_area[0].i_blocks - 1]);
                        inode_area[0].i_blocks--;
                    }
                }
                j = 0;
                while (j < need_blocks) {
                    if (j != need_blocks - 1) {
                        reload_block(inode_area[0].i_block[j]);
                        memcpy(Buffer, tempbuf + j * BLOCK_SIZE, BLOCK_SIZE);
                        update_block(inode_area[0].i_block[j]);
                    }
                    else {
                        reload_block(inode_area[0].i_block[j]);
                        memcpy(Buffer, tempbuf + j * BLOCK_SIZE, length - j * BLOCK_SIZE);
                        inode_area[0].i_size = length;
                        update_block(inode_area[0].i_block[j]);
                    }
                    j++;
                }
                update_inode_entry(dir[k].inode);
            }
            else {
                printf("Sorry,the max size of a file is 4KB!\n");
            }
        }
        else {
            printf("The file %s has not opened!\n", tmp);
        }
    }
    else {
        printf("The file %s does not exist!\n", tmp);
    }
}

// �ļ������д��ʽд��
void write_file1(char tmp[12]) // д�ļ�
{
    unsigned short flag, i, j, k, size = 0, need_blocks, length;
    int ret_inode = reserch_file(tmp);
    if (flag)
    {
        if (search_file(ret_inode))
        {
            reload_inode_entry(ret_inode);
            // i_mode:111b:��,д,ִ��
            if (!(inode_area[0].i_mode & 2)) {
                printf("The file %s can not be writed!\n", tmp);
                return;
            }
            fflush(stdin);
            while (1) {
                tempbuf[size] = getchar();
                if (tempbuf[size] == '#') {
                    tempbuf[size] = '\0';
                    break;
                }
                if (size >= 4095) {
                    printf("Sorry,the max size of a file is 4KB!\n");
                    break;
                }
                size++;
            }
            if (size >= 4095) {
                length = 4096;
            }
            else {
                length = strlen(tempbuf);
            }
            // ������Ҫ�����ݿ���Ŀ
            need_blocks = length / 512;
            if (length % 512) {
                need_blocks++;
            }
            // �ļ���� 8 �� blocks(512 bytes)
            if (need_blocks < 9) {
                // �����ļ��������Ŀ
                // ��Ϊ�Ը���д�ķ�ʽд��Ҫ���ж�ԭ�е����ݿ���Ŀ
                if (inode_area[0].i_blocks <= need_blocks) {
                    while (inode_area[0].i_blocks < need_blocks)
                    {
                        inode_area[0].i_block[inode_area[0].i_blocks] = alloc_block();
                        inode_area[0].i_blocks++;
                    }
                }
                else {
                    while (inode_area[0].i_blocks > need_blocks) {
                        remove_block(inode_area[0].i_block[inode_area[0].i_blocks - 1]);
                        inode_area[0].i_blocks--;
                    }
                }
                j = 0;
                while (j < need_blocks) {
                    if (j != need_blocks - 1) {
                        reload_block(inode_area[0].i_block[j]);
                        memcpy(Buffer, tempbuf + j * BLOCK_SIZE, BLOCK_SIZE);
                        update_block(inode_area[0].i_block[j]);
                    }
                    else {
                        reload_block(inode_area[0].i_block[j]);
                        memcpy(Buffer, tempbuf + j * BLOCK_SIZE, length - j * BLOCK_SIZE);
                        inode_area[0].i_size = length;
                        update_block(inode_area[0].i_block[j]);
                    }
                    j++;
                }
                update_inode_entry(dir[k].inode);
            }
            else {
                printf("Sorry,the max size of a file is 4KB!\n");
            }
        }
        else {
            printf("The file %s has not opened!\n", tmp);
        }
    }
    else {
        printf("The file %s does not exist!\n", tmp);
    }
}

// �鿴Ŀ¼�µ�����
void ls(void) {
    printf("items          time           mode           size\n"); /* 15*4 */
    reload_dir();
    for (int i = 0; i < last_alloc_dir_no; i++) {
        if (dir[i].inode) {
            printf("%s\n", dir[i].name);
        }
    }
}

void test_inode(void)
{
    get_inode();
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
    reload_super_block();
    printf("volume name       : %s\n", sb_block[0].sb_volume_name);
    printf("disk size         : %d(blocks)\n", sb_block[0].sb_disk_size);
    printf("blocks per group  : %d(blocks)\n", sb_block[0].sb_blocks_per_group);
    printf("ext2 file size    : %ld(kb)\n", sb_block[0].sb_disk_size * sb_block[0].sb_size_per_block / 1024);
    printf("block size        : %ld(kb)\n", sb_block[0].sb_size_per_block);
}
