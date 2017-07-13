#ifndef INODE_H_
#define INODE_H_

#include "File.h"
#include "FIT.h"
#include "FBL.h"
#include <iostream>
#include <ctime>
using namespace std;
class inode_Class{
public:

	unsigned int inode_number;  	// starts with (0) with max (inode_numbers)
	unsigned int First_Block_Number;     	// get it from FBL
	unsigned int Number_Blocks;				//number of blocks of this file

	unsigned int EndingOffset;
	char Mode;					    // 0 for file and 1 for directory
	char FileName[101]; 	// The name of the file (with 10 letters maximum)
	char CreationTime[18];



	void get_inodeNumber_Data(File* myfile,SuperBlock_Class* sb,unsigned int inode_number,FIT_Class *FitPtr);
	void write_inode(File * myfile,SuperBlock_Class* sb,FIT_Class* FitPtr,FBL_Class *FblPtr);		// write inode in the file system in the fat




	void delete_inode(FIT_Class* FitPtr,SuperBlock_Class* SuperPtr,FBL_Class * FBL);   // delete inode from the VFS , just clear the Status and override it's data
	void New_inode(const char* NewFileName,char mode,FIT_Class* FitPtr,SuperBlock_Class* SuperPtr,FBL_Class *FblPtr,File *myfile);
	bool get_inode_NUM(FIT_Class* FitPtr,SuperBlock_Class* SuperPtr,FBL_Class *FblPtr,File *myfile) ;

};
#endif
