#ifndef PARSER_H
#define PARSER_H
#include <iostream>

#include "Scanner.h"
#include "Node.h"

using namespace std;

class Parser
{
	Node* syntaxTree; //The root node of the syntax tree
    Token lookahead;
public:
    Scanner scanner;
	Parser(Scanner& obj);
	void match(string expectedToken);
	Node* Parse();
	Node* parseStatement_Sequence();
	Node* parseStatement();
	Node* parseIf_Statement();
	Node* parseRepeat_Statement();
	Node* parseAssign_Statement();
	Node* parseRead_Statement();
	Node* parseWrite_Statement();
	Node* parseExp();
	Node* parseSimple_Exp();
	Node* parseTerm();
	Node* parseFactor();
    ~Parser();
};
#endif
