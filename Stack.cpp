#include "Stack.h"

Stack::Stack() {
	this->size = 0;
	this->head = nullptr;
	this->tail = nullptr;
}

Stack::~Stack() {
}

void Stack::push(int newValue) {
	this->incrementSize();
	Node* newNode = new Node(newValue);
	if (this->tail == nullptr) {
		this->tail = newNode;
		return;
	}
	newNode->next = this->tail;
	this->tail = newNode;
}

int Stack::pop() {
	if (this->size == 0) {
		return 0;
	}
	if (this->size == 1) {
		int valuetoReturn = this->tail->value;
		this->tail = nullptr;
		this->head = nullptr;
		this->decrementSize();
		return valuetoReturn;
	}
	int valueToReturn = tail->value;;
	this->tail = this->tail->next;
	this->decrementSize();
	return valueToReturn;
}

int Stack::top() {
	return this->tail->value;
}

bool Stack::empty() {
	bool empty = true;
	if (this->size != 0)
		empty = false;
	return empty;
}

void Stack::print()
{
	Node* iteratorNode = this->tail;
	cout << "null ";
    while (iteratorNode != nullptr) {
        cout << " <- " <<iteratorNode->value;
        iteratorNode = iteratorNode->next;
    }
	cout << endl;
}

int Stack::getSize() {
	return this->size;
}

void Stack::incrementSize() {
	this->size = this->size + 1;
}

void Stack::decrementSize() {
	this->size = this->size - 1;
}

bool Stack::exists(int value) {
	bool exists = false;
	if (this->tail->value == value) {
		exists = true;
	}
	Node* iteratorNode = this->tail;
	while (iteratorNode->next != nullptr) {
		if (iteratorNode->next->value == value) {
			exists = true;
			break;
		}
		iteratorNode = iteratorNode->next;
	}
	if (exists)
		return true;
	else
		return false;
}
