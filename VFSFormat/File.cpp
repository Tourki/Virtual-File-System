/*
 * FIle.cpp
 *
 *  Created on: May 8, 2017
 *      Author: tourki
 */
#include "File.h"
#include "Inode.h"
#include "FBL.h"
#include "FIT.h"
#include "SuperBlock.h"
#include "LinkedList.h"
#include <math.h>

File::File(const char * file_name2){
	fd= open(file_name2,O_RDWR,0600);
	assert(fd > 0);
	SuperBlock=new SuperBlock_Class;
	SuperBlock->load_super(this);
	FBL=new FBL_Class(SuperBlock,this);
	FBL->load_FBL();
	FIT=new FIT_Class(SuperBlock,FBL);
	FIT->load_FIT(this,SuperBlock);
}
File::~File(){
	close(fd);
	delete SuperBlock;
	delete FBL;
	delete FIT;

}

bool File::as_read(inode_Class * targetInode,void* &buffer1){
	char*buffer=new char[SuperBlock->Data_blocksize*targetInode->Number_Blocks];
	assert(buffer);
	char*moving_buffer=(char*)buffer; //to keep appending on the main buffer
	unsigned int * start=FBL->get_sequence(targetInode);
	unsigned int *origin=start;//start of the blocks array
	unsigned int * next;
	while(moving_buffer<(buffer+SuperBlock->Data_blocksize*targetInode->Number_Blocks))//while moving buffer didn't reach the end
		{	next=start;//at the beginning of the sequence next = start
			while(next[1]-next[0]==1)//while the difference of the block number and the next one is one then next to read block sequence on time
				next++;//move the next pointer to the next block number to check
			lseek(fd,SuperBlock->DataBlocks_Start+start[0]*SuperBlock->Data_blocksize,SEEK_SET);// the next is out of sequence then seek to the start block number
			moving_buffer+=read(fd,moving_buffer,(next[0]-start[0]+1)*SuperBlock->Data_blocksize);//read the data and input it at the position of the moving buffer then move the moving buffer by the number of the data read
			start=&next[1]; // the new start is the new next
		}
	delete[]origin;
	buffer1=buffer;
	return true;
}
bool File::as_write(inode_Class * targetInode,void * buffer1,unsigned int inputLength){
	assert(buffer1);
	char*buffer=(char*)buffer1;
	//int inputLength=strlen(buffer1);
	if(inputLength<(targetInode->Number_Blocks*SuperBlock->Data_blocksize))
		{	int LengthOfZeros=(targetInode->Number_Blocks*SuperBlock->Data_blocksize)-inputLength;
			char*zeros=new char[LengthOfZeros]();
			buffer=new char[(targetInode->Number_Blocks*SuperBlock->Data_blocksize)];
			copy((char*)buffer1, (char*)buffer1+inputLength, buffer);
			copy(zeros, zeros + LengthOfZeros, buffer+inputLength);
			delete[]zeros;
		}
	targetInode->EndingOffset=inputLength%SuperBlock->Data_blocksize;
	char*moving_buffer=(char*)buffer;
	unsigned int * start=FBL->get_sequence(targetInode);
	unsigned int *origin=start;
	unsigned int * next;
	while(moving_buffer<(buffer+SuperBlock->Data_blocksize*targetInode->Number_Blocks))//while moving buffer didn't reach the end
		{	next=start;
			while(next[1]-next[0]==1)
				next++;
			lseek(fd,SuperBlock->DataBlocks_Start+start[0]*SuperBlock->Data_blocksize,SEEK_SET);
			moving_buffer+=write(fd,moving_buffer,(next[0]-start[0]+1)*SuperBlock->Data_blocksize);
			start=&next[1];
		}
	delete[]origin;
	delete[]buffer;
	targetInode->write_inode(this,SuperBlock,FIT,FBL);
	return true;
}

unsigned int File::llseek(unsigned int offset,unsigned int origin){
	return lseek(fd,origin+offset,SEEK_SET);
}
unsigned int File::s_read(void *buf,unsigned int count){
		return read(fd,buf,count);
}
unsigned int File::s_write(void *buf,unsigned int count){
		return write(fd,buf,count);
}

