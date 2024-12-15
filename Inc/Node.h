#include "Scanner.h"
class Node
{
    string nodeType; //ex ; if-stmt , repeat-stmt , ....-stmt or expression
    string nodeVal; //ex: identifier X , number 345 etc..
    Node* leftChild;
    Node* rightChild;
    Node* elseChild;//For if-stmt only
    Node* siblingNode; //if any
public:
    Node(string type, string value)
    {
        nodeType = type;
        nodeVal = value;
        leftChild = nullptr;
        rightChild = nullptr;
        elseChild = nullptr;
        siblingNode = nullptr;
    }
    string getNodeType();
    string getVal();
    Node* getLeftChild();
    Node* getRightChild();
    Node* getElseChild();//in case of if-stmt
    Node* getSibling();
    void makeLeftChild(Node* child);
    void makeRightChild(Node* child);
    void makeElseChild(Node* child);
    void makeSiblingNode(Node* sibling);
    Node* makeOpNode(Token op, Node* LeftOperand, Node* RightOpernd);
    ~Node();

};
