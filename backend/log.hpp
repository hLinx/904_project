#pragma once
#include<stdio.h>
#include<assert.h>
#include<stdlib.h>
#include<stdarg.h>
#include<time.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<iostream>
#include<string>
#define DEBUG 0
#define NOTICE 1
#define WARINING 2
#define FATAL 3
#define LOGFILE "log.txt"
const char *log_level[]={"DEBUG", "NOTICE", "WARINING", "FATAL"};
class MY_LOG
{
    public:
    void init()
    {
        umask(0);
        int fd = open(LOGFILE,O_CREAT | O_APPEND | O_WRONLY,0666);//只写 追加方式打开 权限0666
        dup2(fd,1);//重定向
        dup2(fd,2);
        close(fd);
    }
    void log(int level,const char* format,...)
    {
        assert(level >= DEBUG);
        assert(level <= FATAL);

        char *name = getenv("USER");
        char logInfo[2048];
        va_list ap; // ap -> char*
        va_start(ap, format);
        vsnprintf(logInfo, sizeof(logInfo)-1, format, ap);
        va_end(ap); // ap = NULL
        #ifdef CLIENT
        FILE *out = fopen("clinet.txt","a+");
        #else
        FILE *out = (level == FATAL) ? stderr:stdout;
        #endif

        fprintf(out, "%s | %u | %s | %s\n", \
        log_level[level], \
        (unsigned int)time(nullptr),\
        name == nullptr ? "unknow":name,\
        logInfo);
        fflush(out);
    }
};

