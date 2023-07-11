#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "../common/crc32.h"
/**
 * print octal number 
 * @x octal array
 * @len octal array's length
 **/
void output_octal(unsigned char *x, unsigned int len){
    unsigned int i;
    for (i = 0; i < len; i++){
        printf("%02x", x[i]);
        if (i != len - 1&&len==6)
            printf(".");
        else{
            printf(" ");
        }
    }
    printf("\n");
}

/**
 * frame construction function
 * @frame return frame
 * @sta_addr start address
 * @des_addr destination address
 * @ protocol
 * @payload frame's data
 * @payload_len data's length
 **/
unsigned int frame_construction(unsigned char frame[1518], unsigned char *source_addr, unsigned char *des_addr,
                       unsigned short protocol,unsigned int payload_len){
    /* data length check */
    /*
    if (payload_len < 46)    {
        printf("[Error] Payload length too low!\n");
        exit(1);
    }else if (payload_len > 1500){
        printf("[Error] Payload length too long!\n");
        exit(1);
    }*/
    /* Construct data to be a frame array */
    memcpy(&frame[0], des_addr, 6);
    memcpy(&frame[6], source_addr, 6);
    memcpy(&frame[12], &protocol, sizeof(protocol));
    /* crc32: check frame sequence */
    init_crc_table(); /* initialize crc table */
    unsigned int crc32_result = crc32(frame, 14 + payload_len);
    memcpy(&frame[14+payload_len], &crc32_result, sizeof(crc32_result));//crc32封装
    /* get char array for crc32 result, so that program can print it */
    unsigned char crc32_result_[4];
    memcpy(crc32_result_, &crc32_result, sizeof(crc32_result));
    /* frame length */
    unsigned int length = payload_len + 6 + 6 + sizeof(protocol) + sizeof(crc32_result);
    /* print frame information */ 
    printf("******send frame information******\n");
    printf("Start address: ");
    output_octal(source_addr,6);
    printf("Des address  : ");
    output_octal(des_addr,6);
    printf("protocol     : %d\n",protocol);
    printf("CRC32        : ");
    output_octal(crc32_result_, 4);
    printf("Frame length : %d\n", length);
    printf("**************************************\n");
    return length;
}
