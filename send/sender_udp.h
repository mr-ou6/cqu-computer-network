#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <winsock.h>
/**
 * @param frame
 * @param source_port
 * @param dest_port
 * @param payload
 * @param udp_length
 * @param check_sum_udp
 * @return
 */

//增加校验和
unsigned short get_udp_checksum(unsigned short*buffer1, int size1)
{
    unsigned long long cksum = 0;
    //udp数据报
    while(size1>1)
    {   cksum += *buffer1++;
        size1 -= sizeof(unsigned short);
    }
    //最后的奇数字节//补全
    if(size1)
    {
        cksum += *(unsigned char*)buffer1;
    }
    cksum = (cksum>>16) + (cksum&0xffff);  //将高16bit与低16bit相加
    cksum += (cksum>>16);

    return (unsigned short)(~cksum); //最后将结果取反,得到checksum
}
unsigned char *udp_construction(unsigned char *frame, unsigned short source_port, unsigned short dest_port,
                       unsigned short udp_length, unsigned short check_sum_udp,
                       unsigned char protocol_in_ip_header,unsigned char *source_ip_address,unsigned char *dest_ip_address){
    /*fake_header*/
    unsigned char udp_[65536];
    unsigned char fake_head[12];
    unsigned char zero=0X00;
    memcpy(&fake_head[0],source_ip_address,4);
    memcpy(&fake_head[4],dest_ip_address,4);
    memcpy(&fake_head[8],&zero,1);
    memcpy(&fake_head[9],&protocol_in_ip_header,1);
    memcpy(&fake_head[10],&udp_length,2);

    /* package udp*/
    memcpy(&udp_[0], &source_port ,sizeof(source_port));
    memcpy(&udp_[2], &dest_port, sizeof(dest_port));
    memcpy(&udp_[4], &udp_length, sizeof(udp_length));
    memcpy(&udp_[6], &check_sum_udp,sizeof(check_sum_udp));
    memcpy(&udp_[8], frame,udp_length-8);


    unsigned char check_data[udp_length+12];
    memcpy(&check_data[0], &fake_head[0],12);
    memcpy(&check_data[12], &udp_[0],udp_length);
    unsigned char *ff= &check_data[0];
    unsigned short check=get_udp_checksum(ff,12+udp_length);
    memcpy(&udp_[6], &check,2);

    /* print frame information */
    printf("******Send udp information******\n");
    printf("source_port  :%d\n",source_port);
    printf("dest_port    :%d\n",dest_port);
    unsigned char s[16];
    itoa(check, s, 2);
    printf("check_sum_udp :%s\n",s);
    printf("udp length : %d\n",udp_length);
    printf("**************************************\n\n");
    unsigned char *udp__=&udp_[0];
    return udp__;
}
