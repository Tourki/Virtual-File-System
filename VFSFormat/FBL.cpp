/*
 * FBL.cpp
 *
 *  Created on: May 12, 2017
 *      Author: muhammad
 */
#include "FBL.h"
#include "Inode.h"
#include <stdio.h>

/***********************constructor***************************
 * get fblstart and end from superblock and number of blocks
 * allocate new fbl in the memory
 * */

 FBL_Class::FBL_Class(SuperBlock_Class *sb,File *myfile1){
	 myfile=myfile1;
	 SB=sb;
	 FBL=new int[SB->Blocks_Number]();
	 assert(FBL);
	 //ediiiited byzizooo
	 /*
	   * the following step must be in format not here
	   * i think the same for FIT_CLASS
	   *
	 */
	 std::fill(FBL,FBL+SB->Blocks_Number+1,-1);//initialize the fbl with -1 which means all block is free to be used .


}
/*
 * free=-1
 * EOF=-2
 * */
/**********list_all_blocks_status*************/
void FBL_Class::list_FBL(){
	for (int i= 0; i <48; i++) {
		cout<<FBL[i]<<" ";
	}
}
/**********write_FBL_in_the_file*************/
void FBL_Class::sync_FBL(){
	assert(myfile);
	myfile->llseek(0,SB->FBL_Start);
	myfile->s_write((char*)FBL,sizeof(FBL[0])*SB->Blocks_Number);//dd if=/dev/zero of=hamada.txt bs=8 count=1024
}
/**********load_FBL_from_the_file*************/
void FBL_Class::load_FBL(){
	assert(myfile);
	myfile->llseek(0,SB->FBL_Start);
	myfile->s_read(FBL,sizeof(FBL[0])*SB->Blocks_Number);//dd if=/dev/zero of=hamada.txt bs=8 count=1024
}
/*********************getSequence************************/
unsigned int * FBL_Class::get_sequence(inode_Class *targetInode){
	assert(targetInode);
	if(targetInode->Number_Blocks==0)
		return NULL;
	unsigned int *x=new unsigned int[targetInode->Number_Blocks]();
	assert(x);
	x[0]=targetInode->First_Block_Number;
	if(targetInode->Number_Blocks==1)
		return x;
	for(int i=0;FBL[x[i]]!=-2;i++)//loop till reach the final block in the file
		x[i+1]=FBL[x[i]];//get the next block number
	return x ;
}

/**************free_file_blockss**************/
void FBL_Class::delete_file(inode_Class *targetInode){
	assert(targetInode);

	if(FBL[targetInode->First_Block_Number]==-1)//editeeeeeed by zizo
		{
			cout<<"already deleted ..!";
			return;
		}
	unsigned int *x=get_sequence(targetInode);
	if(!x)
		return;
	unsigned int i;
	for( i=0;i<targetInode->Number_Blocks;i++)
		FBL[x[i]]=-1;
	delete[]x;
	SB->FreeBlocks_Number+=i+1;//editeeeeeed by zizo

}
/***********************get blocks**********************///editeeeeed by zizooo
unsigned int* FBL_Class::get_blocks(unsigned int number_of_blocks){

	/**check for number of blocks to be allocated is less than number of free blocks else give a message that we don't have enough space**/
	if(SB->FreeBlocks_Number<number_of_blocks)
	{
		cout<<"there is no enough space .. !";
		return 0;
	}

	unsigned int *start=new unsigned int[number_of_blocks]();//allocate array with numbers of the blocks
	assert(start);
	unsigned int j=0,count=0;
	for(unsigned int i=0;i < SB->Blocks_Number;i++)
	{
		if(FBL[i]==-1 && j<number_of_blocks)//search for a free block
		{
			if(FBL[i+1]==-1 || SB->FreeBlocks_Number-j<number_of_blocks || count+1==number_of_blocks)
			{
				start[j]=i;
				j++;
				count++;
			}
			else
			{
				j=0;
			}

		}
	}

	FBL[start[j-1]]=-2;//set the end of the file blocks to -2
	for(unsigned int i=0;i < j-1;i++)
	{
		FBL[start[i]]=start[i+1]; //link blocks of a one file
	}
	SB->FreeBlocks_Number-=number_of_blocks;
	return start;
}/*********************************************************************/
void FBL_Class::Append_blocks(inode_Class * extendedInode,unsigned int number_of_blocks_Added){
	assert(number_of_blocks_Added<SB->FreeBlocks_Number);
	assert(extendedInode);
	unsigned int *OldBlocks=get_sequence(extendedInode);
	unsigned int *NewBlocks=get_blocks(number_of_blocks_Added);
	FBL[OldBlocks[(extendedInode->Number_Blocks)-1]]=NewBlocks[0];
	extendedInode->Number_Blocks+=number_of_blocks_Added;
//	SB->FreeBlocks_Number-=number_of_blocks_Added; ediiiiiiiited by zizo
	delete[]OldBlocks;
	delete[]NewBlocks;
}


