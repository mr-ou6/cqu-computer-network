#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
unsigned char version_and_headlength;//该字节前四位为版本，后四位为首部长度（一个单位代表4字节，最大首部为60字节,这里只有20个字节）
unsigned char distinguish_service; //区分服务
unsigned short ipv4_length; //ip数据报长度
unsigned short identification; //标识
unsigned short flag_and_fragment_move;//前3位表示flag，后13位表示fragment_move
unsigned char TTL; //生存时间
unsigned char protocol_in_ip_header; //协议:UDP为17，TCP为6
unsigned short check_sum_ip; //首部检验和
unsigned char source_ip_address[4];
unsigned char dest_ip_address[4];
unsigned char *pay_load_1;
unsigned char new_data[1500];//将IP源地址，目的地址，协议和数据部分传给上层，体现透明
unsigned char *udp_new;

unsigned char my_source_ip_address[4]={192,178,2,3};
/**
 * check address (char array, length = 6 Bytes)
 * @des_addr received frame's destination address
 * @return flag: the resul of check (boolean)
 **/
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
int addr_check_ip(unsigned char dest_ip_address[4], unsigned char my_source_ip_address_[4]){
    int flag = 1;
    int i ;
    for (i = 0; i < 4; i++){
        if (dest_ip_address[i] != my_source_ip_address_[i])
            flag = 0;
    }
    return flag;
}
unsigned char *receive_ipv4(unsigned char *ipv4_){
    /* analysis frame */
    memcpy(&version_and_headlength, &ipv4_[0], 1);
    memcpy(&distinguish_service, &ipv4_[1], 1);
    memcpy(&ipv4_length, &ipv4_[2], 2);
    memcpy(&identification, &ipv4_[4] ,2);
    memcpy(&flag_and_fragment_move, &ipv4_[6], 2);
    memcpy(&TTL, &ipv4_[8], 1);
    memcpy(&protocol_in_ip_header, &ipv4_[9],1);
    memcpy(&check_sum_ip, &ipv4_[10], 2);

    memcpy(source_ip_address, &ipv4_[12], 4);
    memcpy(dest_ip_address, &ipv4_[16], 4);

//ip数据部分
    pay_load_1 = &ipv4_[20];
    /*  address check */
    if (!addr_check_ip(dest_ip_address, my_source_ip_address)){
        printf("********ip address failed*******\n\n");
        exit(1);}

    printf("\n******receive ipv4 information******\n");
    output_version_and_headlength(version_and_headlength);
    printf("distinguish_service     :%d\n",distinguish_service);
    printf("ipv4_length             :%d\n",ipv4_length);
    printf("identification          :%d\n",identification);
    output_flag_and_fragment_move(flag_and_fragment_move);
    printf("TTL                     :%d\n",TTL);
    printf("protocol_in_ip_header   :%d\n",protocol_in_ip_header);
    unsigned char s[16];
    itoa(check_sum_ip, s, 2);
    printf("check_sum_ip            :%016s\n",s);
    printf("source_ip_address       :");
    output_ipaddress(source_ip_address,4);
    printf("dest_ip_address         :");
    output_ipaddress(dest_ip_address,4);
    printf("**************************************\n");
    //下层对上层透明，传来的IP数据报的源地址和目的地址，和协议部分传给上层。
    unsigned char source_dest_ip_address[8];
    memcpy(&source_dest_ip_address[0], source_ip_address, 4);
    memcpy(&source_dest_ip_address[4], dest_ip_address, 4);
    unsigned char protocol_=protocol_in_ip_header;

    //IP层计算校验和，只计算20个字节首部
    unsigned char*f=&frame[14];
    unsigned short check;
    check = get_ip_checksum(f,20);
    //判断校验和，若为0则成功，同时将数据交到上层;
    if (check==0){
        printf("**** complete successful checksum ****\n\n");
        memcpy(&new_data[0], &source_dest_ip_address[0], 8);
        memcpy(&new_data[8], &protocol_, 1);
        memcpy(&new_data[9], pay_load_1, ipv4_length-20);
        udp_new=&new_data[0];
        return udp_new;
    }
    //校验和不为0则退出
    else{
        printf("******** complete failed checksum *******\n\n");
        exit(1);
    }
}