bool File::WriteInodeNumber(inode_Class * targetInode,unsigned int AddedInodeNumber){
	if(targetInode->EndingOffset==0&&targetInode->Number_Blocks==0){//first inode in the first block
	unsigned int*block=FBL->get_blocks(1);//get a block
	targetInode->Number_Blocks=1;//update the inode data
	targetInode->First_Block_Number=block[0];
	delete block;
	}
	else if(targetInode->EndingOffset==0){//not the first block but at the beginning of a new one that isn't there yet
		FBL->Append_blocks(targetInode,1);//get one more block
	}
	unsigned int*blocks=FBL->get_sequence(targetInode);
	unsigned int lastblock=blocks[targetInode->Number_Blocks-1];//get the last block we gonna write to
	delete[]blocks;
	lseek(fd,SuperBlock->DataBlocks_Start+(lastblock*SuperBlock->Data_blocksize)+targetInode->EndingOffset,SEEK_SET);
	write(fd,&AddedInodeNumber,sizeof(targetInode->inode_number));
	targetInode->EndingOffset=(targetInode->EndingOffset+sizeof(targetInode->inode_number))%(SuperBlock->Data_blocksize);
	targetInode->write_inode(this,SuperBlock,FIT,FBL);
	return true;
}


bool File::DeleteInodeNumber(inode_Class * targetInode,unsigned int deletedInodeNumber){
	LinkedList<unsigned int> temp;
	void*buffer;
	/*get the inode numbers of the directory*/
	as_read(targetInode,buffer);
	unsigned int *childern_inodes=(unsigned int*)buffer;
	unsigned int i=0;
	unsigned int max_inode_number=SuperBlock->inode_Numbers+SuperBlock->inode_Numbers*SuperBlock->extension_nums;
	unsigned int actualsize=((targetInode->Number_Blocks?(targetInode->Number_Blocks==1?0:targetInode->Number_Blocks-1):0)*SuperBlock->Data_blocksize)+(targetInode->EndingOffset);
	while(childern_inodes[i]<max_inode_number&&childern_inodes[i]!=0&&i<(actualsize/sizeof(unsigned int))){/*as the inode number is not 0*/
		if(childern_inodes[i]!=deletedInodeNumber)
			temp.insert(childern_inodes[i]);
		i++;
	}
	unsigned int *remainingInodes=new unsigned int [temp.count]();
	unsigned int data;
	bool flag=temp.first(data);
	i=0;
	while(flag){
		remainingInodes[i]=data;
		flag=temp.next(data);
		i++;
	}

	if(ceil(((float)(temp.count*sizeof(unsigned int)))/SuperBlock->Data_blocksize)<targetInode->Number_Blocks)
		FBL->Free_Last(targetInode);
	as_write(targetInode,remainingInodes,(temp.count*sizeof(unsigned int)));
	return true;
}








//******************************************************************************************** //new

bool File::as_write(void * buffer1,unsigned int inputLength,unsigned int *BLocks,unsigned int NumOfBlocks){
	assert(buffer1);
	char*buffer=(char*)buffer1;
	if(inputLength<(NumOfBlocks*SuperBlock->Data_blocksize))
		{	int LengthOfZeros=(NumOfBlocks*SuperBlock->Data_blocksize)-inputLength;
			char*zeros=new char[LengthOfZeros]();
			buffer=new char[(NumOfBlocks*SuperBlock->Data_blocksize)];
			copy((char*)buffer1, (char*)buffer1+inputLength, buffer);
			copy(zeros, zeros + LengthOfZeros, buffer+inputLength);
			delete[]zeros;
		}

	//targetInode->EndingOffset=inputLength%SuperBlock->Data_blocksize;
	char*moving_buffer=(char*)buffer;
	unsigned int * start=BLocks;
	unsigned int *origin=start;
	unsigned int * next;
	while(moving_buffer<(buffer+SuperBlock->Data_blocksize*NumOfBlocks))//while moving buffer didn't reach the end
		{	next=start;
			while(next[1]-next[0]==1)
				next++;
			lseek(fd,SuperBlock->DataBlocks_Start+start[0]*SuperBlock->Data_blocksize,SEEK_SET);
			moving_buffer+=write(fd,moving_buffer,(next[0]-start[0]+1)*SuperBlock->Data_blocksize);
			start=&next[1];
		}
	delete[]origin;
	delete[]buffer;
	//targetInode->write_inode(this,SuperBlock);
	return true;
}

