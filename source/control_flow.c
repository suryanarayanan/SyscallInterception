#include "control_flow.h"

using namespace std;


void ControlFlowRelation::serializeMap()
{
	map<int, pair<int, Relationship> >::iterator it = control_map_.begin();
	while(it != control_map_.end())
	{
		pair<int,Relationship> temp = it->second;
		mapfile_<<it->first<<",";
		mapfile_<<temp.first<<","<<temp.second<<endl;
		it++;
	}
	serializeSysSeqMap();
}

void ControlFlowRelation::serializeSysSeqMap()
{
	map<int, vector<int> >::iterator it = syscall_sequence_map_.begin();
	while(it != syscall_sequence_map_.end())
	{
		vector <int> temp = it->second;
		sys_seq_map_<<it->first;
		for(int i =0 ; i < int(temp.size()); ++i)
			sys_seq_map_<<","<<temp[i]<<endl;
		it++;
	}
}

/* too bad */
int toDigit(const string& s)
{
	int number = 0;
	for(int i = 0; i < int(s.length()); ++i)
		number = number*10 + (s[i]-'0');
	return number;
}

void ControlFlowRelation::constructMap()
{
	char temp[1000];
	Syscall sys;
	map<string, pair<string, pair<int, Relationship> > > intermediate; 
	int sequence_number = 0;
	while(sys.getTraceFile()->getline(temp, MAX_CHAR))
	{
		string str(temp);
		sys.parseTrace(str);

		for(int i = 0; i < MAP_ARGS; ++i)
		{
			string args = sys.getSyscallArgs(i);
			if(args != " NA")
			{
				pair<int, Relationship> rel = related(intermediate, args, sys.getSyscallNo());
				/* check if we have already seen the argument */
				if(rel.second != NONE)
				{
					/* if they are related check whether the relation is already in control_map */
					vector <int> seq_numbers = syscall_sequence_map_[toDigit(sys.getSyscallNo())];

					/* check for self matching */
					bool flag = false;
					for(int j = 0; j < int(seq_numbers.size()); ++j)
					{
						if(rel.first == seq_numbers[j])
						{	
							flag = true;
							break;
						}
					}
					if(flag)
						continue;

					/* now check for already existing with same relation R */
					flag = false;
					for(int j = 0; j < int(seq_numbers.size()); ++j)
					{
						pair<int, Relationship> temp;
						if(control_map_.count(seq_numbers[j]) > 0)
						{
							temp = 	control_map_[seq_numbers[j]];
							if(temp.second == rel.second)
							{
								flag=true; 
								break;
							}
						}
						
					}
					if(flag)continue;
					
					/* Unique relation R, storing this seq_number, matched_seq number using relation R */
					control_map_[sequence_number] = make_pair(rel.first,rel.second);
					/* TODO could be changed to syscall name. currently hashing using number */
					syscall_sequence_map_[toDigit(sys.getSyscallNo())].push_back(sequence_number);
				}
				else
				{	
					intermediate[args] = make_pair(sys.getSyscallNo(),make_pair(sequence_number, rel.second));
				}
				sequence_number++;
			}
		}
	}

}

/* Only performs EQUIVALENCE relation, add more */

pair<int, Relationship> ControlFlowRelation::related(map<string, pair<string, pair<int, Relationship> > >& intermediate, string args, string syscall)
{
	/* right now only Equality condition */
	pair<int, Relationship> ret;
	if(intermediate.count(args) > 0 && intermediate[args].first != syscall) 
	{	
		/* return the matching sequence number, relationship pair */
		return ret = make_pair(intermediate[args].second.first, EQUAL);		
	} 
	/* no relationship */
	ret = make_pair(-1, NONE);
	return ret;
}
