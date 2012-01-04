#include "relation.h"
#include <sstream>

/* for the two c style function */
#include <cstdio>
#include <cstring>

using namespace std;

SyscallMap SystemMap::syscall_map[SYSCALLS];

int SystemMap::countArgs(char* line)
{
	/* counts the number of arguments in map file */

	int cnt = 0;
	int i;
	for(i = 0; i < int(strlen(line)); ++i)
	{
		if(line[i] == ',')
			cnt++;
	}
	return cnt;
}


void SystemMap::parseMap()
{

	/* lines in mapfile shouldn't contain whitespace characters */
	char line[25];
	int syscallno;
	char a[5];
	int cnt = 0;
	int i,j;
	FILE *mapfile = fopen(MAP_FILE, "r");
	
	/* by default arguments are assigned to no values '0' */
	for(i = 0; i < SYSCALLS; ++i)
		for(j = 0; j < MAP_ARGS; ++j)
			syscall_map[i].args[j] = '0';

	/* reads line by line and parses the argument types */
	while(fscanf(mapfile, "%s", line) != EOF)
	{
		if(countArgs(line) != MAP_ARGS)
		{
			fclose(mapfile);
			return;
		}
		/* using static array indexing (bad?) assuming there won't be a drastic changes to syscall interface */
		sscanf(line, "%d,%c,%c,%c,%c,%c", &syscallno, &a[0], &a[1], &a[2], &a[3], &a[4]);
		if(syscallno < 1 || syscallno > SYSCALLS)
		{
			fclose(mapfile);
			return;
		}
		
		for(i = 0; i < MAP_ARGS; ++i)
		{
			syscall_map[syscallno].args[i] = a[i];
		}
		++cnt;
	}
	fclose(mapfile);
	return;
}




Syscall::Syscall()
{
	args_.resize(MAP_ARGS);
	trace_ = new ifstream(TRACE_FILE, ios::in);
}

Syscall::~Syscall()
{
	delete trace_;
}


void Syscall::parseTrace(const string& str)
{
	vector <string> args = parseArgument(str);
	/* looks dirty. clean it up! */	
	no_ = args[0];
	for(int i = 1; i <= MAP_ARGS; ++i)
		args_[i-1] = args[i];
}


ifstream* Syscall::getTraceFile()
{
	return trace_;
}


string Syscall::trim(const string& str)
{
	/* it only does ltrim */

	string temp;
	bool flag = false;
	for(int i = 0; i < int(str.length()); ++i)
	{
		if(flag == false && str[i] == ' ')
			continue;
		else
		{
			flag = true;
			temp += str[i];
		}
	}
	return temp;
}

string Syscall::parseSyscallNo(const string& str)
{
	return trim(str.substr(0,str.find(",")));
}

/* TODO parse arguments */

/* one dumb way of parsing. This is where BOOST wins over stl? */

vector <string> Syscall::parseArgument(const string& str)
{
	istringstream ss(str);
	vector <string> s;
	string temp;
	while(getline(ss, temp, ','))
		s.push_back(temp);
	
	return s;
}


string Syscall::getSyscallNo()
{
	return no_;
}

string Syscall::getSyscallArgs(int i)
{
	return args_[i];
}

string Syscall::getSyscallResult()
{
	return result_;
}
