#pragma once
#ifndef SCANNER_H
#define SCANNER_H
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>

using namespace std;

class Token {
public:
    string type;
    string value;
    int lineNumber; //This member is only added for error message that will be implemented in Parser
    Token(string t, string v,int lineNumber);
    Token();
};

class Scanner {
private:
    vector<string> reservedWords = { "if", "then", "end", "repeat", "until", "read", "write" };
    vector<Token> tokens;
    unsigned int tokenIndex = 0;
    bool inComment = false;
    int openCommentLine = -1;

    bool isDigit(char c);
    bool isLetter(char c);
    bool isSymbol(char c);
    bool isColon(char c);
    bool isReservedWord(const string& word);

    void addToken(const string& type, const string& value , int lineNumber);
    void handleError(const string& message, int lineNumber);
    void processToken(const string& token, int lineNumber);
    void processLine(const string& line, int lineNumber);

public:
    Scanner();
    Token getToken();
    void scanFile(const string& filename);
    void printTokens();
    ~Scanner();
};
#endif
