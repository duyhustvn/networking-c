#include "log.h"
#include <stdio.h>

FILE *logFile = NULL;
LogLevel currentLevel = LOG_DEBUG;


const char* logLevelToString(LogLevel lv) {
    char* levelStr;
    switch (lv) {
        case LOG_DEBUG:
            levelStr = "DEBUG";
            break;
        case LOG_INFO:
            levelStr = "INFO";
            break;
        case LOG_WARN:
            levelStr = "WARN";
            break;
        case LOG_ERROR:
            levelStr = "ERROR";
            break;
        default:
            levelStr = "UNKNOWN";
            break;
    }
    return levelStr;
};

void initLogFile(const char *filepath){
    logFile = fopen(filepath, "a");
    if (logFile == NULL) {
        fprintf(stderr, "Error opening log filed at %s\n", filepath);
    }
};


void closeLogFile() {
    if (logFile != NULL) {
        fclose(logFile);
    }
};


void setLogLevel(LogLevel level) {
    currentLevel = level;
};

void logMessage(LogLevel level, const char *message) {
    if (level < currentLevel) {
        return;
    }

    if (logFile == NULL) {
        fprintf(stderr, "Log file not initialize\n");
    }

    const char* levelStr = logLevelToString(level);
    // fprintf(logFile, "[%02d-%02d-%04d %02d:%02d:%02d] [%s] %s\n",
    //         local->tm_mday, local->tm_mon + 1, local->tm_year + 1900,
    //         local->tm_hour, local->tm_min, local->tm_sec, levelStr, message);

    fprintf(logFile, "[%s] %s\n", levelStr, message);
};
