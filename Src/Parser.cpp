#include <iostream>
#include "Parser.h"
#include "Scanner.h"

// Constructor that initializes the syntax tree and scanner, and stores the first token
Parser::Parser(Scanner& obj)
{
    syntaxTree = nullptr;
    scanner = obj;
    lookahead = scanner.getToken(); // Store the first token
}

// Method to match the expected token and advance to the next token
void Parser::match(string expectedToken)
{
    if (lookahead.type == expectedToken)
    {
        lookahead = scanner.getToken();
    }
    else
    {
        throw std::runtime_error("Syntax error : Unmatched token ` " + expectedToken + "`" + " at Line -> " + to_string(lookahead.lineNumber));
    }
}

// The main parsing function that begins parsing the statement sequence
Node* Parser::Parse()
{
    syntaxTree = parseStatement_Sequence();
    if(lookahead.type != "EOF")
    {
        throw runtime_error("Syntax eerror : Unmatched token ` " + lookahead.value + "`" + " at Line -> " + to_string(lookahead.lineNumber));
    }
    return syntaxTree;
}

// Parse a sequence of statements
Node* Parser::parseStatement_Sequence()
{
    Node* firstStatement = parseStatement();
    Node* prevStatement = firstStatement;

    while (lookahead.type == "SEMICOLON")
    {
        match("SEMICOLON");
        Node* currentStatement = parseStatement();
        prevStatement->makeSiblingNode(currentStatement);
        prevStatement = currentStatement;
    }
    return firstStatement;
}

// Parse a single statement
Node* Parser::parseStatement()
{
    if (lookahead.type == "IF")
    {
        return parseIf_Statement();
    }
    else if (lookahead.type == "REPEAT")
    {
        return parseRepeat_Statement();
    }
    else if (lookahead.type == "IDENTIFIER")
    {
        return parseAssign_Statement();
    }
    else if (lookahead.type == "READ")
    {
        return parseRead_Statement();
    }
    else if (lookahead.type == "WRITE")
    {
        return parseWrite_Statement();
    }
    else
    {
        throw std::runtime_error("Syntax error : Unmatched token ` " + lookahead.value + "`" + " at Line -> " + to_string(lookahead.lineNumber));
    }
}

// Parse an 'if' statement
Node* Parser::parseIf_Statement()
{
    Node* IfNode = new Node(lookahead.type, "if");
    match("IF");

    Node* testChild = parseExp();
    match("THEN");
    Node* thenChild = parseStatement_Sequence();

    if (lookahead.type == "ELSE")
    {
        match("ELSE");
        Node* elseChild = parseStatement_Sequence();
        IfNode->makeElseChild(elseChild);
    }

    match("END");

    IfNode->makeLeftChild(testChild);
    IfNode->makeRightChild(thenChild);
    return IfNode;
}

// Parse a 'repeat' statement
Node* Parser::parseRepeat_Statement()
{
    Node* repeatNode = new Node(lookahead.type, "repeat\n");
    match("REPEAT");

    Node* bodyNode = parseStatement_Sequence();
    match("UNTIL");

    Node* testNode = parseExp();
    repeatNode->makeLeftChild(bodyNode);
    repeatNode->makeRightChild(testNode);

    return repeatNode;
}

// Parse an assignment statement
Node* Parser::parseAssign_Statement()
{
    Node* stmtNode = new Node("ASSIGN", "assign\n (" + lookahead.value+")");
    match("IDENTIFIER");
    match("ASSIGN");

    Node* node = parseExp();
    stmtNode->makeRightChild(node);

    return stmtNode;
}

// Parse a read statement
Node* Parser::parseRead_Statement()
{
    match("READ");
    string identiferval = lookahead.value;
    match("IDENTIFIER");

    Node* readNode = new Node("READ", "read \n ("+identiferval+")");
    // Read node doesn't have children
    return readNode;
}

// Parse a write statement
Node* Parser::parseWrite_Statement()
{
    match("WRITE");
    Node* node = parseExp();

    Node* writeNode = new Node("WRITE", "write");
    writeNode->makeRightChild(node);

    return writeNode;
}

// Parse an expression, which may include comparison operators
Node* Parser::parseExp()
{
    Node* node = parseSimple_Exp();

    if (lookahead.type == "LESSTHAN" || lookahead.type == "EQUAL")
    {
        Node* opNode = new Node(lookahead.type, "Op\n("+lookahead.value+")");
        match(lookahead.type);
        opNode->makeLeftChild(node);

        Node* rightNode = parseSimple_Exp();
        opNode->makeRightChild(rightNode);

        node = opNode;
    }

    return node;
}

// Parse a simple expression (handles addition and subtraction)
Node* Parser::parseSimple_Exp()
{
    Node* node = parseTerm();

    while (lookahead.type == "PLUS" || lookahead.type == "MINUS")
    {
        Node* opNode = new Node(lookahead.type, "Op\n("+lookahead.value+")");
        match(lookahead.type);
        opNode->makeLeftChild(node);

        Node* rightNode = parseTerm();
        opNode->makeRightChild(rightNode);

        node = opNode;
    }

    return node;
}

// Parse a term
Node* Parser::parseTerm()
{
    Node* node = parseFactor();

    while (lookahead.type == "MULT" || lookahead.type == "DIV")
    {
        Node* opNode = new Node(lookahead.type, "Op\n("+lookahead.value+")");
        match(lookahead.type);

        opNode->makeLeftChild(node);

        Node* rightNode = parseFactor();
        opNode->makeRightChild(rightNode);

        node = opNode;
    }

    return node;
}

// Parse a factor
Node* Parser::parseFactor()
{
    Node* node;
    if (lookahead.type == "OPENBRACKET")
    {
        match("OPENBRACKET");
        node = parseExp();
        match("CLOSEDBRACKET");
    }
    else if (lookahead.type == "NUMBER")
    {
        node = new Node(lookahead.type, "Const\n("+lookahead.value+")");
        match("NUMBER");
    }
    else if (lookahead.type == "IDENTIFIER")
    {
        node = new Node(lookahead.type, "ID\n("+lookahead.value+")");
        match("IDENTIFIER");
    }
    else
    {
        throw std::runtime_error("Syntax error : Unmatched token ` " + lookahead.value + "`" + " at Line -> " + to_string(lookahead.lineNumber));
    }
    return node;
}
Parser::~Parser() {
    delete syntaxTree;
}
