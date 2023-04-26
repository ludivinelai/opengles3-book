/****************************************************************
file:         log.c
description:  the source file of log implementation
date:         2020/09/03
author        hufengkai
****************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <Windows.h>
//#include <dirent.h>
#include "log.h"
#include <time.h>
//#include <sys/time.h>
//
//#include <sys/syscall.h>
//



const char *colr[] = {
    "\033[0m",
    "\033[0;31m",
    "\033[0;32m",
    "\033[0;35m",
    "\033[0;34m",
};


const char *level_output_info[] = {
    "E",
    "W",
    "I",
    "D",
    "V",
};

#define NUM_LEN         6
#define TIMER_LEN       8
#define CAN_STR_LEN    16

#define LOG_ON "ON"
#define LOG_OFF "OFF"

FILE *logFileID = NULL;

#define LOG_BUF_SIZE            10240   //(10*1024)
#define MAX_NAME 24
#define MAX_FUNC 26
#define PRE     3
#define F_PRE '.'
#define R_PRE '.'
#define MAX_NAME_BUF (MAX_NAME + PRE)
#define MAX_FUNC_BUF (MAX_FUNC + PRE)

static char s_name_buf[MAX_NAME_BUF];
static char s_func_buf[MAX_FUNC_BUF];

/**
 * @brief lenLimit
 * @param dst bufer
 * @param dst_len
 * @param src must be string
 */
void lenLimit(char dst[], int dst_len, const char *src)
{
    int src_len = strlen(src);
    int start = 0;
    int max_data = dst_len - PRE;
    memset(dst, 0, dst_len);
    if(src_len < max_data) {
        memcpy(dst, src, src_len);
    } else { //copy from mid
        start = (src_len - max_data) / 2;
        memcpy(dst + 1, src + start, max_data);
        dst[0] = F_PRE;
        dst[dst_len - 2] = R_PRE;
    }
}


int need_show(int lv)
{
    char *log = NULL;
    switch(lv) {
        case LOG_WARNING:
            log = getenv("LOG_W");
            break;
        case LOG_ERROR:
            break;
        case LOG_INFO:
            log = getenv("LOG_I");
            break;
        case LOG_DEBUG:
            log = getenv("LOG_D");
            break;
    }
    if(log == NULL) {
        return 1;
    } else {
        return 0;
    }
}

void get_time(char *time_str, size_t time_str_size)
{
    SYSTEMTIME lt;

    GetLocalTime(&lt);
    memset(time_str, 0, time_str_size);
    snprintf(time_str, time_str_size, "%d/%d %d:%d:%d.%-3d", lt.wMonth,lt.wDay,lt.wHour,lt.wMinute,lt.wSecond,lt.wMilliseconds);
    return;
}

void log_msg(const char *name, int lv,  const char *func, int line,  char *fmt, ...)
{
    char log_buf_print[LOG_BUF_SIZE] = {0};
    int log_colr;
    static  char times[32];
    int bufferLen = 0;
    get_time((char *)times, sizeof(times));
    lenLimit(s_name_buf, MAX_NAME_BUF, name);
    lenLimit(s_func_buf, MAX_FUNC_BUF, func);
    bufferLen = snprintf(log_buf_print, LOG_BUF_SIZE, "[%s] (%6s %4s:%-3d) : [%s%s%s] ", times, s_name_buf, s_func_buf, line,   colr[LOG_COLOR_GREEN], level_output_info[lv], colr[LOG_COLOR_BLACK]);
    log_colr = (lv == LOG_ERROR ? LOG_COLOR_RED : LOG_COLOR_BLACK);
    va_list args;
    va_start(args, fmt);
    bufferLen += vsnprintf(log_buf_print + bufferLen, LOG_BUF_SIZE - bufferLen, fmt, args);
    va_end(args);
    if(need_show(lv)) {
        printf("%s%s%s\n", colr[log_colr], log_buf_print, colr[LOG_COLOR_BLACK]);
    }
}


void progress(int percent, char *fmt, ...)
{
    int i = percent;
    char log_buf_print[LOG_BUF_SIZE] = {0};
    int bufferLen = 0;
    if(i != 100) {
        printf(" [%s", colr[LOG_COLOR_BLUE]);
    } else {
        printf(" [%s", colr[LOG_COLOR_GREEN]);
    }
    for(int j = 0; j < i; j++) {
        printf("█");
    }
    for(int j = 0; j < 100 - i; j++) {
        printf(" ");
    }
    printf("%s]", colr[LOG_COLOR_BLACK]);
    printf("\t[%s%3d%%%s]", colr[LOG_COLOR_GREEN], i, colr[LOG_COLOR_BLACK]);
    switch(i % 4) {
        case 0:
            printf("[—]");
            break;
        case 1:
            printf("[\\]");
            break;
        case 2:
            printf("[|]");
            break;
        case 3:
            printf("[/]");
            break;
    }
    va_list args;
    va_start(args, fmt);
    bufferLen += vsnprintf(log_buf_print + bufferLen, LOG_BUF_SIZE - bufferLen, fmt, args);
    va_end(args);
    printf("%s\r", log_buf_print);
    fflush(stdout);
}