//********************************************************************************************

bool File::as_read(char* &buffer1,unsigned int *Blocks,unsigned int NumOfBlocks){
	char*buffer=new char[SuperBlock->Data_blocksize*NumOfBlocks];
	assert(buffer);
	char*moving_buffer=(char*)buffer; //to keep appending on the main buffer
	unsigned int * start=Blocks;
	unsigned int *origin=start;//start of the blocks array
	unsigned int * next;
	while(moving_buffer<(buffer+SuperBlock->Data_blocksize*NumOfBlocks))//while moving buffer didn't reach the end
		{	next=start;//at the beginning of the sequence next = start
			while(next[1]-next[0]==1)//while the difference of the block number and the next one is one then next to read block sequence on time
				next++;//move the next pointer to the next block number to check
			lseek(fd,SuperBlock->DataBlocks_Start+start[0]*SuperBlock->Data_blocksize,SEEK_SET);// the next is out of sequence then seek to the start block number
			moving_buffer+=read(fd,moving_buffer,(next[0]-start[0]+1)*SuperBlock->Data_blocksize);//read the data and input it at the position of the moving buffer then move the moving buffer by the number of the data read
			start=&next[1]; // the new start is the new next
		}
	delete[]origin;
	buffer1=buffer;
	return true;
}

//********************************************************************************************

//
//bool File::as_read(unsigned int number_of_blocks,unsigned int *blocks,void* &buffer1){
//	char*buffer=new char[SuperBlock->Data_blocksize*number_of_blocks];
//	assert(buffer);
//	char*moving_buffer=(char*)buffer; //to keep appending on the main buffer
//	unsigned int * start=blocks;
//	unsigned int * next;
//	while(moving_buffer<(buffer+SuperBlock->Data_blocksize*number_of_blocks))//while moving buffer didn't reach the end
//		{	next=start;//at the beginning of the sequence next = start
//			while(next[1]-next[0]==1)//while the difference of the block number and the next one is one then next to read block sequence on time
//				next++;//move the next pointer to the next block number to check
//			lseek(fd,SuperBlock->DataBlocks_Start+start[0]*SuperBlock->Data_blocksize,SEEK_SET);// the next is out of sequence then seek to the start block number
//			moving_buffer+=read(fd,moving_buffer,(next[0]-start[0]+1)*SuperBlock->Data_blocksize);//read the data and input it at the position of the moving buffer then move the moving buffer by the number of the data read
//			start=&next[1]; // the new start is the new next
//		}
//	buffer1=buffer;
//	return true;
//}
//bool File::as_write(unsigned int number_of_blocks,unsigned int *blocks,void * buffer1,unsigned int inputLength){
//	assert(buffer1);
//	char*buffer=(char*)buffer1;
//	//int inputLength=strlen(buffer1);
//	if(inputLength<(number_of_blocks*SuperBlock->Data_blocksize))
//		{	int LengthOfZeros=(number_of_blocks*SuperBlock->Data_blocksize)-inputLength;
//			char*zeros=new char[LengthOfZeros]();
//			buffer=new char[(number_of_blocks*SuperBlock->Data_blocksize)];
//			copy((char*)buffer1, (char*)buffer1+inputLength, buffer);
//			copy(zeros, zeros + LengthOfZeros, buffer+inputLength);
//			delete[]zeros;
//		}
////	targetInode->EndingOffset=inputLength%SuperBlock->Data_blocksize;
//	char*moving_buffer=(char*)buffer;
//	unsigned int * start=blocks;
//	unsigned int * next;
//	while(moving_buffer<(buffer+SuperBlock->Data_blocksize*number_of_blocks))//while moving buffer didn't reach the end
//		{	next=start;
//			while(next[1]-next[0]==1)
//				next++;
//			lseek(fd,SuperBlock->DataBlocks_Start+start[0]*SuperBlock->Data_blocksize,SEEK_SET);
//			moving_buffer+=write(fd,moving_buffer,(next[0]-start[0]+1)*SuperBlock->Data_blocksize);
//			start=&next[1];
//		}
//	delete[]buffer;
////	targetInode->write_inode(this,SuperBlock);
//	return true;
//}

