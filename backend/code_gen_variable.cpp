#include "code_gen_internal.hpp"
#include "code_gen_utils.hpp"
#include <string>
using namespace std;


OpDesc translateFunction(CodeGenerator *gen, AstNode *parseTree)
{
    gen->localSymtab = (SymbolTable*)parseTree->children[0]->data;
    fillTypeHwdInfoForBlock(gen->symtab);
    SymbolFunction* symFn = (SymbolFunction*)GET_SCOPED_SYM(gen, *parseTree->data);
    gen->cpu = generateCpuState(parseTree, gen->localSymtab, symFn);

    Instruction inst = generateFunctionLabel(parseTree);
    ADD_INST_MV(gen, inst);
    // function preambule
    ADD_INST(gen, {INSTRUCTION, "pushq", "%rbp", ""} );
    ADD_INST(gen, {INSTRUCTION, "movq", "%rsp", "%rbp"} );
    ADD_INST(gen, {INSTRUCTION, "subq", "", "%rsp"} );
    size_t fillSubInstInd = gen->code.size() - 1;

    AstNode* fnBody = parseTree->children[0];
    for (size_t i = 0; i < fnBody->children.size(); i++)
    {
        AstNode* parseTree = fnBody->children[i];
        dispatch(gen, parseTree);
    }
    gen->code[fillSubInstInd].src = '$' + to_string(gen->cpu->maxStackSize);
    ADD_INST(gen, {INSTRUCTION, "leave"} );
    ADD_INST(gen, {INSTRUCTION, "ret"} );

    gen->localSymtab = gen->localSymtab->parent;
    return {OP::NONE};
}

OpDesc translateDeclaration(CodeGenerator *gen, AstNode *parseTree)
{
    if(gen->localSymtab->scopeLevel == 0)
    {
        return translateGlobalInit(gen, parseTree->children[0]);
    }
    else
    {
        return translateLocalInit(gen, parseTree->children[0]);
    }
    return {OP::NONE};
}



OpDesc prepareConstant(CodeGenerator *gen, AstNode *parseTree)
{
    OpDesc opDesc = {
        .operandType =  OP::CONSTANT,
        .operand =  *parseTree->data,
        .operandAffi = 0,
        .scope = 0
    };
    return opDesc;
}

OpDesc prepareVariable(CodeGenerator *gen, AstNode *parseTree)
{
    return parseEncodedAccess(gen,  *parseTree->data);
}

OpDesc writeConstantToSym(CodeGenerator *gen, std::string constant, const std::string& dest)
{
    OpDesc destDesc = parseEncodedAccess(gen, dest);
    uint8_t gr = getTypeGr(destDesc.operandAffi);

    if(gr == SIGNED_INT_GROUP || gr == UNSIGNED_INT_GROUP)
    {
        moveConstantInt(gen, constant, destDesc);
    }
    else if (gr == FLOAT_GROUP)
    {
        moveConstantFloat(gen, constant, destDesc);
    }
    else
    {
        printf("Internal Error: Unsupported group\n");
        exit(-1);
    }
    return {OP::NONE};
}

OpDesc processChild(CodeGenerator *gen, AstNode *parseTree, std::size_t child_index)
{
    if( parseTree->children[child_index]->nodeType == NodeType::IDENTIFIER )
    {
        OpDesc varDesc = prepareVariable(gen, parseTree->children[child_index]);
        VariableCpuDesc var = fetchVariable(gen->cpu, varDesc.operand);
        if(var.storageType != Storage::REG)
        {
            SymbolType* symType = (SymbolType*)GET_SCOPED_SYM(gen, *parseTree->type);
            loadVariableToReg(gen, varDesc, symType->affiliation);
        }

        return varDesc;
    }
    return dispatch(gen, parseTree->children[child_index]);
}


OpDesc translateGlobalInit(CodeGenerator *gen, AstNode *parseTree)
{
    SymbolVariable* symVar = (SymbolVariable*)GET_SCOPED_SYM(gen, *parseTree->data);
    SymbolType* symType = (SymbolType*)GET_SCOPED_SYM(gen, *symVar->varType);
    Instruction inst;
    inst.type = LABEL;
    inst.mnemonic = std::move(*parseTree->data);
    if(parseTree->children.size() == 0)
    {
        zeroInitVariable(&inst, symType, inst.mnemonic);
    }
    ADD_INST_MV(gen, inst);
    FREE_NODE_REC(gen, parseTree);
    return {OP::NONE};
}

OpDesc translateLocalInit(CodeGenerator *gen, AstNode *parseTree)
{
    OpDesc opDesc = dispatch(gen, parseTree->children[0]->children[0]);
    writeToLocalVariable(gen, *parseTree->data, opDesc);
    return {OP::NONE};
}


OpDesc writeToLocalVariable(CodeGenerator *gen, const std::string &varname, OpDesc operandDesc)
{
    if(operandDesc.operandType == OP::CONSTANT)
    {
        writeConstantToSym(gen, operandDesc.operand, varname);
    }
    else
    {
        printf("Non constant assignment not supported \n");
        exit(-1);
    }
    return {OP::NONE};
}
