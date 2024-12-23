#ifndef PARSER_UTILS_H
#define PARSER_UTILS_H
#include "scanner.hpp"
#include "../symbol_table.hpp"
// math ops  
//   AMPRESAND, STAR, PLUS, MINUS, TILDE, BANG, SLASH, 
//   PERCENT, LESS, GREATER,  L_SHIFT,
//    R_SHIFT, LESS_EQUAL, GREATER_EQUAL, EQUAL_EQUAL, BANG_EQUAL,
//   PIPE, CARET, DOUBLE_AMPRESAND, DOUBLE_PIPE, QUESTION_MARK,
//    PLUS_PLUS, MINUS_MINUS,

enum class NodeType
{
    NONE, 
    IDENTIFIER,
    CONSTANT,
    DECLARATION,
    CONDITIONAL_EXPRESSION,
//basic math ops
    MULTIPLY, DIVIDE, DIVIDE_MODULO,
    ADD, SUBTRACT, L_SHIFT, R_SHIFT,
    AND, EXC_OR, OR, COMPLIMENT,
// prefix ops
    PRE_INC, POST_INC, PRE_DEC, POST_DEC,
//logical ops    
    LOG_AND, LOG_OR, LOG_NOT,
// comparisons
    LESS, GREATER, LESS_EQUAL, 
    GREATER_EQUAL, EQUAL, NOT_EQUAL,
// assignment nodes 
    ASSIGNMENT, MUL_ASSIGNMENT, DIV_ASSIGNMENT, MOD_ASSIGNMENT,
    ADD_ASSIGNMENT, SUB_ASSIGNMENT, L_SHIFT_ASSIGNMENT, R_SHIFT_ASSIGNMENT,
    AND_ASSIGNMENT, EXC_OR_ASSIGNMENT, OR_ASSIGNMENT,
// misc
   NODE_EMPTY, DECLARATION_LIST, INIT_DECLARATOR
};

struct ParserState;
struct AstNode;


typedef AstNode* ( *parseFunctionPtr)(ParserState* parser);

void triggerParserError(ParserState* parser,
                         int value, 
                         const char* format, ...);
AstNode* parseLoop(ParserState* parser, 
                    parseFunctionPtr parsingFunction,
                    AstNode* root, 
                    const std::vector<TokenType>& types);
AstNode* parseLoop(ParserState* parser, 
                    parseFunctionPtr parsingFunction,
                    AstNode* root, 
                    const TokenType* types,
                    const uint64_t typesCount);
NodeType tokenMathTypeToNodeType(const Token& token);

static inline NodeType assignementTokenToNodeType(const Token& token)
{
    return (NodeType) ((int)token.type - (int)TokenType::EQUAL + (int)NodeType::ASSIGNMENT) ;
}

SymbolVariable* addSymbolVariableToSymtab(ParserState* parser, const std::string& symName);
#endif