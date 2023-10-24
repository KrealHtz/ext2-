#include <stdio.h>

int main() {
    // 打开日志文件
    FILE* logFile = fopen("debug.log", "w");

    if (logFile == NULL) {
        printf("无法打开日志文件\n");
        return 1;
    }

    // 调试日志
    fprintf(logFile, "这是一条调试日志\n");
    fprintf(logFile, "另一条调试日志\n");

    // 关闭日志文件
    fclose(logFile);

    return 0;
}