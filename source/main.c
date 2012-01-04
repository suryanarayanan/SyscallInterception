#include "unary.h"
#include "control_flow.h"
#include <sstream>

int main()
{
    Relation *r1 = new UnaryRelation();
    string str;
    r1->constructMap();
    r1->serializeMap();
    cout<<"End of Unary map construction. Umap is stored in "<< UNARY_MAP<<endl;
    Relation *r2 = new ControlFlowRelation();
    r2->constructMap();
    r2->serializeMap();
    cout<<"End of Control Flow map construction. Cmap is stored in "<<CONTROL_MAP<<endl;
    return 0;
}
