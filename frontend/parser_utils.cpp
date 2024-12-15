#include "parser_utils.hpp"
#include "parser.hpp"
#include "parser_internal.hpp"
#include <stdarg.h>
#include <string.h>
using namespace std;
AstNode* parseLoop(ParserState* parser, 
                    parseFunctionPtr parsingFunction, 
                    AstNode* root, 
                    const vector<TokenType>& types)
{
   return parseLoop(parser, parsingFunction, root, types.data(), types.size());
}

AstNode *parseLoop(ParserState *parser,
                    parseFunctionPtr parsingFunction, 
                    AstNode *root, 
                    const TokenType *types, 
                    const uint64_t typesCount)
{
    while (parser->scanner->currentTokenOneOf(types, typesCount))
    {
        Token operatorToken = parser->scanner->getToken();
        AstNode* parent = ALLOCATE_NODE(parser);
        parent->nodeType = tokenMathTypeToNodeType(operatorToken);
        AstNode* right = parsingFunction(parser);

        parent->children.push_back(root);
        parent->children.push_back(right);
        root = parent;
    }

    return root;
}

void triggerParserError(ParserState* parser, int value, const char* format, ...)
{
    parser->errCode = 1;

    va_list args;
    va_start(args, format);
    int len = vsnprintf(parser->errorMessage, parser->errorMessageLen, format, args);
    va_end(args);
    if(len <  0)
    {
        printf("Error buffer is too small \n");
    }

    snprintf(parser->errorMessage + len, parser->errorMessageLen - len, 
    "Error ocurred in line %u\n", parser->scanner->line);


    longjmp(parser->jmpBuff, value);
}

NodeType tokenMathTypeToNodeType(const Token& token)
{
    switch (token.type)
    {
    case TokenType::PLUS :
        return NodeType::ADD;
    case TokenType::MINUS :
        return NodeType::SUBTRACT;
    case TokenType::SLASH :
        return NodeType::DIVIDE;
    case TokenType::PERCENT :
        return NodeType::DIVIDE_MODULO;
    case TokenType::STAR :
        return NodeType::MULTIPLY;
    case TokenType::L_SHIFT :
        return NodeType::L_SHIFT;
    case TokenType::R_SHIFT :
        return NodeType::R_SHIFT;
    case TokenType::LESS :
        return NodeType::LESS;
    case TokenType::GREATER :
        return NodeType::GREATER;
    case TokenType::LESS_EQUAL :
        return NodeType::LESS_EQUAL;
    case TokenType::GREATER_EQUAL :
        return NodeType::GREATER_EQUAL;
    case TokenType::EQUAL_EQUAL :
        return NodeType::EQUAL;
    case TokenType::BANG_EQUAL :
        return NodeType::NOT_EQUAL;
    case TokenType::PIPE :
        return NodeType::OR;
    case TokenType::AMPRESAND :
        return NodeType::AND;
    case TokenType::CARET :
        return NodeType::EXC_OR;
    case TokenType::DOUBLE_AMPRESAND :
        return NodeType::LOG_AND;
    case TokenType::DOUBLE_PIPE :
        return NodeType::LOG_OR;
    case TokenType::COMMA:
        return NodeType::EXPR_GLUE;
    }

    fprintf(stdout, "unexpected token at line %d \n", token.line);
    exit(-1);
}