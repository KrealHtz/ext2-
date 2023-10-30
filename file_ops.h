#ifndef FILE_OP_H
#define FILE_OP_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "init.h"
#include "datetime.h"
#include "dev_io.h"

// 当前目录中查找文件tmp，并得到该文件的inode号，失败返回-1
int reserch_file(char file_name[12]);

// 在打开文件表中查找是否已打开文件
unsigned short search_file(unsigned short Inode);

// 格式化文件系统
void format(void);

// 创建文件
void create(char file_name[12], int type);

// 删除文件
void del(char file_name[12]);

// 打开文件
int open_file(char file_name[12]);

// 关闭文件
void close_file(char file_name[12]);

// 读文件
void read_file(char file_name[12]);

// 文件以覆盖方式写入
void write_file(char file_name[12]);

 // append 写文件
void write_file_append(int fd, const void *buf, unsigned long n);

// 查看目录下的内容
void ls(void);

#endif /* FILE_OP_H */