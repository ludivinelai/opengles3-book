/****************************************************************
file:         log.c
description:  the source file of log implementation
date:         2022/09/03
author        hufengkai
****************************************************************/
#ifndef __LOG_H__
#define __LOG_H__

//#define BIN2CHAR(ch) (((ch) > ' ' && (ch) <= '~') ? (ch) : '.')
#define filename(x) (strrchr(x,'/'))?strrchr(x,'/')+1:x

#define SUCCESSFUL 0
#define FAIL -1
#define LIGHT_ON 1
#define LIGHT_OFF 0
#define NOT_START -2
#define UNKNOWN -3

/* log level definition */
enum LOG_LEVEL_TYPE {
    LOG_ERROR,
    LOG_WARNING,
    LOG_INFO,
    LOG_DEBUG
};
enum {
    LOG_COLOR_BLACK,
    LOG_COLOR_RED,
    LOG_COLOR_GREEN,
    LOG_COLOR_PURPLE,
    LOG_COLOR_BLUE,
};


typedef struct {
    int  level;
    char name[32];
} log_info_t;


#define log_e(...) \
    do\
    {\
        log_msg(filename(__FILE__), LOG_ERROR, __FUNCTION__, __LINE__, __VA_ARGS__);\
    } while (0)


#define log_w(...) \
    do\
    {\
        log_msg(filename(__FILE__), LOG_WARNING, __FUNCTION__, __LINE__, __VA_ARGS__);\
    } while (0)



#define log_i( ...) \
    do\
    {\
        log_msg(filename(__FILE__), LOG_INFO, __FUNCTION__, __LINE__, __VA_ARGS__);\
    } while (0)

#define log_d( ...) \
    do\
    {\
        log_msg(filename(__FILE__), LOG_DEBUG, __FUNCTION__, __LINE__, __VA_ARGS__);\
    } while (0)


// only for debug print,don't save to the log file.
#define log_hex_dump(name, buf, len)\
    do\
    {\
        dumphex(name, LOG_INFO, buf, len, __FUNCTION__, __LINE__);\
    } while(0)


void log_msg(const char *name, int lvl,  const char *func, int line,  char *fmt, ...);
void progress(int percent, char *fmt, ...);

#endif
