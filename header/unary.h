#ifndef _UNARY__
#define _UNARY__

#include "relation.h"
#include <map>
#include <fstream>
#include <set>

/* configuration parameters */
#define UNARY_MAP "./umap.txt"

using namespace std;

class UnaryRelation : public Relation, public Syscall
{
    map <string, set<string> > unary_map_;
    public:
        UnaryRelation()
        {
            mapfile_.open(UNARY_MAP, ios::out);
            smap.parseMap();
        }
        virtual void constructMap();
        virtual void serializeMap();
};

#endif
