/*
 * FBL.h
 *
 *  Created on: May 6, 2017
 *      Author: tourki
 */

#ifndef FBL_H_
#define FBL_H_

class inode_Class;
class SuperBlock_Class;
class FIT_Class;
class File;

class FBL_Class{
public:
	 File*myfile;
	 SuperBlock_Class*SB;
	int * FBL; 			/*pointer to dynamiclly allocated array with numbers of blocks */
public :
	FBL_Class(SuperBlock_Class *sb,File *myfile1);
	void list_FBL();	/*list all blocks status*/
	void load_FBL();   /*load from the actual file*/
	void sync_FBL();   /* write the FBL back to the actual file*/
	unsigned int * get_sequence(unsigned int First_BLock_Number,unsigned int Number_Blocks);
	void delete_file(unsigned int First_BLock_Number,unsigned int Number_Blocks);
	unsigned int * get_sequence(inode_Class *targetInode);
	void delete_file(inode_Class *targetInode); /*take number of blocks for the file and the first file and free all blocks that is's have*/
	unsigned int * get_blocks(unsigned int number_of_blocks);/* take number of desired blocks and return free blocks numbers*/
	void Append_blocks(inode_Class * extendedInode,unsigned int number_of_blocks_Added);
	void Free_Last(inode_Class * extendedInode);
};
/*void load_FBL(){
	lseek(fblstart)
			read(FBL,(FBLs-fblend))
}


*/

#endif /* FBL_H_ */
