#include "Scanner.h"
#include <fstream>
#include <cctype>
#include <algorithm>
#include <iostream>
#include <QMessageBox>
#include <QString>
using namespace std;

// Token class constructor definitions
Token::Token(string t, string v , int lineNumber) : type(t), value(v),lineNumber(lineNumber) {}
Token::Token() {}

// Scanner class constructor and method definitions
Scanner::Scanner() : inComment(false), openCommentLine(-1) {
    reservedWords = { "if", "then", "end", "repeat", "until", "read", "write", "else" };
}

bool Scanner::isDigit(char c) {
    return isdigit(c);
}

bool Scanner::isLetter(char c) {
    return isalpha(c);
}

bool Scanner::isSymbol(char c) {
    return c == ';' || c == '<' || c == '=' || c == '+' || c == '-' ||
        c == '*' || c == '/' || c == '(' || c == ')';
}

bool Scanner::isColon(char c) {
    return c == ':';
}

bool Scanner::isReservedWord(const string& word) {
    return find(reservedWords.begin(), reservedWords.end(), word) != reservedWords.end();
}

void Scanner::addToken(const string& type, const string& value, int lineNumber) {
    tokens.emplace_back(type, value, lineNumber);
}

void Scanner::handleError(const string& message, int lineNumber) {
    if(lineNumber == -9999)
    {
        throw ios_base::failure(message);
    }
    else
    {
        throw std::runtime_error("Error at line "+to_string(lineNumber)+" : "+message);
    }
}

void Scanner::processToken(const string& token, int lineNumber) {
    if (isReservedWord(token)) {
        string upperToken = token;
        transform(upperToken.begin(), upperToken.end(), upperToken.begin(), ::toupper);
        addToken(upperToken, token, lineNumber);
    }
    else {
        addToken("IDENTIFIER", token, lineNumber);
    }
}

void Scanner::processLine(const string& line, int lineNumber) {
    string token;
    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (inComment) {
            if (c == '}') {
                inComment = false;
            }
            continue;
        }
        if (c == '{') {
            inComment = true;
            openCommentLine = lineNumber;
            continue;
        }
        if (isLetter(c)) {
            token += c;
            while (i + 1 < line.size() && isLetter(line[i + 1])) {
                token += line[++i];
            }
            processToken(token, lineNumber);
            token.clear();
        }
        else if (isDigit(c)) {
            token += c;
            while (i + 1 < line.size() && isDigit(line[i + 1])) {
                token += line[++i];
            }
            addToken("NUMBER", token , lineNumber);
            token.clear();
        }
        else if (isSymbol(c)) {
            switch (c) {
            case '+': addToken("PLUS", "+", lineNumber); break;
            case '-': addToken("MINUS", "-", lineNumber); break;
            case '*': addToken("MULT", "*", lineNumber); break;
            case '/': addToken("DIV", "/", lineNumber); break;
            case '(': addToken("OPENBRACKET", "(", lineNumber); break;
            case ')': addToken("CLOSEDBRACKET", ")", lineNumber); break;
            case '<': addToken("LESSTHAN", "<", lineNumber); break;
            case '=': addToken("EQUAL", "=", lineNumber); break;
            case ';': addToken("SEMICOLON", ";", lineNumber); break;
            }
        }
        else if (isColon(c)) {
            if (i + 1 < line.size() && line[i + 1] == '=') {
                addToken("ASSIGN", ":=", lineNumber);
                ++i;
            }
            else {
                handleError("Expected '=' after ':'", lineNumber);
            }
        }
        else if (!isspace(c)) {
            handleError("Unknown symbol '" + string(1, c) + "'", lineNumber);
        }
    }
}

Token Scanner::getToken() {
    if (tokenIndex >= tokens.size())
    {
        tokenIndex = 0;
        return Token("EOF", "" , tokens.back().lineNumber); // To mark the end of string of tokens
    }
    Token token = tokens.at(tokenIndex++); // advance after accessing the token}
    return token;
}
void Scanner::scanFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        handleError("Unable to open file , Enter a valid file",-9999); //-9999 to mark file error not line error
        return;
    }

    string line;
    int lineNumber = 0;

    while (getline(file, line)) {
        lineNumber++;
        processLine(line, lineNumber);
    }

    if (inComment) {
        handleError("Unclosed comment starting at line " + to_string(openCommentLine), lineNumber);
    }

    file.close();
}

void Scanner::printTokens() {
    cout << "TokenValue  :  TokenType\n";
    for (const auto& token : tokens) {
        cout << token.value << "  :  " << token.type << "\n";
    }
}
Scanner::~Scanner() {
    tokens.clear();
    tokenIndex = 0;
    inComment = false;
    openCommentLine = -1;
}
