/*
 * LinkedList.h
 *
 *  Created on: Mar 15, 2017
 *      Author: tourki
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <iostream>

template <class ListDataType>
class LinkedList{
private :
	struct node;
	typedef node* link; // typedef of the pointer to the structure node
	struct node	//node structure contian the data (element) and a pointer to the next node in the list
	{
		ListDataType element;
		link next;
	};
	link head,tail,current;
public :
	unsigned int count;
	LinkedList();
	~LinkedList();
	bool insert(const ListDataType &e);
	bool first(ListDataType &e);
	bool next(ListDataType &e);
	bool Delete(ListDataType &e);

};

template <class ListDataType>
LinkedList<ListDataType>::LinkedList()
{	count=0;
	head=NULL;
	tail=NULL;
	current=NULL;
}

template <class ListDataType>
LinkedList<ListDataType>::~LinkedList()
{	link target=head;
	link next;
	while(target!=NULL)
		{	next=target->next;
			delete target;
			target=next;
		}
}

/*This function is used to insert a new element to the list
 * arguments: e -> a reference to a constant data
 * return type : bool to indicate the success of the insert
 */
template <class ListDataType>
bool LinkedList<ListDataType>::insert(const ListDataType &e)
{	link NewNode=new node;//locating a new block in the heap
	if(NewNode==NULL) //asking if the allocation is a success
		return false;
	else
		{
			NewNode->element=e;	//inserting the data element to the node
			NewNode->next=NULL;//clearing the pointer of this new block
			if(head==NULL){//if the head pointer is empty then this is the first element
				head=NewNode;//this node is the head
				tail=NewNode;//and the tail at the same time as only one element of the list
			}
			else{
				tail->next=NewNode;//not the first element then link the old tail to the new node
				tail=NewNode;//make the new node the current tail
			}
			count++;
			return true;
		}
}

/*This function is used to read the first element in the list
 * arguments: e -> reference to the variable in which the data will be returned
 * return type : bool to indicate the success of the reading
 */
template <class ListDataType>
bool LinkedList<ListDataType>::first( ListDataType &e)
{	current=head;//as it is the first read it would be equal to the head
	if(current==NULL)
		return false;
	else
		{
			e=current->element;//get the data of the current node which is the head
	    	return true;
		}
}

/*This function is used to read the next element in the list
 * arguments: e -> reference to the variable in which the data will be returned
 * return type : bool to indicate the success of the reading
 */
template <class ListDataType>
bool LinkedList<ListDataType>::next(ListDataType &e)
{	if ((current==NULL || current==tail))
		return false;
	else
		{
		 	current=current->next;//increasing the current pointer to point to the next node
			e=current->element;//get the data of the current node
			return true;
		}
}



template <class ListDataType>
bool LinkedList<ListDataType>::Delete(ListDataType &e){

	if(head==tail&&head->element==e){
		delete head;
		head=tail=current=NULL;
		count--;
	}

	else{
		link pre=NULL,target=head;
		while(target!=NULL){
			if(pre==NULL&&target->element==e){//deleting the first element
				head=target->next;
				current=(current==target)?NULL:current;//current = new head if the current at the head
				delete target;count--;
				target=head;
			}
			else if(target==tail&&target->element==e){//deleting the tail element
				tail=pre;
				tail->next=NULL;
				current=(current==target)?pre:current;
				delete target;count--;
				target=pre->next;
			}
			else if(target->element==e){//element in the middle
				pre->next=target->next;
				current=(current==target)?pre:current;
				delete target;count--;
				target=pre->next;
			}
			else{//move cursors
				pre=target;
				target=pre->next;
			}
		}
	}

}

#endif /* LINKEDLIST_H_ */
