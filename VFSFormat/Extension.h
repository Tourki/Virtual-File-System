
#ifndef EXTENSION_H_
#define EXTENSION_H_
#include "FIT.h"
#include "SuperBlock.h"
#include "FBL.h"

class Extension {
public:
struct Point
	{
		char state;
		unsigned int Block_Number;
	}__attribute__((packed));           // to disable badding

Point *x;
Extension *Next;
unsigned int Extension_Number;
unsigned int NextBlocks;					//0
//unsigned int Extension_Number;


bool DeleteBlock(FBL_Class *FblPtr,SuperBlock_Class *sb,unsigned int num);

bool SearchForBlock(SuperBlock_Class *sb,unsigned int num,unsigned int NumOfNodes,unsigned int &BlockNumber);

void Sync(File* myfile,SuperBlock_Class* sb,FBL_Class *FblPtr,unsigned int *Blocks);
void Load(File* myfile,SuperBlock_Class* sb,unsigned int *Blocks);

//unsigned int InodeNum_Address(unsigned int num);		// map num to be from 0 to (inode_numbers)-1
void  Delete_inode(SuperBlock_Class* sb,unsigned int num,FBL_Class *FblPtr); // Done
bool  get_inode_number(unsigned int &num, SuperBlock_Class *sb,FBL_Class *FblPtr, File *myfile ); //Done
Extension(SuperBlock_Class *sb); //Done
//~Extension();
};

#endif /* EXTENSION_H_ */
