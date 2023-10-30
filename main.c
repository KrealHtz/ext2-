#include <stdio.h>
#include <string.h>
#include "main.h"
#include "init.h"
#include "file_ops.h"

int main(int argc,char **argv) {
    char command[10],temp[9];
    initialize_lpuefs();
    while(1) {
        printf("%s]#",current_path);
        scanf("%s",command);
        //printf("%s h\n",command);
        //�����ļ�
        if(!strcmp(command,"touch")) {
            scanf("%s",temp);
            create(temp,1);
        }
        //ɾ���ļ���Ŀ¼������ʾ
        else if(!strcmp(command,"rm")) {
            scanf("%s",temp);
            del(temp);
        }
        //��һ���ļ�
        else if(!strcmp(command,"open")) {
            scanf("%s",temp);
            open_file(temp);
        }
        //�ر�һ���ļ�
        else if(!strcmp(command,"close")) {
            scanf("%s",temp);
            close_file(temp);
        }
        //��һ���ļ�
        else if(!strcmp(command,"read")) {
            scanf("%s",temp);
            read_file(temp);
        }
        //дһ���ļ�
        else if(!strcmp(command,"write")) {
            scanf("%s\n",temp);
            write_file(temp);
        }
        //��ʾ��ǰĿ¼
        else if(!strcmp(command,"ls")) {
        	ls();
        }
        //��ʽ��Ӳ��
        else if(!strcmp(command,"format")) {
            char tempch;
            printf("Format will erase all the data in the Disk\n");
            printf("Are you sure?y/n:\n");
            fflush(stdin);
            scanf(" %c",&tempch);
            if(tempch=='Y'||tempch=='y') {
                format();
            } else {
            	printf("Format Disk canceled\n");
            }
        }
        //���Ӳ��
        else if(!strcmp(command,"ckdisk")) {
        	check_disk();
        }
        //�˳�ϵͳ
        else if(!strcmp(command,"quit")) {
        	break;
        }
        else if(!strcmp(command, "test")){
            for(int i = 0; i < 4096; i ++){
                test_inode();
            }
            printf("get innode test sucess!!\n");
        }
        else if(!strcmp(command, "testd")){
            test_dir();
        }
        else if(!strcmp(command, "testw")){
            int file_inode = 0;
            // create("test_file",1);
            file_inode = open_file("test_file");
            write_file_append(file_inode, "1111111", 8);
            write_file_append(file_inode, "2222222", 8);
            read_file("test_file");
            close_file("test_file");
        }

        else printf("No this Command,Please check!\n");
        getchar();
        //while((getchar())!='\n');
    }
    return 0;
}