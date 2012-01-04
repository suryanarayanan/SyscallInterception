
#ifndef __SYSTRACE_H_
#define __SYSTRACE_H_

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/user.h>
#include <sys/reg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* custom definitions, add or change here if you change the structure of syscallmap.txt */

#define MAP_FILE "./syscallmap.txt"
#define MAP_SEPARATOR ,
#define MAP_ARGS 5

#define LOG_FILE "/tmp/systrace.log"
#define TRACE_FILE "./calltrace.txt"
#define SYSCALLS 320

/* globals */
/* TODO: implement a singleton for logging! */

static FILE* LOGFILE;
static FILE* TRACE;
static time_t RAWTIME;
static int SYSMAP_LENGTH;
static int ARG_CNT;
/* new error codes go here */

typedef enum
{
    NO_ERROR,
    ERROR_INITIALIZING,
    ERROR_CLEANINGUP,
    ERROR_OPENING_FILE,
    ERROR_PARSING_MAP,
    ERROR_TRACING_SYSCALL,
}err_code;

/* new error message corresponding to error codes go here */
/* Ugly ,TODO: implement a map! */

char ERROR_MESSAGES[50][100] = {
"No error",
"Error during initializing the program",
"Error cleaning up the resources",
"Error in opening system call map file",
"Error parsing system call map file",
"Error while tracing system call",
};



/* Map which holds the syscall patterns */
typedef struct
{
    char args[MAP_ARGS];
}syscallMap;

syscallMap syscall_map[SYSCALLS];

/* set of utility functions */

err_code init();
err_code parseMap(FILE *mapfile);
err_code loadSystemCallMap();
err_code trace(pid_t child, long sys, unsigned long *arg);
int notAscii(char *data);
void extractData(pid_t child, long addr, char *str, int len);
void getErrorMessage(char *);
void logError(char *error_msg);

#endif
