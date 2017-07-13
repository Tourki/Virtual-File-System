#ifndef FIT_H_
#define FIT_H_
#include "SuperBlock.h"
#include "File.h"
#include "FBL.h"
#include "Extension.h"
class FIT_Class{
public:
	SuperBlock_Class *sb;
	FBL_Class *FBL;


	char *FIT;

	void UpdateExtension(SuperBlock_Class *sb,FBL_Class *FblPtr);
	void load_FIT(File* myfile,SuperBlock_Class* sb);	// read FIT &FAT from file
	void sync_FIT(File* myfile,SuperBlock_Class* sb,FBL_Class *FblPtr);    // write FIT & FAT to the vfs, update the file with the new FIT
	 FIT_Class(SuperBlock_Class *sb,FBL_Class *fbl);		//Done
	bool  get_inode_number(unsigned int &num, SuperBlock_Class *sb, FBL_Class *FblPtr, File *myfile);	//Done
	void  Delete_inode(SuperBlock_Class* sb,FBL_Class *FblPtr,unsigned long num);		//Done
};
#endif /* FIT_H_ */
