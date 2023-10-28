#ifndef _INIT_H
#define _INIT_H
#include "main.h"
#include "bitmap.h"
#include "hash_table.h"

#define VOLUME_NAME	"LPUE_FS"   // 磁盘卷名
#define BLOCK_SIZE (64 * 1024 * 1024)	       // 块大小

#define DISK_SIZE	4612	   //磁盘总块数

#define DISK_START 0	       // 磁盘开始地址
#define SB_SIZE	32	           //超级块大小是32B

#define	GD_SIZE	32	           // 块组描述符大小是32B
#define GDT_START	512    // 块组描述符起始地址

#define BLOCK_BITMAP (512 + 512) // 块位图起始地址
#define INODE_BITMAP (1024 + 512)// inode 位图起始地址
#define BITMAP_SIZE 512

#define INODE_TABLE (1536 + 512) // 索引节点表起始地址 4*512
#define INODE_SIZE 64	       // 每个inode的大小是64B
#define INODE_TABLE_COUNTS	4096 // inode entry number

//定义目录项
#define DIR_START           (2048 + 64 * 4096)         //目录项起始地址
#define DIR_SIZE            1024             //目录项大小

// Data Block 
#define DATA_BLOCK (64 * 1024 * 1024)	// 数据块起始地址 4*512+4096*64
#define DATA_BLOCK_COUNTS	4096	// 数据块数

#define BLOCKS_PER_GROUP	4097 // 每组中的块数

#define DIR_LEN 64               //目录项容量

struct super_block // 32 B
{
	char sb_volume_name[16]; //文件系统名
	unsigned short sb_disk_size; //磁盘总大小
	unsigned short sb_blocks_per_group; // 每组中的块数
	unsigned long int sb_size_per_block;	// 块大小
	char sb_pad[8];   //填充
};

struct group_desc // 32 B
{
    char bg_volume_name[16]; //文件系统名
    unsigned short bg_block_bitmap; //块位图的起始块号
    unsigned short bg_inode_bitmap; //索引结点位图的起始块号
    unsigned short bg_inode_table; //索引结点表的起始块号
    unsigned short bg_free_blocks_count; //本组空闲块的个数
    unsigned short bg_free_inodes_count; //本组空闲索引结点的个数
    unsigned short bg_used_dirs_count; //组中分配给目录的结点数
    char bg_pad[4]; //填充(0xff)
};
struct inode // 64 B
{
    unsigned short i_mode;   //文件类型及访问权限
    unsigned short i_blocks; //文件所占的数据块个数(0~7), 最大为7
    unsigned short i_uid;    //文件拥有者标识号
    unsigned short i_gid;    //文件的用户组标识符
    unsigned short i_links_count; //文件的硬链接计数
    unsigned short i_flags;  //打开文件的方式
    unsigned long i_size;    // 文件或目录大小(单位 byte)
    unsigned long i_atime;   //访问时间
    unsigned long i_ctime;   //创建时间
    unsigned long i_mtime;   //修改时间
    unsigned long i_dtime;   //删除时间
    unsigned short i_block[8]; //直接索引方式 指向数据块号
    char i_pad[24];           //填充(0xff)
};
struct dir_entry //16B
{
    int inode; //索引节点号
    char name[12]; //文件名
};


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

 void update_fs_super_block(void);   //更新超级块内容
 void reload_fs_super_block(void);   //加载超级块内容
 void update_fs_group_desc(void);    //更新组描述符内容
 void reload_fs_group_desc(void);    //加载组描述符内容
 void update_inode_info(unsigned short i); //更新indoe表
 void reload_inode_info(unsigned short i); //加载inode表
 void update_group_block_bitmap(void);  //更新块位图
 void reload_group_block_bitmap(void);  //加载块位图
 void update_inode_bitmap(void);  //更新inode位图
 void reload_inode_bitmap(void);  //加载inode位图
 void update_dir(void);//更新目录
 void reload_dir(void);//加载目录
 void update_block(unsigned short i);//更新数据块
 void reload_block(unsigned short i);//加载数据库
 int alloc_block(void);//分配数据块
 int request_inode(void); //得到inode节点
 int reserch_file(char file_name[12]);//查找文件
 void dir_prepare(unsigned short file_name);
 void remove_block(unsigned short del_num);//删除数据块
 void remove_inode(unsigned short del_num);//删除inode节点
 unsigned short search_file(unsigned short Ino);//在打开文件表中查找是否已打开文件
 void sleep(int k);
 void initialize_disk(void);//初始化磁盘


#endif // _INIT_H
