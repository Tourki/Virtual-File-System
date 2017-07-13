/*Notes-
 * The block size must be greater than or equal the Fit.
 * fit end = fit start + sizeof(char)*(inode_numbers)
 * we've to add a destructor in all units to sync everything and release any allocated memory.
 * we may add a variable to represent the file attributes (i.e. read only , hidden ,...).
*/

#include "FIT.h"
//--------------------------------------------------------------------------------------------------
FIT_Class::FIT_Class(SuperBlock_Class *SB,FBL_Class *fbl){		//Done
	FIT=new char [(SB->inode_Numbers)];   // the last element in the array's either 0 (no extension) or holds the next fit's address
	for(unsigned int i=0;i<(SB->inode_Numbers);i++)	FIT[i]='0';
	sb=SB;
	FBL=fbl;
	FIT[0]='1';
}

bool FIT_Class::get_inode_number(unsigned int &num,SuperBlock_Class *sb,FBL_Class *FblPtr,File *myfile)		//Done
{
if((sb->Current_inodesNum)<(sb->inode_Numbers)*((sb->extension_nums)+1)) // a free node already exists
{	for(unsigned long i=0; i< (sb->inode_Numbers); i++)	// search in fit
	{
		if(FIT[i]=='0')
		{
			num=i;
			(sb->Current_inodesNum)++;
			FIT[i]='1';
			return true;
		}

	}	//el mafrod hena if current<....
	if((sb->extension_nums)>0){		// search in extension
		Extension* Ex=(Extension*)(sb->First_Extension);

		assert(Ex);
		for(unsigned int k=0;k<(sb->extension_nums);k++)
		{
			bool tmp=Ex->get_inode_number(num,sb,FblPtr,myfile);
			if(tmp) return true;
			Ex=Ex->Next;
		}
	}
}
	//---Extend	-----------------------------------------------------------------------------------------

	Extension *ex=new Extension(sb);
	ex->Extension_Number=(sb->extension_nums)+1;
	Extension *tmp=(Extension *)(sb->First_Extension);	// adjusting the extension number
			for(unsigned int i=1;i<=(sb->extension_nums);i++)
			{
				if(tmp->Extension_Number!=i)
					ex->Extension_Number=i;
				tmp=tmp->Next;
			}


	(sb->extension_nums)++;
	unsigned int* ptr=FblPtr->get_blocks((sb->Extension_NumOfBlocks)); // 5= sizeof(point)				//number of needed blocks
	assert(ptr);

	if((sb->extension_nums)==1){
		ex->Extension_Number=1;
		sb->first_Extension_BlockNum=ptr[0];
		sb->First_Extension=(unsigned long*)(ex);
		ex->get_inode_number(num,sb,FblPtr,myfile);
		return true;
		}
//------------------
	assert(sb->First_Extension);
	Extension *temp1=(Extension *)(sb->First_Extension);
	if(ex->Extension_Number < temp1->Extension_Number){
		ex->Extension_Number=1;
		ex->Next=temp1;
		ex->NextBlocks=sb->first_Extension_BlockNum;
		sb->first_Extension_BlockNum=ptr[0];
		sb->First_Extension=(unsigned long*)ex;
		ex->get_inode_number(num,sb,FblPtr,myfile);
		return true;
	}
////-------------
	while(temp1->Next!=0){
		if(temp1->Next->Extension_Number > ex->Extension_Number)
			break;
		temp1=temp1->Next;
	}

	ex->Next=temp1->Next;
	ex->NextBlocks=temp1->NextBlocks;
	temp1->NextBlocks=ptr[0];
	temp1->Next=ex;
	ex->get_inode_number(num,sb,FblPtr,myfile);
	return true;
	//-------------------------------------------------------------------------------------------------------------
}

