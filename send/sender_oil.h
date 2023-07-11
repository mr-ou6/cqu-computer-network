#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <winsock.h>
#include "../common/china_time.h"
unsigned char *oil_construction(unsigned int source_account,unsigned int dest_account,unsigned int send_time,
                      unsigned int type,unsigned short size,unsigned char *data){
    unsigned char oil_[65528];
    memcpy(&oil_[0], &source_account ,4);
    memcpy(&oil_[4], &dest_account, 4);
    memcpy(&oil_[8], &send_time, 4);
    memcpy(&oil_[12], &type,4);
    memcpy(&oil_[16], &size,2);
    memcpy(&oil_[18], data, size);

    unsigned int t=Time(0); //发送的是现在距离是1970年1月1日的绝对时间。
    memcpy(&oil_[8], &t, 4);

    /* print frame information */
    printf("******Send oil information******\n");
    printf("source_account  :%d\n",source_account);
    printf("dest_account   :%d\n",dest_account);
    printf("type :%d\n",type);
    printf("send_time : %d\n",t);
    printf("size : %d\n",size);
    printf("data : %s\n",data);
    printf("**************************************\n\n");
    unsigned char * oil__=&oil_[0];//封装好的应用层报文
    return oil__;
}
