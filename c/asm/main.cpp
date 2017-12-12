#include <stdio.h>
#include "test.h"
#include <linux/ioctl.h>
#include <linux/rtc.h>
#include <utils/Atomic.h>
#include <linux/android_alarm.h>

#include <sys/time.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <utils/SystemClock.h>
#include <utils/Timers.h>

#define LOG_TAG "Shawn"

#include <utils/Log.h>


int main(void)
{
    //S s = GetS(10);
 
    //s.Value3 = 2;
    int result;
    struct timespec ts;
    struct timeval tv;
    int64_t timestamp;
    
    struct tm _tm;  
    time_t timep;  

    while (1) {
        result = clock_gettime(CLOCK_BOOTTIME, &ts);
        if (result == 0) {
            timestamp = seconds_to_nanoseconds(ts.tv_sec) + ts.tv_nsec;
            // ALOGW("timestamp = %lld", timestamp);
            printf("timestamp = %lld\n", timestamp);
        }

        _tm.tm_sec = 0;  
        _tm.tm_min = _tm.tm_min - 1;  
        _tm.tm_hour = 21;  
        _tm.tm_mday = 29;  
        _tm.tm_mon = 3;  
        _tm.tm_year = 2014 - 1900;  

        timep = mktime(&_tm);  
        tv.tv_sec = timep;
        tv.tv_usec = 0;  

        settimeofday(&tv, NULL);

        sleep(1);
    }

    return 0;
}

