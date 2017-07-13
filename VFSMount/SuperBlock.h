/*
 * SuperBlock.h
 *
 *  Created on: May 5, 2017
 *      Author: tourki
 */

#ifndef SUPERBLOCK_H_
#define SUPERBLOCK_H_
#include "File.h"
class SuperBlock_Class{
public:
	unsigned int Extension_NumOfBlocks;
	unsigned  int first_Extension_BlockNum;	// the blocks in which the first extension was stored
	unsigned long *First_Extension;		// Must still unsigned long
	//
	unsigned int extension_nums;	//Y
	unsigned int Current_inodesNum;			//initially 0,represent overall number of files and folders nodes
	unsigned long Data_blocksize;	/* block size in bytes */
	unsigned long Blocks_Number;	/* total number of data blocks */
	unsigned long FreeBlocks_Number;/*  number of free data blocks */
	unsigned long FIT_Start;		/* Start byte of the FBL */
	unsigned long FIT_END;			/* END byte of the FBL */
	unsigned long FAT_Start;		/* Start byte of the FAT */
	unsigned long FAT_END;			/* END byte of the FAT */
	unsigned long FBL_Start;		/* Start byte of the FBL */
	unsigned long FBL_END;			/* END byte of the FBL */
	unsigned long DataBlocks_Start;	/* Start byte of the DataBlocks */
	unsigned long DataBlocks_END;	/* END byte of the DataBlocks */
	unsigned long inode_Numbers;		/* Files Counter */
public:
	void load_super(File *Myfile);
	/* this function take a pointer of the File class load the super block into this and give the File a Data offset and block size
	 *
	 */
	void sync_super(File *Myfile);
	/* this function update the data of the super block in the actual file system
	 *
	 */
	bool InodeNum_Address(unsigned int num,unsigned int&BlockNumber);
};

/*
SuperBlock_Class::SuperBlock_Class(){

	Data_blocksize=1024;	 //block size in bytes
	Blocks_Number=131072;	 //total number of data blocks
	FreeBlocks_Number=131072;  //number of free data blocks
	FIT_Start;		// Start byte of the FBL
	FIT_END;			// END byte of the FBL
	FAT_Start=89;		 //Start byte of the F+++
	FAT_END=1048665;		//	 END byte of the FAT
	FBL_Start=1048666;		// Start byte of the FBL
	FBL_END=1572954;			// END byte of the FBL
	FBL_entity_size=4;
	DataBlocks_Start=1572955;	 //Start byte of the DataBlocks
	DataBlocks_END=135790683;	 //END byte of the DataBlocks
	FILES_Number=0;		 //Files Counter
}
 */




#endif /* SUPERBLOCK_H_ */
