#include <stdio.h>
#include <time.h>
unsigned int Time(unsigned int time_)
{   char *wday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    time_t t;
    struct tm *p;
    int year = 0, mon = 0, mday = 0;
    int hour = 0, min = 0, sec = 0;
    char *week = NULL;
    if (time_==0||time_==1){
        time(&t);
        p = localtime(&t);
    }//记录本地时间

    else{
        time(&t);
        t = time_;
        p = localtime(&t);
    }//发送时间

    year = 1900 + p->tm_year;
    mon = 1 + p->tm_mon;
    mday = p->tm_mday;
    week = wday[p->tm_wday];
    hour = p->tm_hour;
    min = p->tm_min;
    sec = p->tm_sec;
    unsigned int time__=time_;
    if(time__==0) {
        printf("发送时间:%d-%d-%d %s", year, mon, mday, week);
        printf(" %.2d:%.2d:%.2d\n\n", hour, min, sec);
    }
    else if(time__==1){
        printf("收到信息时间:%d-%d-%d %s", year, mon, mday, week);
        printf(" %.2d:%.2d:%.2d\n\n", hour, min, sec);
    }
    else{
        printf("对方发送的时间:%d-%d-%d %s", year, mon, mday, week);
        printf(" %.2d:%.2d:%.2d\n\n", hour, min, sec);
        }
    return t;
}
unsigned int Time_file(unsigned int time_,unsigned int receive,FILE*file)
{   char *wday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    time_t t;
    struct tm *p;
    int year = 0, mon = 0, mday = 0;
    int hour = 0, min = 0, sec = 0;
    char *week = NULL;

    time(&t);
    t = time_;
    p = localtime(&t);

    year = 1900 + p->tm_year;
    mon = 1 + p->tm_mon;
    mday = p->tm_mday;
    week = wday[p->tm_wday];
    hour = p->tm_hour;
    min = p->tm_min;
    sec = p->tm_sec;

    //一表示收到时间
    if(receive==1){
        fprintf(file,"收到信息时间:%d-%d-%d %s", year, mon, mday, week);
        fprintf(file," %.2d:%.2d:%.2d\n", hour, min, sec);
    }
    //0表示对方发出时间
    else{
        fprintf(file,"\n\n\n对方发送的时间:%d-%d-%d %s", year, mon, mday, week);
        fprintf(file," %.2d:%.2d:%.2d\n", hour, min, sec);
    }
    return t;
}