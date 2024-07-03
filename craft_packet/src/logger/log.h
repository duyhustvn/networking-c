#ifndef LOG_H_
#define LOG_H_


#include <stdio.h>

// Define log levels
typedef enum {
LOG_DEBUG,
LOG_INFO,
LOG_WARN,
LOG_ERROR,
LOG_NONE
} LogLevel;

const char* logLevelToString(LogLevel level);

void initLogFile(const char *filename);
void closeLogFile();
void setLogLevel(LogLevel level);
void logMessage(LogLevel level, const char *message);

#endif // LOG_H_
