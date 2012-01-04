#ifndef _CONTROL_FLOW__
#define _CONTROL_FLOW__

#include "relation.h"
#include <map>
#include <fstream>
#include <set>
#include <utility>

/* configuration parameters */
#define CONTROL_MAP "./control.txt"
#define SYSSEQ_MAP "./sysseq.txt"

using namespace std;


class ControlFlowRelation : public Relation, public Syscall
{
	map <int, pair<int, Relationship > > control_map_;
	map <int, vector<int> > syscall_sequence_map_; 
	ofstream sys_seq_map_;
	pair<int, Relationship> related(map<string, pair<string, pair<int, Relationship> > >& intermediate, string args, string syscall);

	public:
		ControlFlowRelation()
		{
			mapfile_.open(CONTROL_MAP, ios::out);
			sys_seq_map_.open(SYSSEQ_MAP, ios::out);
			smap.parseMap();
		}
		void serializeSysSeqMap();
		virtual void constructMap();
		virtual void serializeMap();
};

#endif

