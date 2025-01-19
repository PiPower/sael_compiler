#ifndef CODE_GEN_UTILS
#define CODE_GEN_UTILS
#include "../symbol_table.hpp"
#include "../frontend/parser.hpp"
#include "code_gen.hpp"

#define CLEAR_OP(gen) (gen)->opDesc.op = OP::NONE;  (gen)->opDesc.operand.clear();
// alignment is first power of 2 that is <= type_size
uint32_t getTypeAlignment(SymbolType* symType);
std::string encodeAsAsmData(AstNode* dataNode);
Instruction generateFunctionLabel(AstNode* fnDef);
void zeroInitVariable(Instruction* inst, SymbolType* symType, const std::string symName);
uint8_t getAffiliationIndex(uint16_t typeGroup);
uint8_t getTypeGroup(uint16_t affiliation);

#endif