#include "symtab_utils.hpp"
#include <stack>
using namespace std;
Symbol* getSymbol(SymbolTable *symtab, const std::string &name, uint64_t* scopeLevel)
{
    if(name.find('*')!= string::npos)
    {
        if(scopeLevel)
        {
            *scopeLevel = 0;
        }
        SymbolTable * glob = symtab;
        while (glob->parent)
        {
            glob = glob->parent;
        }
        
        SymtabIter iter = glob->symbols.find("*");
        return (Symbol*)(*iter).second;
    }

    while (symtab)
    {
        SymtabIter iter = symtab->symbols.find(name);
        if(iter != symtab->symbols.cend())
        {
            if(scopeLevel)
            {
                *scopeLevel = symtab->scopeLevel;
            }
            return (Symbol*)(*iter).second;
        }
        symtab = symtab->parent;
    }

    return nullptr;
}

Symbol *getSymbolLocal(SymbolTable *symtab, const std::string &name)
{
    SymtabIter iter = symtab->symbols.find(name);
    if(iter != symtab->symbols.cend())
    {
        return (Symbol*)(*iter).second;
    }
    return nullptr;
}

FieldDesc getNthFieldDesc(SymbolTable *symtab, SymbolType *type, int idx)
{
    if(isBuiltInType(type))
    {
        printf("Internal Error: getNthFieldType should not be used with non aggregate types\n");
        exit(-1);
    }

    struct AggrDesc
    {
        SymbolType* aggr;
        int offset;
        uint32_t memoryBase;
    };
    

    SymbolType* out;
    stack<AggrDesc> aggrTypes;
    aggrTypes.push({type, 0, 0});
    int localIdx = 0;
    uint32_t memoryOffset = 0;
    while (localIdx < idx)
    {
get_aggr:
        if(aggrTypes.size() == 0 )
        {
            return {nullptr, 0};
        }
        SymbolType* currType = aggrTypes.top().aggr;
        int* offset = &aggrTypes.top().offset;
        uint32_t* memoryBase = &aggrTypes.top().memoryBase;

        if(*offset >= currType->types.size())
        {
            aggrTypes.pop();
            goto get_aggr;
        }

        SymbolType* subType = (SymbolType*) getSymbol(symtab, currType->types[*offset]);
        if(isBuiltInType(subType))
        {
            out = subType;
            memoryOffset = *memoryBase + currType->paramOffset[*offset];
            (*offset)++;
            localIdx++;
        }
        else
        {
            aggrTypes.push({subType, 0, *memoryBase + currType->paramOffset[*offset]});
        }

    }
    
    return {out, memoryOffset};
}
