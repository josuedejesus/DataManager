#pragma once
#include "Node.h"
#include<iostream>
using std::cout;
using std::endl;

class Stack
{
private:
	Node* head;
	Node* tail;
	int size;
	void incrementSize();
	void decrementSize();
public:
	Stack();
	~Stack();
	void push(int);
	int pop();
	int getSize();
	void print();
	bool exists(int);
	int top();
	bool empty();
};
