#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
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
unsigned short get_ip_checksum(unsigned short* buffer, int size)
{
    unsigned long cksum = 0;
    //每16位相加
    while(size>1)
    {
        cksum += *buffer++;
        size -= sizeof(unsigned short);
    }
    //最后的奇数字节
    if(size)
    {
        cksum += *(unsigned char*)buffer;
    }
    cksum = (cksum>>16) + (cksum&0xffff);  //将高16bit与低16bit相加
    cksum += (cksum>>16);
    return (unsigned short)(~cksum); //最后将结果取反,得到checksum
}

void output_ipaddress(unsigned char *x,int len){
    unsigned int i;
    for (i = 0; i < len; i++){
        printf("%d", x[i]);
        if (i != len - 1)
            printf(".");
    }
    printf("\n");
}
void output_version_and_headlength(unsigned char value)
{
    char string[8]={0};
    unsigned int i;
    itoa((value), string, 2);
    printf("version                 :%c\n",string[0]);
    printf("headlength              :");
    for (i =1; i < 5; i++){
        printf("%c", string[i]);
        if(i==4){
            printf("\n");
        }
    }
}
void output_flag_and_fragment_move(unsigned short value)
{   //切片，最后一个分片
    if(value>=0b00000000&&value<=0b0001111111111111) {

        printf("flag                    :000\n");
        printf("fragment                :");
        printf("%d\n",value);
    }
    //不切片
    if(value>=0b0100000000000000&&value<=0b0101111111111111){

        printf("flag                    :010\n");
        printf("fragment                :");
        printf("%d\n",value-0b0100000000000000);
    }
    //切片，后面还有分片
    if(value>=0b1000000000000000&&value<0b1001111111111111){
        printf("flag                    :100\n");
        printf("fragment                :");
        printf("%d\n",value-0b1000000000000000);
    }

}

void ipv4_construction(unsigned char frame[1518],unsigned char *ipv4_,unsigned char version_and_headlength, unsigned char distinguish_service,
                      unsigned short ipv4_length, unsigned short identification, unsigned short flag_and_fragment_move,
                       unsigned char TTL,unsigned char protocol_in_ip_header,unsigned short check_sum_ip,
                       unsigned char *source_ip_address,unsigned char *dest_ip_address){
    /* data length check */
    /* package udp*/
    memcpy(&frame[14], &version_and_headlength ,sizeof(version_and_headlength));
    memcpy(&frame[15], &distinguish_service, sizeof(distinguish_service));
    memcpy(&frame[16], &ipv4_length, sizeof(ipv4_length));
    memcpy(&frame[18], &identification,sizeof(identification));
    memcpy(&frame[20], &flag_and_fragment_move, 2);
    memcpy(&frame[22], &TTL, sizeof(TTL));
    memcpy(&frame[23], &protocol_in_ip_header, sizeof(protocol_in_ip_header));
    memcpy(&frame[24], &check_sum_ip, sizeof(check_sum_ip));
    memcpy(&frame[26], source_ip_address, 4);
    memcpy(&frame[30], dest_ip_address, 4);
    memcpy(&frame[34], ipv4_, ipv4_length-20);
    unsigned char*f=&frame[14];
    unsigned short check;

    check = get_ip_checksum(f,20);
    memcpy(&frame[24], &check, sizeof(check));
    /* print frame information */
    printf("******Send ipv4 information******\n");
    output_version_and_headlength(version_and_headlength);
    printf("distinguish_service     :%d\n",distinguish_service);
    printf("ipv4_length             :%d\n",ipv4_length);
    printf("identification          :%d\n",identification);
    output_flag_and_fragment_move(flag_and_fragment_move);
    printf("TTL                     :%d\n",TTL);
    printf("protocol_in_ip_header   :%d\n",protocol_in_ip_header);
    unsigned char s[16];
    itoa(check, s, 2);
    printf("check_sum_ip            :%016s\n",s);
    printf("source_ip_address       :");
    output_ipaddress(source_ip_address,4);
    printf("dest_ip_address         :");
    output_ipaddress(dest_ip_address,4);
    printf("**************************************\n\n");
}
