#ifndef _MAIN_H
#define _MAIN_H

extern char current_path[256];


extern void initialize_memory(void);
extern void format(void);
extern void create(char tmp[12],int type);
extern void del(char tmp[12]);
extern int open_file(char tmp[12]);
extern void close_file(char tmp[12]);
extern void read_file(char tmp[12]);
extern void write_file(char tmp[12]);
extern void ls(void);
extern void check_disk(void);
extern void test_inode(void);
extern void rm_inode(unsigned short del_num);
extern void test_dir(void);
extern void write_file_append(int fd, const void *buf, unsigned long n);

#endif
