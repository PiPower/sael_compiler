#ifndef CODE_GEN_UTILS
#define CODE_GEN_UTILS
#include "../symbol_table.hpp"
#include "../frontend/parser.hpp"
#include "code_gen.hpp"

// alignment is first power of 2 that is <= type_size
uint32_t getTypeAlignment(SymbolType* symType);
std::string encodeAsAsmData(AstNode* dataNode);
Instruction generateFunctionLabel(AstNode* fnDef);
void zeroInitVariable(Instruction* inst, SymbolType* symType, const std::string symName);
#endif