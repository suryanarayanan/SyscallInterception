#include "unary.h"
#include <sstream>

using namespace std;

void UnaryRelation::constructMap()
{
	char temp[1000];
	Syscall sys;
	while(sys.getTraceFile()->getline(temp, MAX_CHAR))
	{
		string str(temp);
		sys.parseTrace(str);
		for(int i = 0; i < MAP_ARGS; ++i)
		{
			string args = sys.getSyscallArgs(i);
			if(args != " NA")
			{
				ostringstream ss;
				ss<<i<<"["<<trim(args)<<"]";
				unary_map_[sys.getSyscallNo()].insert(ss.str());
			}
		}
	}
}

void UnaryRelation::serializeMap()
{
	map<string, set<string> >::iterator it = unary_map_.begin();
	while(it != unary_map_.end())
	{
		set <string> temp = it->second;
		mapfile_<<it->first<<",";
		set <string> ::iterator sit = temp.begin();
		while(sit != temp.end())
		{
			mapfile_<<*sit<<",";
			sit++;
		}
		mapfile_<<endl;
		it++;
	}
}

