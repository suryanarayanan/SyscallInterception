#include "systrace.h"


err_code init()
{
    LOGFILE = fopen(LOG_FILE, "a");
    if(LOGFILE == NULL)
        return ERROR_INITIALIZING;
    TRACE = fopen(TRACE_FILE, "a");
    if(TRACE == NULL)
        return ERROR_INITIALIZING;
    return NO_ERROR;
}

err_code cleanUp(int er)
{
    err_code err = NO_ERROR;
    if(fclose(LOGFILE) == EOF)
    {
        err = ERROR_CLEANINGUP;
    }
    if(fclose(TRACE) == EOF)
    {
        err = ERROR_CLEANINGUP;
    }
    if(er)
        printf("Program ended with error. See %s for error logs\n", LOG_FILE);
    else
        printf("Program successfully traced. See %s for the trace\n", TRACE_FILE);
    return err;
}

int countArgs(char* line)
{
    /* counts the number of arguments in map file */

    int cnt = 0;
    int i;
    for(i = 0; i < strlen(line); ++i)
    {
        if(line[i] == ',')
            cnt++;
    }
    return cnt;
}

err_code parseMap(FILE *mapfile)
{

    /* lines in mapfile shouldn't contain whitespace characters */

    char line[25];
    int syscallno;
    char a[5];
    int cnt = 0;
    int i,j;

    /* by default arguments are assigned to no values '0' */
    for(i = 0; i < SYSCALLS; ++i)
        for(j = 0; j < MAP_ARGS; ++j)
            syscall_map[i].args[j] = '0';

    /* reads line by line and parses the argument types */
    while(fscanf(mapfile, "%s", line) != EOF)
    {
        if(countArgs(line) != MAP_ARGS)
            return ERROR_PARSING_MAP;

        /* using static array indexing (bad?) assuming there won't be a drastic changes to syscall interface */
        sscanf(line, "%d,%c,%c,%c,%c,%c", &syscallno, &a[0], &a[1], &a[2], &a[3], &a[4]);
        if(syscallno < 1 || syscallno > SYSCALLS)
            return ERROR_PARSING_MAP;

        for(i = 0; i < MAP_ARGS; ++i)
        {
            syscall_map[syscallno].args[i] = a[i];
        }
        ++cnt;
    }
    SYSMAP_LENGTH = cnt;
    return NO_ERROR;
}


err_code loadSystemCallMap()
{
    FILE *mapfile;
    err_code err = NO_ERROR;
    mapfile = fopen(MAP_FILE, "r");
    if(mapfile == NULL)
        return ERROR_OPENING_FILE;
    err = parseMap(mapfile);
    fclose(mapfile);
    return err;
}

void extractData(pid_t child, long addr, char *str, int len)
{
    char *value;
    int i, j;
    union u
    {
        long val;
        char chars[sizeof(long)];
    } data;
    i = 0;
    j = len / sizeof(long);
    value = str;
    while(i < j)
    {
        data.val = ptrace(PTRACE_PEEKDATA, child, addr + i * 4, NULL);
        memcpy(value, data.chars, sizeof(long));
        ++i;
        value += sizeof(long);
    }
    j = len % sizeof(long);
    if(j != 0)
    {
        data.val = ptrace(PTRACE_PEEKDATA,child, addr + i * 4,NULL);
        memcpy(value, data.chars, j);
    }
    str[len] = '\0';
    return;
}


/* right now this program forks the child process (argv[1]), may be changed to attach an already running process */


