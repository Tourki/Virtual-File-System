/*
 * queue.h
 *
 *  Created on: May 18, 2017
 *      Author: tourki
 */

#ifndef QUEUE_H_
#define QUEUE_H_
#include <iostream>
#include <assert.h>
template < class queueElementType >
class Queue {
private:
	struct node;
	typedef node * link;
	struct node {
			queueElementType data;
			link next;
		};

	link head;
	link tail;
public:
	Queue();
	~Queue();
	bool enqueue(const queueElementType &e);
	queueElementType dequeue();
	bool front(queueElementType&e);
	bool isEmpty();
	bool DeleteTail();
	bool CopyQueue(Queue < queueElementType > &target);

};

template < class queueElementType >
Queue < queueElementType >::Queue():head(0),tail(0)
{}
template < class queueElementType >
Queue < queueElementType >::~Queue()
	{
		link target=head;
		link next;
		while(target!=NULL)
			{	next=target->next;
				delete target;
				target=next;
			}
		head=tail=NULL;
	}

template < class queueElementType >
bool Queue < queueElementType >::enqueue(const queueElementType &e)
{
	link NewNode=new node;
	NewNode->data=e;
	NewNode->next=0;
	assert(NewNode);
	if(head==0)//first element
		{
			head=tail=NewNode;
			return true;
		}
	else
		{	link pre=0,target=head;
			while(target!=0)
				{
					if(target->data==NewNode->data)
						return false;
					else if(target->data>NewNode->data)
					{
						if(pre==0) //if i am at the head
						{
							link temp=head;
							head=NewNode;
							NewNode->next=temp;
							return true;
						}
						else
						{
							pre->next=NewNode;
							NewNode->next=target;
							return true;
						}
					}
					else if(target->next !=0) //if this is not  the last element
						  {
							 pre=target;
							 target=target->next;
						  }
					else			//if this is the last node then add at the end
						{
							target->next=NewNode;
							target=NewNode->next;
							tail=NewNode;
							return true;
						}
				}
		}

}

template < class queueElementType >
queueElementType Queue < queueElementType >::dequeue()
{
	assert(head); // make sure queue is not empty
	link n(head);
	queueElementType frontElement(head->data);
	head = head->next;
	delete n;
	tail=head?tail:0;
	return frontElement;
}

template < class queueElementType >
bool Queue < queueElementType >::front(queueElementType&e)
{
	if(head) // make sure queue is not empty
	{
		e=head->data;
		return true;
	}
	else
		return false;
}

template < class queueElementType >
bool Queue < queueElementType >::isEmpty(){
		return bool(!head);
}

template < class queueElementType >
bool Queue < queueElementType >::DeleteTail(){
		if(isEmpty())
			return false;
		else if(head==tail){
			delete tail;
			head=NULL;
			tail=NULL;
			return true;
		}
		else{
			link tailPre=head;
			while(tailPre==tail){
				if(tailPre->next==tail){
					delete tail;
					tail=tailPre;
					return true;
				}
				else
					tailPre=tailPre->next;
			}
		}
}


template < class queueElementType >
bool Queue < queueElementType >::CopyQueue(Queue < queueElementType > &target){
	link temp=target.head;
	while(temp!=NULL){
		enqueue(temp->data);
		temp=temp->next;
	}
	return true;
}

#endif /* QUEUE_H_ */