void FIT_Class::Delete_inode(SuperBlock_Class* sb,FBL_Class *FblPtr,unsigned long num){			//Done
// Adjusting fit-----------------------------------
//	assert(num<(sb->inode_Numbers)*((sb->extension_nums)+1));     // assert that the inode_number is valid and existed.
	if(num<(sb->inode_Numbers)){	FIT[num]='0';   (sb->Current_inodesNum)--;      return;  }
//	If an extension happened-----------------

	unsigned int count=num/(sb->inode_Numbers);
	Extension *temp=(Extension *)(sb->First_Extension);

	while(temp->Next!=0){
				if(temp->Extension_Number==count)
					break;
				temp=temp->Next;
			}


	temp->x[num%(sb->inode_Numbers)].state='0';
	temp->DeleteBlock(FblPtr,sb,num);
	temp->x[num%(sb->inode_Numbers)].Block_Number=0;
	(sb->Current_inodesNum)--;
	if(sb->extension_nums)//if extention number doesn't equal zero
		UpdateExtension(sb,FblPtr);
}

//--------------------------------------------------------------------------------------------------------

void FIT_Class::sync_FIT(File* myfile,SuperBlock_Class* sb,FBL_Class *FblPtr)
{
	assert(myfile);
	myfile->llseek(sb->FIT_Start,0);
	//char* buf=(char*)FIT;
	myfile->s_write(FIT,sizeof(char)* ((sb->inode_Numbers)));
	if((sb->extension_nums)==0) return;

//-----Extension----------------------------------------------------------------------------------------------------------------------------
	unsigned int *Extension_Blocks=FblPtr->get_sequence(sb->first_Extension_BlockNum,sb->Extension_NumOfBlocks);	// the block in which the first extension was stored
	Extension *temp=(Extension *)(sb->First_Extension);

	for(unsigned int i=0;i<(sb->extension_nums);i++){				//sync all Extensions
		temp->Sync(myfile,sb,FblPtr, Extension_Blocks);
		if(i==(sb->extension_nums)-1) return;
		Extension_Blocks=FblPtr->get_sequence(temp->NextBlocks,sb->Extension_NumOfBlocks);		//check	here
		temp=temp->Next;					//check	here
//-------------------------------------------------------------------------------------------------------------------------------------------
	}

}
void FIT_Class::load_FIT(File* myfile,SuperBlock_Class* sb)
{
	assert(myfile);
	myfile->llseek(sb->FIT_Start,0);
	myfile->s_read(FIT,sizeof(char)*(sb->inode_Numbers));
	if(sb->extension_nums==0) // if no extension happened
	return;
//Adjusting Extension--------- ----------------------------------------------------------------------
	for(unsigned int i=0;i<(sb->extension_nums);i++){
		Extension *e=new Extension(sb);
		if(i==0)
		{
			unsigned int* Blocks=FBL->get_sequence(sb->first_Extension_BlockNum,sb->Extension_NumOfBlocks);
			e->Load(myfile,sb,Blocks);
			(sb->First_Extension)=(unsigned long*)e;
		}
		if(i>0)
		{
			Extension *tmp=(Extension*)(sb->First_Extension);
			assert(tmp);
			for(unsigned int j=1;j<i;j++)		tmp=tmp->Next;
			e->Load(myfile,sb,FBL->get_sequence(tmp->NextBlocks,sb->Extension_NumOfBlocks));
			tmp->Next=e;
		}
	}
}



//------------------------------------------------------------------------------------------------------------------------------
void FIT_Class::UpdateExtension(SuperBlock_Class *sb,FBL_Class *FblPtr)
{
	Extension* temp=(Extension*)(sb->First_Extension);
	while(temp!=0)
	{
		bool flag=true;
		for(unsigned int i=0;i<(sb->inode_Numbers);i++)
		{
			if(temp->x[i].state=='1')
				flag=false;
		}
		if(flag)
		{
			sb->extension_nums--;
			if(temp==(Extension*)(sb->First_Extension))
			{
				FblPtr->delete_file(sb->first_Extension_BlockNum,sb->Extension_NumOfBlocks);
				(sb->First_Extension)=(unsigned long*)(temp->Next);
				(sb->first_Extension_BlockNum)=temp->NextBlocks;
				delete temp;
				sync_FIT(FblPtr->myfile,sb,FblPtr);
				return;
			}

			Extension* prev=(Extension*)(sb->First_Extension);
			while(prev->Next!=temp) prev=prev->Next;

			FblPtr->delete_file(prev->NextBlocks,sb->Extension_NumOfBlocks);

			prev->Next=temp->Next;
			prev->NextBlocks=temp->NextBlocks;
			delete temp;
			sync_FIT(FblPtr->myfile,sb,FblPtr);

			return;
		}
		temp=temp->Next;
	}

}
