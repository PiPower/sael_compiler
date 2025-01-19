#include "code_gen_internal.hpp"
#include "code_gen_utils.hpp"
using namespace std;

void translateExpr(CodeGenerator *gen, AstNode *parseTree)
{
    switch (parseTree->nodeType)
    {
    case NodeType::MINUS:
        translateNegation(gen, parseTree);
        break;
    }
}

void translateNegation(CodeGenerator *gen, AstNode *parseTree)
{
    CLEAR_OP(gen);
    dispatch(gen, parseTree->children[0]);
    if(gen->opDesc.op == OP::CONSTANT)
    {
        gen->opDesc.operand.insert(0, 1, '-');
    }
}

void generateCodeForSICA(CodeGenerator *gen, const std::string &constant, const OpDesc &destDesc)
{
    //first process value to be sure it is in correct range
    long int value = encodeAsSignedBin(constant);
    Instruction inst;
    inst.type = INSTRUCTION;
    if(destDesc.operandAffi == INT8_S)
    {
        inst.mnemonic = "movb";
        inst.src = encodeIntAsString(value, 1);
    }
    else if(destDesc.operandAffi  == INT16_S)
    {
        inst.mnemonic = "movw";
        inst.src = encodeIntAsString(value, 2);
    }
    else if(destDesc.operandAffi  == INT32_S)
    {
        inst.mnemonic = "movl";
        inst.src = encodeIntAsString(value, 4);
    }
    else if(destDesc.operandAffi  == INT64_S)
    {
        inst.mnemonic = "movq";
        inst.src = encodeIntAsString(value, 8);
    }

    if(destDesc.scope > 0)
    {
        switch( gen->cpu->data[destDesc.operand].storageType)
        {
        case Storage::REG:
            printf("Internal Error: storage type error \n");
            exit(-1);
            break;
        case Storage::STACK:
            inst.dest += to_string(gen->cpu->data[destDesc.operand].offset) +
                        '('  + '%'+ cpu_registers_str[RBP][0] + ')';
            break;
        default:
            printf("Internal Error: storage type error \n");
            exit(-1);
            break;
        }
    }
    ADD_INST_MV(gen, inst);
}
