#include <stdio.h>
#include <time.h>
#include "datetime.h"

void formatTime(long long seconds, char* formattedTime, size_t bufferSize) {
    // 将秒数转换为 time_t 类型
    time_t rawTime = (time_t)seconds;

    // 转换为本地时间
    struct tm* localTime = localtime(&rawTime);

    // 格式化时间为 "月-日 小时:分钟" 的字符串
    strftime(formattedTime, bufferSize, "%m-%d %H:%M", localTime);
}
