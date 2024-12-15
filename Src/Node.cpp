#include <iostream>
#include "Node.h"
using namespace std;


Node::~Node() {
    delete leftChild;
    delete rightChild;
    delete elseChild;
    delete siblingNode;
}
string Node::getNodeType()
{
    return nodeType;
}
string Node::getVal()
{
    return nodeVal;
}
Node* Node::getLeftChild()
{
    return leftChild;
}
Node* Node::getRightChild()
{
    return rightChild;
}
Node* Node::getElseChild()
{
    return elseChild;
}
Node* Node::getSibling()
{
    return siblingNode;
}
void Node::makeLeftChild(Node* child)
{
	if (nullptr == child)
	{
		std::cerr << "ERROR: Child is null!" << std::endl;
		exit(1);
	}
	else if (leftChild) //if there exist.
	{
		delete leftChild;
		leftChild = child;
	}
	else
	{
		leftChild = child;
	}
}
void Node::makeRightChild(Node* child)
{
	if (nullptr == child)
	{
		std::cerr << "ERROR: Child is null!" << std::endl;
		exit(1);
	}
	else if (rightChild) //if there exist.
	{
		delete rightChild;
		rightChild = child;
	}
	else
	{
		rightChild = child;
	}
}
void Node::makeElseChild(Node* child)
{
	if (nullptr == child)
	{
		std::cerr << "ERROR: Child is null!" << std::endl;
		exit(1);
	}
	else if (elseChild) //if there exist.
	{
		delete elseChild;
		elseChild = child;
	}
	else
	{
		elseChild = child;
	}
}
void Node::makeSiblingNode(Node* sibling)
{
	if (sibling == nullptr)
	{
		std::cerr << "ERROR: sibling node is null!" << std::endl;
		exit(1); // Or throw an exception if needed
	}

	// Traverse to the last sibling in the linked list of siblings
	Node* current = this;
	while (current->siblingNode != nullptr) {
		current = current->siblingNode;  // Move to the next sibling
	}

	// Now, link the new sibling
	current->siblingNode = sibling;
}
Node* Node::makeOpNode(Token op, Node* LeftOperand, Node* RightOpernd)
{
	Node *root = new Node("Expression", op.value);
	makeLeftChild(LeftOperand);
	makeRightChild(RightOpernd);
	return root;
}
