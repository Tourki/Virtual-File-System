/*
 * SuperBlock.cpp
 *
 *  Created on: May 5, 2017
 *      Author: tourki
 */

#include "SuperBlock.h"
#include "Extension.h"
void SuperBlock_Class::load_super(File *Myfile){
	assert(Myfile);//check the file pointer is not null
	Myfile->llseek(0,0);//as the super block at the beginning of the file
	Myfile->s_read(this,sizeof(SuperBlock_Class));//get the super block from the VFS
}

void SuperBlock_Class::sync_super(File *Myfile){
	assert(Myfile);//check the file pointer is not null
	Myfile->llseek(0,0);//as the super block at the beginning of the file
	Myfile->s_write(this,sizeof(SuperBlock_Class));//get the super block from the VFS
}



bool SuperBlock_Class::InodeNum_Address(unsigned int num,unsigned int &BlockNumber)	// returns the block in which the node was stored in case of extension
{
	assert(num>(inode_Numbers)-1);

	unsigned int count=num/(inode_Numbers);
	Extension *temp=(Extension*)(First_Extension);
	for(unsigned int i=1;i<count;i++)
		temp=temp->Next;
	if((temp->x[num%(inode_Numbers)].state)=='1'){
		BlockNumber=temp->x[num%(inode_Numbers)].Block_Number;
		return true;
	}
	else
		return false;

}


