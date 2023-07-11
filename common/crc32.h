/**
 * Name         : crc32.h
 * Author       : Qu Xiangjun
 * Version      : 1.0
 * Date         : 2020-10-8
 * Description  : CRC32 Checking
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#define BUFSIZE  1024*4

static unsigned int crc_table[256];  /* crc32 table */
const static char *program_name = "crc32";

static void init_crc_table(void);
static unsigned int crc32(unsigned char * buffer, unsigned int size);

/**
 * initialize crc32 table
 **/
static void init_crc_table(void)
{
    unsigned int c;
    unsigned int i, j;

    for (i = 0; i < 256; i++) {
        c = (unsigned int)i;
        for (j = 0; j < 8; j++) {
            if (c & 1)
                c = 0xedb88320L ^ (c >> 1);
            else
                c = c >> 1;
        }
        crc_table[i] = c;
    }
}

/**
 * find crc32 value in crc32 table
 * @buffer Calculated buffer string
 * @size  Calculated buffer string's length
 * @return crc32 4Bytes
 **/
static unsigned int crc32(unsigned char *buffer, unsigned int size)
{
    unsigned int crc = 0xFFFFFFFF;
    unsigned int i;
    for (i = 0; i < size; i++) {
        crc = crc_table[(crc ^ buffer[i]) & 0xff] ^ (crc >> 8);
    }
    return crc ;
}
