#include"../common/china_time.h"
unsigned char *receive_oil(unsigned char *oil_){
    unsigned int source_account;//最多八位
    unsigned int dest_account;//最多八位
    unsigned int send_time=0;//发送时间
    unsigned int type=0;//类型
    unsigned short size=0; //数据部分的大小
    unsigned char *data;
    unsigned int my_account=88888888;
    memcpy(&source_account,&oil_[0],4);
    memcpy(&dest_account,&oil_[4],4);
    memcpy(&send_time,&oil_[8],4);
    memcpy(&type,&oil_[12],4);
    memcpy(&size,&oil_[16],2);
    unsigned int t_1=Time(1);
    unsigned int t_= Time((unsigned int)send_time);

    data=&oil_[18];
    if (my_account!=dest_account){
        printf("******account failed******\n");
        exit(0);
    }//账号不对则退出。
    /* print frame information */
    printf("******receive oil information******\n");
    printf("source_account  :%d\n",source_account);
    printf("dest_account   :%d\n",dest_account);
    printf("type :%d\n",type);
    printf("send_time : %d\n",t_);
    printf("size : %d\n",size);
    printf("data : %s\n",data);
    printf("**************************************\n");
    printf("******receive successfully******\n");

    FILE *file= fopen("pipe.bin","w");// 将模拟物理层二进制流文件清空，即表示本次传送结束
    fclose(file);

    //将受到的信息用txt装起来


    if (type==0) {//判定类型为0，则收到聊天信息，直接展示出来（可以限定每次发送内容的限制500字）
        FILE *fp;//定义文件流指针，用于打开读取的文件
        fp = fopen("receive_history.txt","a+");//只读方式打开文件
        Time_file(t_, 0, fp);
        Time_file(t_1, 1, fp);//收到
        fprintf(fp, "%s", data);
        fclose(fp);//关闭文件，有打开就要有关闭*
    }
    else if (type==1) {//判定类型为0，则收到聊天信息，直接展示出来（可以限定每次发送内容的限制500字）
        FILE *fp;//定义文件流指针，用于打开读取的文件
        fp = fopen("receive_history.txt","a+");//只读方式打开文件
        Time_file(t_, 0, fp);
        Time_file(t_1, 1, fp);//收到
        fprintf(fp, "%s", data);
        fclose(fp);//关闭文件，有打开就要有关闭*
    }
    else if(type==2){//判定类型为1，文件则展示文件存放的地址
        FILE *fp_save;
        printf("将文件放在:");
        unsigned char new_place[100];
        scanf("%s",new_place);
        fp_save = fopen(new_place,"a+");//只读方式打开文件
        fprintf(fp_save, "%s", data);//把送来的文件存到指定路径下

        FILE *fp_save1;
        fp_save1 = fopen("receive_history.txt","a+");//只读方式打开文件
        Time_file(t_, 0, fp_save1);
        Time_file(t_1, 1, fp_save1);
        fprintf(fp_save1, "文件存放路径:%s", new_place);//指定路径写入收到的内容中
        fclose(fp_save);
        fclose(fp_save1);
    }
    return data;
}
