unsigned short my_port=34;
//增加校验和
unsigned short get_udp_checksum(unsigned short*buffer1, int size1)
{unsigned long long  cksum = 0;
    //udp数据报
    while(size1>1)
    {
        cksum += *buffer1++;
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
unsigned  char *receive_udp(unsigned char *udp_new_){
    unsigned char fake_head[12];
    unsigned char zero=0X00;
    unsigned short source_port;//源端口
    unsigned short dest_port;//目的端口
    unsigned short udp_length;//udp数据报长度1518-14-4-20=1480
    unsigned short check_sum_udp;//检验和
    unsigned char *pay_load_2;
    //将传进来的最大可能UDP报文变为实际长度的报文大小
   /* memcpy(&udp_length, &udp_new_1[13], 2);
    printf("sdasda%d",udp_length);
    unsigned char udp_new_[udp_length+9];
    memcpy(&udp_new_[0], &udp_new_1[0], udp_length+9);//加九的原因时，这九个字节是ip层的东西用于构造伪首部*/
    /* package udp*/
    memcpy(&source_port, &udp_new_[9] ,2);
    memcpy(&dest_port, &udp_new_[11], 2);
    memcpy(&udp_length, &udp_new_[13], 2);
    memcpy(&check_sum_udp, &udp_new_[15],2);
    pay_load_2=&udp_new_[17];
    //伪首部
    memcpy(&fake_head[0], &udp_new_[0] ,4);
    memcpy(&fake_head[4], &udp_new_[4], 4);
    memcpy(&fake_head[8], &zero, 1);
    memcpy(&fake_head[9], &udp_new_[8],1);
    memcpy(&fake_head[10], &udp_length,2);
    if (dest_port!=my_port){
        printf("********port failed*******\n\n");
        exit(0);
    }
    unsigned char check_data[udp_length+12];
    memcpy(&check_data[0], &fake_head[0],12);
    memcpy(&check_data[12], &udp_new_[9],udp_length);
    unsigned char *ff= &check_data[0];
    unsigned short check=get_udp_checksum(ff,12+udp_length);
    /* print frame information */
    printf("******receive udp information******\n");
    printf("source_port  :%d\n",source_port);
    printf("dest_port    :%d\n",dest_port);
    unsigned char s[16];
    itoa(check_sum_udp, s, 2);
    printf("check_sum_udp :%s\n",s);
    printf("udp length : %d\n",udp_length);
    printf("**************************************\n");
    if(check==0){
        printf("************check_sum is 0************\n******complete successful checksum ****\n\n");
    }
    else{
        printf("******** complete failed checksum *******\n\n");
        exit(1);
    }
    return pay_load_2;
}
