#include "log.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

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

void logMessage(LogLevel level, const char *format, ...) {
    if (logFile == NULL) {
        fprintf(stderr, "Log file not initialized\n");
        return;
    }

    if (level < currentLevel) {
        return; // Don't log messages below the current log level
    }

    // Get the current time
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);

    // Log level string representation
    const char *levelStr;
    switch (level) {
        case LOG_DEBUG: levelStr = "DEBUG"; break;
        case LOG_INFO:  levelStr = "INFO";  break;
        case LOG_WARN:  levelStr = "WARN";  break;
        case LOG_ERROR: levelStr = "ERROR"; break;
        default:        levelStr = "UNKNOWN"; break;
    }

    // Write the timestamp and log level to the log file
    fprintf(logFile, "[%02d-%02d-%04d %02d:%02d:%02d] [%s] ",
            local->tm_mday, local->tm_mon + 1, local->tm_year + 1900,
            local->tm_hour, local->tm_min, local->tm_sec, levelStr);

    // Handle the variable arguments
    va_list args;
    va_start(args, format);
    vfprintf(logFile, format, args);
    va_end(args);

    // Write a newline character at the end of the log message
    fprintf(logFile, "\n");
};
