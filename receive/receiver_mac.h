#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../common/crc32.h"
 /* frame array */
unsigned char sta_addr[6]; /* start address */
unsigned char des_addr[6]; /* destination address */
unsigned short protocol;
unsigned char *payload;
unsigned int frame_len;
unsigned int payload_len;
unsigned char my_addr[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55}; /* destination address */
unsigned char all[67218];
unsigned char frame[1518];
/**
 * get file size
 * @file_handle FILE
 * @return size: file size
 **/
int file_size(FILE *file_handle){
    /* 获取当前读取文件的位置 进行保存 */
    unsigned int current_read_position = ftell(file_handle);
    int file_size;
    fseek(file_handle, 0, SEEK_END);
    /* 获取文件的大小 */
    file_size = ftell(file_handle);
    /* 恢复文件原来读取的位置 */
    fseek(file_handle, current_read_position, SEEK_SET);
    return file_size;
}
/**
 * print octal number 
 * @x octal array
 * @len octal array's length
 **/
int output_octal(unsigned char *x, unsigned int len){
    unsigned int i;
    unsigned int flag=len;
    for (i = 0; i < len; i++)
    {
        printf("%02x", x[i]);
        if (i != len - 1&&flag==6)
            printf(".");
        if (i != len - 1&&flag==4)
            printf(" ");
        if (i % 9 == 0 && i != 0)
            printf("\n");
    }

    printf("\n");
    return 0;
}
/**
 * check address (char array, length = 6 Bytes)
 * @des_addr received frame's destination address
 * @return flag: the resul of check (boolean) 
 **/
int addr_check(unsigned char des_addr[6], unsigned char my_addr[6]){
    int flag = 1;
    int i = 0;
    for (i = 0; i < 6; i++){
        if (des_addr[i] != my_addr[i])
            flag = 0;
    }
    return flag;
}
/**
 * @function: Receive frame from local file. Analysis frame architecture. CRC32 check frame sequence s. Output frame information.
 **/
unsigned char *receive_frame(unsigned int begin){

    FILE *file = fopen("pipe.bin", "r");
    /* read frame length */
    frame_len = file_size(file);

    /* check frame length */
    if (frame_len < 64) {
        // check frame length
        printf("[Error] Payload length too short!\n");
        exit(1);
    }
    /* read frame */
    fread(all, sizeof(char), frame_len, file);

    memcpy(frame,&all[begin],1518);
    /* analysis frame */
    memcpy(des_addr, &frame[0], 6);
    memcpy(sta_addr, &frame[6], 6);
    memcpy(&protocol, &frame[12], sizeof(protocol));

    payload=&frame[14];
    /*  address check */
    if (!addr_check(des_addr, my_addr))
        exit(1);
    /* crc32 check frame se,my_address */
    init_crc_table();
    unsigned int crc32_temp = crc32(frame, 1518 - 4);
    /* get char array for crc32 result, so that program can print it */
    unsigned char crc32_result[4];
    memcpy(crc32_result, &crc32_temp, sizeof(crc32_temp));

    unsigned int i = 0;
    unsigned int flag_crc32 = 1;
    for (i = 3; i < sizeof(crc32_result); i--){
        if (crc32_result[i] != frame[1518 - (4 - i)])
            flag_crc32 = 0;
    }
    if (flag_crc32){
        printf("CRC32 check success!\n");
    }else{
        printf("[Error] CRC32 check error!\n");
        printf("[Error] frame's CRC32 : ");
        printf("%02x %02x %02x %02x\n", frame[frame_len - 4], frame[frame_len - 3], frame[frame_len - 2], frame[frame_len - 1]);
        printf("[Error] calculated CRC32 : ");
        output_octal(crc32_result, 4);
        exit(1);
    }
    /* print frame information */
    printf("******receive frame information******\n");
    printf("Start address: ");
    output_octal(sta_addr, 6);
    printf("Des address  : ");
    output_octal(des_addr, 6);
    printf("protocol     : %d\n", protocol);
    printf("CRC32        : ");
    output_octal(crc32_result, 4);
    printf("**************************************\n");
    printf("Frame received success!\n");
    return payload;
}
