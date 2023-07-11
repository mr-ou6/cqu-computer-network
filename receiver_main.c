#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
# include "receive/receiver_mac.h"
# include "receive/receiver_ipv4.h"
#include "receive/receiver_udp.h"
#include"receive/receiver_oil.h"
int main(){
    unsigned char udp_new1[65536];
    unsigned int mac_begin=0;
    unsigned int udp_begin=0;
    unsigned char *oill;
    unsigned int i=0;
    while(1) {
        unsigned char *temp_ipv4_;
        unsigned char *temp_udp_new1;
        temp_ipv4_ = receive_frame(mac_begin);
        unsigned short temp_ipv4_length;
        memcpy(&temp_ipv4_length,&temp_ipv4_[2],2);//分片IP数据报的长度
        temp_udp_new1 = receive_ipv4(temp_ipv4_);
        if(udp_begin!=0){
            //第一个报文返回ip层的一些信息，需要删去前九位
            memcpy(&udp_new1[udp_begin],&temp_udp_new1[9],temp_ipv4_length-20);//将被分割的udp数据报组合起来。
        }
        if(udp_begin==0){
            //第一个报文返回ip层的一些信息，需要留下前九位
            memcpy(&udp_new1[udp_begin],&temp_udp_new1[0],temp_ipv4_length-11);//将被分割的udp数据报组合起来。
            udp_begin=9;
         }
        //后面的则不需要前九位
        unsigned short pan;
        memcpy(&pan,&temp_ipv4_[6],2);
        if (pan<0b1000000000000000){//000 010
            break;
        }//如果从mac层传回给网络层的IP数据报的标志位为分片时的000或不分片010停止。
        mac_begin+=1518;//一个mac帧最大为1518
        udp_begin=udp_begin+1480;
        i++;

    }
    if(i!=0) {
        printf("*****共收到%d个分片*****\n\n", i + 1);
    }
    oill=receive_udp(udp_new1);
    unsigned char *data;
    data=receive_oil(oill);

}
