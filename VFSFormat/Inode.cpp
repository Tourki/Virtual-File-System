
#include "Inode.h"
// fe delete check enaha as8ar men inode_numbers  law kda  ---> delete file

void inode_Class::delete_inode(FIT_Class* FitPtr,SuperBlock_Class* SuperPtr,FBL_Class * FBL){

		//FBL->delete_file(this);
		FitPtr->Delete_inode(SuperPtr,FBL,inode_number);     // current inodes number is decreased in fit,now it's free and we can override it's data
}


//-----------------------------------------------------------------------------------------------------

void inode_Class::New_inode(const char* NewFileName,char mode,FIT_Class* FitPtr,SuperBlock_Class* SuperPtr,FBL_Class *FblPtr,File *myfile){
	strcpy(FileName,NewFileName);
	Mode=mode;
	First_Block_Number=0;
	Number_Blocks=0;
	EndingOffset=0;
	get_inode_NUM(FitPtr,SuperPtr,FblPtr,myfile);

	// adjusting the creation time------------------------------------------------------------------------
	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	strftime(CreationTime,sizeof(CreationTime),"%d-%m-%y %I:%M:%S",timeinfo);
}


 //-----------------------------------------------------------------------------------------------------
bool inode_Class::get_inode_NUM(FIT_Class *FitPtr,SuperBlock_Class *SuperPtr,FBL_Class *FblPtr,File *myfile)
{
	unsigned int num;
	bool flag=FitPtr->get_inode_number(num,SuperPtr,FblPtr,myfile); // ask the FIT for a node number
	if(flag){
		inode_number=num;	// give a number to the node
		return true;
	}
	return false;  // there're no free nodes in the FIT,we will need to extend the FIT
}
//------------------------------------------------------------------------------------------------------

void inode_Class::write_inode(File *myfile,SuperBlock_Class *sb,FIT_Class *FitPtr,FBL_Class *FblPtr){		// nodes num starts with 0	/// zapat el pointer dah 8alat
	assert(myfile);
	if(inode_number<(sb->inode_Numbers))
		{
			myfile->llseek(inode_number*sizeof(inode_Class),sb->FAT_Start);
			myfile->s_write(this,sizeof(inode_Class));
			return;
		}
	else  // an extended node
		{
			unsigned int NumOfNodes=(sb->Data_blocksize)/sizeof(inode_Class);
			unsigned int FirstToCheck=((inode_number-sb->inode_Numbers)/NumOfNodes)*NumOfNodes+sb->inode_Numbers;


			assert((sb->Data_blocksize)>sizeof(inode_Class));		// assert that the block size is enough to store the inode data in it
			unsigned int Block;
			sb->InodeNum_Address(inode_number,Block);


			myfile->llseek(Block*(sb->Data_blocksize)+(inode_number%FirstToCheck)*sizeof(inode_Class),sb->DataBlocks_Start);
			myfile->s_write(this,sizeof(inode_Class));

		}
}
//-----------------------------------------------------------------------------------------------------
void inode_Class::get_inodeNumber_Data(File* myfile,SuperBlock_Class* sb,unsigned int inode_number,FIT_Class *FitPtr){
	assert(myfile);
	if(inode_number<(sb->inode_Numbers))
		{
			myfile->llseek(inode_number*sizeof(inode_Class),sb->FAT_Start);
			myfile->s_read(this,sizeof(inode_Class));
			return;
		}
	else
		{
			unsigned int NumOfNodes=(sb->Data_blocksize)/sizeof(inode_Class);
			unsigned int FirstToCheck=((inode_number-sb->inode_Numbers)/NumOfNodes)*NumOfNodes+sb->inode_Numbers;

			assert((sb->Data_blocksize)>sizeof(inode_Class));		// assert that the block size is enough to store the inode data in it
			unsigned int Block;
			sb->InodeNum_Address(inode_number,Block);

			myfile->llseek(Block*(sb->Data_blocksize)+(inode_number%FirstToCheck)*sizeof(inode_Class),sb->DataBlocks_Start);
			myfile->s_read(this,sizeof(inode_Class));

		}

}



//----------------------------------------------------------------------------------------------------------------------------------------------

