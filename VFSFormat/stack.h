/*
 * stack.h
 *
 *  Created on: Apr 30, 2017
 *      Author: tourki
 */

#ifndef STACK_H_
#define STACK_H_
#include <iostream>
template <class StackElementType>
class Stack {
public:
	Stack();
	~Stack();
	bool push(const StackElementType& e);
	bool top(StackElementType &e);
	bool pop(StackElementType &e);
	bool isEmpty();
	int Counter;
private:
	struct node;
	typedef node * link;
	struct node {
		StackElementType element;
		link next;
	};
	link head;
};

template <class StackElementType>
Stack<StackElementType>::Stack(){
	head=NULL;
	Counter=0;
}

template <class StackElementType>
Stack<StackElementType>::~Stack(){
	link target=head,next;
	while(target!=NULL){
		next=target->next;
		delete target;
		target=next;
	}
}

template <class StackElementType>
bool Stack<StackElementType>::push(const StackElementType &e){
	link NewNode = new node;
	if(NewNode==NULL)
		return false;
	NewNode->element=e;
	NewNode->next=head;
	head=NewNode;
	Counter++;
	return true;
}

template <class StackElementType>
bool Stack<StackElementType>::top(StackElementType &e){
	if(head==NULL)
		return false;
	e=head->element;
	return true;
}

template <class StackElementType>
bool Stack<StackElementType>::pop(StackElementType &e){
	if(head==NULL)
		return false;
	e=head->element;
	head=head->next;
	Counter--;
	return true;
}


template <class StackElementType>
bool Stack<StackElementType>::isEmpty(){
	if(head==NULL)
		return true;
	return false;
}

#endif /* STACK_H_ */
