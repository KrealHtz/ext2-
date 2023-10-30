#!/bin/bash

# 清理函数
clean() {
    # 删除可执行文件
    rm -f lpue_fs
    rm -f lpuefs
}

# 编译函数
compile() {
    # 编译main.c文件
    gcc -c main.c -o main.o

    # 编译init.c文件
    gcc -c init.c -o init.o

    gcc -c bitmap.c -o bitmap.o

    gcc -c hash_table.c -o hash_table.o

    gcc -c datetime.c -o datetime.o

    gcc -c dev_io.c -o dev_io.o

    gcc -c file_ops.c -o file_ops.o

    # 将main.o和init.o链接为可执行文件
    gcc main.o init.o bitmap.o hash_table.o datetime.o file_ops.o dev_io.o -o lpue_fs

    # 清理中间文件
    rm main.o init.o bitmap.o hash_table.o datetime.o dev_io.o file_ops.o
}

# 根据参数执行相应操作
if [ "$1" == "c" ]; then
    clean
    echo "清理完成"
else
    compile
    echo "编译完成"
fi