#include "Node.h"

Node::Node(int newValue) {
	this->value = newValue;
	this->next = nullptr;
}

Node::~Node() {
}