/*************************************************************************/
void FBL_Class::Free_Last(inode_Class * targetInode){
	if(targetInode->Number_Blocks==1){
		FBL[targetInode->First_Block_Number]=-1;
		targetInode->First_Block_Number=0;
		targetInode->Number_Blocks=0;
	}
	else{
		unsigned int * blocks=get_sequence(targetInode);
		unsigned int newlastblock=blocks[targetInode->Number_Blocks-2];
		unsigned int oldlastblock=blocks[targetInode->Number_Blocks-1];
		delete[]blocks;
		FBL[newlastblock]=-2;
		FBL[oldlastblock]=-1;
		targetInode->Number_Blocks--;
		targetInode->EndingOffset=0;
		targetInode->write_inode(myfile,myfile->SuperBlock,myfile->FIT,myfile->FBL);
	}

}


/*************************************************************************************/
unsigned int * FBL_Class::get_sequence(unsigned int First_BLock_Number,unsigned int Number_Blocks){
	if(Number_Blocks==0)
		return NULL;
	unsigned int *x=new unsigned int[Number_Blocks]();
	assert(x);
	x[0]=First_BLock_Number;
	if(Number_Blocks==1)
		return x;
	for(int i=0;FBL[x[i]]!=-2;i++)//loop till reach the final block in the file
		x[i+1]=FBL[x[i]];//get the next block number
	return x ;
}

/**************free_file_blockss**************/
void FBL_Class::delete_file(unsigned int First_BLock_Number,unsigned int Number_Blocks){
	if(FBL[First_BLock_Number]==-1){//editeeeeeed by zizo
		cout<<"already deleted ..!";
		return;
	}
	unsigned int *x=get_sequence(First_BLock_Number,Number_Blocks);
	if(!x)
		return;
	unsigned int i;
	for( i=0;i<Number_Blocks;i++)
		FBL[x[i]]=-1;
	if(Number_Blocks==1)
		delete x;
	else
		delete[]x;
	SB->FreeBlocks_Number+=i+1;//editeeeeeed by zizo

}

//
//unsigned int * FBL_Class::get_sequence(unsigned int first_block,unsigned int number_of_block){
//	unsigned int *x=new unsigned int();
//	assert(x);
//	x[0]=first_block;
//	if(number_of_block==1)
//		return x;
//	x[1]=FBL[first_block];//get the next block number
//	int i=0;
//	while(x[i]!=-2)//loop till reach the final block in the file
//		{
//			x[i+1]=FBL[x[i]];//get the next block number
//			i++;
//		}
//	return x ;
//}
//
///**************free_file_blockss**************/
//void FBL_Class::delete_file(unsigned int first_block){
//	if(FBL[first_block]==-1)
//	{
//		cout<<"already deleted ..!";
//		return;
//	}
//	int *x=new int();
//	assert(x);
//	x[0]=FBL[first_block];//get the next block number
//	FBL[first_block]=-1; //free the first block
//	int i=0;
//	while(x[i]!=-2)//loop till reach the final block in the file
//		{
//			x[i+1]=FBL[x[i]];//get the next block number
//			FBL[x[i]]=-1; //free the current bl0ck
//			i++;
//		}
//	SB->FreeBlocks_Number+=i+1;
//}
