#include "logger.h"
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

static logLevel currentLogLevel = LOG_DEBUG;
static FILE* logFile = NULL;

// Function to convert log level to string for output
static const char* logLevelToString(logLevel level) {
    switch (level) {
        case LOG_EMERGENCY: return "EMERGENCY";
        case LOG_ALERT: return "ALERT";
        case LOG_CRITICAL: return "CRITICAL";
        case LOG_ERROR: return "ERROR";
        case LOG_WARNING: return "WARNING";
        case LOG_INFO: return "INFO";
        case LOG_DEBUG: return "DEBUG";
        default: return "UNKNOWN";
    }
}

// Function to initialize logger with optinal file name and lowest log level
void loggerInit(logLevel level, const char* fileName){
    currentLogLevel = level;
    if(fileName){
        logFile = fopen(fileName, "a");
        if(logFile == NULL){
            fprintf(stderr, "[LOGGER ERROR] Failed to open log file: %s\n", fileName);
        }
    }
}

// Function to set the log level dynamically
void loggerSetLevel(logLevel level) {
    currentLogLevel = level;
}   

// Function to close the log file if it was opened
void loggerClose(void){
    if(logFile != NULL){
        fclose(logFile);
        logFile = NULL;
    }
}

// Function to log messages with a specific level, file name, line number and format string
void loggerLog(logLevel level, const char* fileName, int line, const char* format, ...){
    if(level > currentLogLevel || logFile == NULL) {
        return; // Ignore log if level is lower than current or file is not open
    }

    // Get time stamp
    time_t now = time(NULL);
    struct tm* localTime = localtime(&now);
    char timeBuffer[32];
    strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", localTime);

    // Prepare log message
    char message[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    // Final fomatted log entry
    char logEntry[1200];
    snprintf(logEntry, sizeof(logEntry), "[%s] [%s] [%s:%d] %s\n", 
             timeBuffer, logLevelToString(level), fileName, line, message);

    // Output to stdout or stderr
    FILE* output = (level <= LOG_ERROR) ? stderr : stdout;
    fprintf(output, "%s", logEntry);
    fflush(output);

    // Ouput to log file if configured
    if(logFile != NULL) {
        fprintf(logFile, "%s", logEntry);
        fflush(logFile);
    }
}