int main(int argc, char** argv)
{
    pid_t child;
    long orig_eax;
    int status;
    int called = 0;
    err_code err;
    int fork_error = 0;
    ARG_CNT = argc;
    /* init singletons */
    if((err =  init()) != NO_ERROR)
    {
        logError(ERROR_MESSAGES[err]);
        cleanUp(1);
        return 0;
    }

    if(argc <= 1)
        {
        logError("Insufficient Arguments to program");
        cleanUp(1);
        return 0;
    }

    /* initialize syscall map */
    if((err = loadSystemCallMap()) != NO_ERROR)
    {
        logError(ERROR_MESSAGES[err]);
        cleanUp(1);
        return 0;
    }
    child = fork();
        if(child == 0)
        {
            ptrace(PTRACE_TRACEME, 0, NULL, NULL);
        int exec_err = 0;
        switch(ARG_CNT)
        {
            case 2:
            exec_err = execl(argv[1], argv[1], NULL);
            break;

            case 3:
            exec_err = execl(argv[1], argv[2], NULL);
            break;

            case 4:
            exec_err = execl(argv[1], argv[2], argv[3], NULL);
            break;

            case 5:
            exec_err = execl(argv[1], argv[2], argv[3], argv[4], NULL);
            break;

            case 6:
            exec_err = execl(argv[1], argv[2], argv[3], argv[4], argv[5], NULL);
            break;
        }
            if(exec_err == -1)
        {
            logError("Error forking child process");
            fork_error = 1;
        };
        }

        else
    {
        struct user_regs_struct regs;
            while(1)
            {
            unsigned long args[5];
                    child = wait(&status);
                if(WIFEXITED(status))
                        break;
            orig_eax = ptrace(PTRACE_PEEKUSER, child, 4 * ORIG_EAX, NULL);
                    if(called == 0)
                    {
                            called = 1;
                ptrace(PTRACE_GETREGS, child, NULL, &regs);
                        args[0] = regs.ebx;
                            args[1] = regs.ecx;
                            args[2] = regs.edx;
                args[3] = regs.esi;
                args[4] = regs.eax;
                if((err = trace(child, orig_eax, args)) != NO_ERROR)
                    logError(ERROR_MESSAGES[err]);

                        }
                else
                    {
                called = 0;
                    }
                ptrace(PTRACE_SYSCALL, child, NULL, NULL);
        }
//		fprintf(TRACE, "EndofRun\n");
        }

    /* a glitch - when execl() fails clean up of child and clean up of parent contradicts */
    /* no functional problem, just the error messages are misleading */
    if(!fork_error)
        cleanUp(0);
    else
        cleanUp(1);

    return 0;
}

/* tight coupling with standard system call interface. should change this design */

err_code trace(pid_t child, long sys, unsigned long arg[5])
{
    int i;
    err_code err = NO_ERROR;

    if(fprintf(TRACE, "%ld, ", sys) < 0)
        err = ERROR_TRACING_SYSCALL;

    for(i = 0; i < MAP_ARGS; ++i)
    {
        char data[1024];

        /* crude hack for some strange bug */
        int isdata = 0;

        switch(syscall_map[sys].args[i])
        {
            /* change both 'isdata' to 2 to print only system calls and not arguments */

            case 'd':
                /* change it to 1 */
                isdata = 1;
                break;
            case 'p':
                /* change it to 0 */
                isdata = 0;
                extractData(child, arg[i], data, 1024);
                break;
            case '0':
                isdata = 2;
                break;
        }
        if(isdata == 1)
        {
            if(fprintf(TRACE, "%ld, ", arg[i]) < 0)
                err = ERROR_TRACING_SYSCALL;
        }
        else if(isdata == 0)
        {
            if(fprintf(TRACE, "%s, ", data) < 0)
                err = ERROR_TRACING_SYSCALL;

        }
        else
        {
            if(fprintf(TRACE, "%s, ", "NA") < 0)
                err = ERROR_TRACING_SYSCALL;
        }
    }
    if(fprintf(TRACE, "\n") < 0)
        err = ERROR_TRACING_SYSCALL;

    return err;
}

int notAscii(char *data)
{
    int i;
    for(i = 0 ; i < strlen(data); ++i)
        if(data[i] < 0 || data[i] > 127)
            return 1;
    return 0;
}

void logError(char* str)
{
    time(&RAWTIME);
    char* clock = asctime(localtime(&RAWTIME));
    clock[strlen(clock)-1] = '\0';
    fprintf(LOGFILE, "[%s]: %s\n", clock, str);
    return;
}
