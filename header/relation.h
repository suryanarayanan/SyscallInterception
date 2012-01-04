#ifndef _RELATION__
#define _RELATION__

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#define MAP_ARGS 5
#define MAP_FILE "./syscallmap.txt"
#define SYSCALLS 350
#define TRACE_FILE "./calltrace.txt"
#define MAX_CHAR 1000

using namespace std;


struct SyscallMap
{
    char args[MAP_ARGS];
};

/* A one time map holds the interesting syscall arguments structure */

class SystemMap
{
private:
    int countArgs(char*);

public:
    /* kind of bad design. TODO: */
    static SyscallMap syscall_map[SYSCALLS];
    void parseMap();
};

/* Encapsulates each system call found in traces */

class Syscall
{
    string no_;
    vector <string> args_;
    string result_;
    ifstream *trace_;

private:
    vector <string> parseArgument(const string& str);
    string parseSyscallNo(const string& str);
public:
    Syscall();
    ~Syscall();
    void parseTrace(const string& str);
    string getSyscallNo();
    string getSyscallArgs(int i);
    string getSyscallResult();
    ifstream* getTraceFile();
    string trim(const string&);
};


enum Relationship
{
    NONE,
    EQUAL,
    SUBSTRING,
};


class Relation
{
protected:
    ofstream mapfile_;
    SystemMap smap;
public:
    virtual void constructMap() = 0;
    virtual	void serializeMap() = 0;
};

#endif
