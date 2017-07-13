/*
 * Extension.cpp
 *
 *  Created on: Jul 2, 2017
 *      Author: amr
 */
/*Notes : datablock size must be greater than or equal the size of inode class to prevent storing the node in many blocks
 */
#include "Extension.h"
#include <math.h>
#include"Inode.h"


//------------------------------------------------------------------------------
void Extension::Sync(File* myfile,SuperBlock_Class* sb,FBL_Class *FblPtr,unsigned int *Blocks){

	char *X=(char*)x;
	char *data=new char[(sb->Extension_NumOfBlocks)*(sb->Data_blocksize)];

	copy(X ,X+sizeof(Point)*(sb->inode_Numbers) ,data);			//

	unsigned int* P=new unsigned int;
	P[0]=NextBlocks;

	copy(P,P+sizeof(unsigned int),data+sizeof(Point)*(sb->inode_Numbers));

	P=new unsigned int ;
	P[0]=Extension_Number;

	copy(P,P+sizeof(unsigned int),data+sizeof(Point)*(sb->inode_Numbers)+sizeof(unsigned int));
	myfile->as_write(data,sizeof(Point)*(sb->inode_Numbers)+2*sizeof(unsigned int),Blocks,sb->Extension_NumOfBlocks);

	delete P;
	//	delete []data;
}
//----------------------------------------------------------------------------------------------------
void Extension::Load(File* myfile,SuperBlock_Class* sb,unsigned int *Blocks){

	Point *buf=new Point[sb->inode_Numbers];
	char *buf1=(char*)buf;
	//char *data=new char[(sb->Extension_NumOfBlocks)*(sb->Data_blocksize)];
	char*data;
	myfile->as_read(data,Blocks,(sb->Extension_NumOfBlocks));

	copy(data,data+sizeof(Point)*(sb->inode_Numbers) ,buf1);
	delete[]x;
	x=(Point*)buf1;

	char* buf2=new char[sizeof(unsigned int)];

	copy(data+sizeof(Point)*(sb->inode_Numbers),data+sizeof(Point)*(sb->inode_Numbers)+sizeof(unsigned int) ,buf2);
	unsigned int* tmp=(unsigned int*)buf2;
	NextBlocks=tmp[0];

	copy(data+sizeof(Point)*(sb->inode_Numbers)+sizeof(unsigned int) ,data+sizeof(Point)*(sb->inode_Numbers)+2*sizeof(unsigned int) ,buf2);
	tmp=(unsigned int*)buf2;;
	Extension_Number=tmp[0];

	delete[] data;
	delete tmp;
}

Extension::Extension(SuperBlock_Class *sb) {
	//EndingOffset=0;

	Next=0;
	NextBlocks=0; // also does not matter , we can handle it from the extension numbers
	Extension_Number=0;
	x=new Point[sb->inode_Numbers];

	for(unsigned int i=0;i<(sb->inode_Numbers);i++)
	{
		x[i].state='0';
		x[i].Block_Number=0;	// it doesn't matter if it starts with zero as the state identifies if the inode number is occupied
	}
}
//----------------------------------------------------------------------------------------------------------
bool Extension::get_inode_number(unsigned int &num, SuperBlock_Class *sb,FBL_Class *FblPtr, File *myfile ){
	assert((sb->Data_blocksize)>sizeof(inode_Class));
	unsigned int NumOfNodes=(sb->Data_blocksize)/sizeof(inode_Class);	// check if 0,number of nodes in one block , if num of nodes =0 then a single node needs many blocks


	Extension *temp=(Extension*)(sb->First_Extension);
	while(temp!=this){temp=temp->Next;	 }

	for(unsigned int i=0;i<(sb->inode_Numbers);i++)
	{
		if(x[i].state=='0')
		{
			unsigned int BlockNumber;
			num=i+Extension_Number*(sb->inode_Numbers);
			bool flag=SearchForBlock(sb,num,NumOfNodes,BlockNumber);
			if (flag) {	x[i].Block_Number=BlockNumber; (sb->Current_inodesNum)++; 	x[i].state='1'; 		return true;   }



			//--------------------------------------------------- get a new block

			unsigned int*temp=FblPtr->get_blocks(1);
			assert(temp);
			x[i].Block_Number=temp[0];

			(sb->Current_inodesNum)++;
			x[i].state='1';
			return true;
		}
	}
	return false;
}

bool Extension::DeleteBlock(FBL_Class *FblPtr,SuperBlock_Class *sb,unsigned int num){
	unsigned int NumOfNodes=(sb->Data_blocksize)/sizeof(inode_Class);
	unsigned int FirstToCheck=((num-sb->inode_Numbers)/NumOfNodes)*NumOfNodes+sb->inode_Numbers;
	for(unsigned int i=FirstToCheck;i<FirstToCheck+NumOfNodes;i++){
		unsigned int BlockNumber;
		if(i==num)	continue;
		bool flag;
		flag=sb->InodeNum_Address(i,BlockNumber);
		if(!flag) continue;
		else return false;
	}
	FblPtr->delete_file(this->x[num%(sb->inode_Numbers)].Block_Number,NumOfNodes);
	return true;
}

bool Extension::SearchForBlock(SuperBlock_Class *sb,unsigned int num,unsigned int NumOfNodes,unsigned int &BlockNumber){
	unsigned int FirstToCheck=((num-sb->inode_Numbers)/NumOfNodes)*NumOfNodes+sb->inode_Numbers;
	for(unsigned int i=FirstToCheck;i<FirstToCheck+NumOfNodes;i++){
		if(i==num)	continue;
		bool flag;
		flag=sb->InodeNum_Address(FirstToCheck,BlockNumber);
		if(flag) return true;
	}
	return false;

}

void  Extension::Delete_inode(SuperBlock_Class* sb,unsigned int num,FBL_Class *FblPtr){
	unsigned int NumOfNodes=(sb->Data_blocksize)/sizeof(inode_Class);
	x[num].state='0';
	FblPtr->delete_file(x[num].Block_Number,NumOfNodes);
	x[num].Block_Number=0;
	(sb->Current_inodesNum)--;
}

/*
unsigned int Extension::InodeNum_Address(unsigned int num){
return x[num].Block_Number;
}
 */

/*Extension::~Extension() {
	// TODO Auto-generated destructor stub
}*/

