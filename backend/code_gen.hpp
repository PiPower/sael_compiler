#ifndef CODE_GEN
#define CODE_GEN
#include "../frontend/parser.hpp"

/*
    Mnemomic can either be instruction or label.
    If it is a label then src is a sequence of
    directives used  before writing a label and 
    dest is a sequence of directives used after
    writing a label.
    If mnemonic is instruction then src is source operand
    and dest is destination operand

*/

#define LABEL 0x01
#define INSTRUCTION 0x02

struct Instruction
{
    uint8_t type;
    uint8_t padd[7];
    std::string mnemonic;
    std::string src;
    std::string dest;
};

typedef std::vector<Instruction> InstructionSeq;
struct CodeGenerator
{
    SymbolTable* scopedSymtab;
    SymbolTable* symtab;
    NodeAllocator* allocator;
    std::vector<AstNode*>* parseTrees;
    InstructionSeq code;
    uint64_t scope;
};

char* generate_code(CodeGenerator* gen);
void dispatcher(CodeGenerator* gen, AstNode* parseTree);
void write_to_file(const InstructionSeq& instructions, FILE* stream);
void write_label(const Instruction* inst, FILE* stream);
#endif