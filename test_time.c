#include <stdio.h>
#include <time.h>
#include <string.h>
char* getCurrentTime() {
    // 获取当前时间
    time_t currentTime;
    time(&currentTime);

    // 将时间转换为可打印的字符串格式
    char* timeString = ctime(&currentTime);

    char formattedTime[20];
    strftime(formattedTime, sizeof(formattedTime), "%Y-%m-%d %H:%M:%S", localtime(&currentTime));

    // 删除字符串末尾的换行符
    // timeString[strlen(timeString) - 1] = '\0';

    // 返回时间字符串
    return timeString;
}
int main() {
    
    // 自定义时间格式
    char formattedTime[20];


    char* currentTime = getCurrentTime();

    // 打印当前时间
    printf("Current time: %s\n", currentTime);
    // 打印格式化后的时间
    printf("Formatted time: %s\n", formattedTime);

    return 0;
}
