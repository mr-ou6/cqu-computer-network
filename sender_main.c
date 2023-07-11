#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "send/sender_mac.h"
#include "send/sender_ipv4.h"
#include "send/sender_udp.h"
#include "send/sender_oil.h"
//oil
unsigned int source_account =66666666;//最多八位
unsigned int dest_account =88888888;//最多八位
unsigned int send_time=0;//发送时间
unsigned int type=0;//类型
unsigned short size=0; //数据部分的大小
//头部15个字节

//udp
unsigned short source_port=12;//源端口
unsigned short dest_port=34;//目的端口
unsigned short udp_length=1480;//udp数据报长度1518-14-4-20=1480
unsigned short check_sum_udp=0;//检验和
//ipv4
unsigned char version_and_headlength=0x15;//该字节前四位为版本，后四位为首部长度（一个单位代表4字节，最大首部为60字节,这里只有20个字节）
unsigned char distinguish_service=0x00; //区分服务
unsigned short ipv4_length=0; //ip数据报长度
unsigned short identification=12345; //标识
unsigned short flag_and_fragment_move=0b0100000000000000;//前3位表示flag，后13位表示fragment_move
unsigned char TTL=0x00; //生存时间
unsigned char protocol_in_ip_header=17; //协议:UDP为17，TCP为6
unsigned short check_sum_ip=0; //首部检验和
unsigned char source_ip_address[4]={192,178,1,0};
unsigned char dest_ip_address[4]={192,178,2,3};
//mac
unsigned char sta_addr[6] = {0x11, 0x33, 0x55, 0x44, 0x22, 0x55}; /* start address */
unsigned char des_addr[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55}; /* destination address */
unsigned short protocol = 1;


void send_frame(FILE *file,unsigned char *frame1, unsigned int frame_len){
    if (file == NULL)    {
        printf("File open error!\n");
    }
    fwrite(frame1, sizeof(char), frame_len, file);

    printf("\n");
    fclose(file);
    printf("Frame send success!\n");
}
int main(){
    FILE *new_file= fopen("pipe.bin","w");
    fclose(new_file);//新建传输

    char data[65512];
    int len=0;
    unsigned int label;//类型
    printf("你发送的内容:\n0.短文本\n1.长文本\n2.文本文件\n你的选择:");
    scanf("%d",&label);
    if (label==0) {
        printf("输入短文本内容:");
        scanf("%s", data);
        while(data[++len] != '\0'); //计算字符串长度
        type=0;//报文传输的类型
        if(len>1480){
            printf("超过上限");
            exit(1);
        }
    }
    else if(label==1){
        printf("输入长文本内容:");
        scanf("%s", data);
        while(data[++len] != '\0'); //计算字符串长度
        type=1;//报文传输的类型类型
        if(len<=1480){
            printf("请改用短文本发送");
            exit(1);
        }

    }
    else if(label==2){
        unsigned char file_path[100];
        printf("输入发送文件的路径:");
        scanf("%s",file_path);
        FILE *fp;//定义文件流指针，用于打开读取的文件
        fp = fopen(file_path,"rb");//只读方式打开文件
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        rewind(fp);
        fread(data,1,len, fp);
        fclose(fp);
        type=2;//报文传输的类型类型
    }
    unsigned int length;
    //应用层oil
    unsigned char* ready_oil;
    if(len>65513) {//2^16-8-15;
        printf("数据太大，只支持最大发送64kb文件");
        exit(0);
    }
    size=len;
    ready_oil=oil_construction(source_account,dest_account,send_time,type,size,data);
    //传输层UDP
    unsigned char* ready_udp;
    udp_length=8+18+size;
    ready_udp=udp_construction(ready_oil,source_port,dest_port,udp_length,check_sum_udp,protocol_in_ip_header,source_ip_address,dest_ip_address);
    //网络层ipv4
    //分片这里操作；
    //如果封装好的udp数据报小于等于IP数据报最大的数据容纳量，则不用切片，直接发送
    if(type==0) {
        FILE *file = fopen("pipe.bin", "wb+");//定义底层2进制文件
        unsigned char frame[1518]; /* 固定最长frame array为1518 */
        ipv4_length = 20 + udp_length;
        ipv4_construction(frame,ready_udp,version_and_headlength, distinguish_service, ipv4_length, identification,
                          flag_and_fragment_move,
                          TTL, protocol_in_ip_header, check_sum_ip, source_ip_address, dest_ip_address);
        //数据链路层
        length = frame_construction(frame, sta_addr, des_addr, protocol, ipv4_length);
        //以二进制形式存入二进制文件模拟物理层传输
        send_frame(file ,frame, length);
    }
    //若udp数据报大于IP数据报数据部分最大容纳量，则需要切片。
    else{
        //定义底层2进制文件
        unsigned int counts=0;
        if(udp_length%1480==0) counts=udp_length/1480;
        else{counts=udp_length/1480+1;}
        unsigned short ready_flag_and_fragment_move;
        unsigned int i;
        for(i=0;i<counts;i++){
            FILE *file = fopen("pipe.bin", "ab+");
            if(counts!=1)
                printf("分片：%d\n",i);
            unsigned char frame[1518]; /* 固定最长frame array为1518 */
            unsigned char new_udp[1480];
            if(i!=counts-1) {
                ipv4_length = 1480+20;
                memcpy(&new_udp[0],&ready_udp[i*1480],1480);
                unsigned short shu=0b1000000000000000;
                shu=shu+(unsigned short)i*1480/8;
                unsigned short new_flag_and_fragment_move=shu;//前3位表示flag，100指还有分片
                // 后13位表示fragment_move
                ready_flag_and_fragment_move=new_flag_and_fragment_move;
            }
            else{
                ipv4_length = 20 + udp_length%1480;
                memcpy(&new_udp[0],&ready_udp[i*1480],udp_length-1480*i);
                unsigned short shu=(unsigned short)i*1480/8;
                unsigned short new_flag_and_fragment_move=shu;//前3位表示flag，000指这个分片是最后一个了
                // 后13位表示fragment_move
                ready_flag_and_fragment_move=new_flag_and_fragment_move;
            }
            ipv4_construction(frame,new_udp,version_and_headlength, distinguish_service, ipv4_length, identification,
                              ready_flag_and_fragment_move,
                              TTL, protocol_in_ip_header, check_sum_ip, source_ip_address, dest_ip_address);
            //数据链路层
            length = frame_construction(frame, sta_addr, des_addr, protocol, ipv4_length);
            //以二进制形式存入二进制文件模拟物理层传输
            send_frame(file,frame, length);
        }
    }
    return 0;
}
