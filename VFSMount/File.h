/*
 * File.h
 *
 *  Created on: May 8, 2017
 *      Author: tourki
 */

#ifndef FILE_H_
#define FILE_H_
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
using namespace std;


class SuperBlock_Class;
class FIT_Class;
class FBL_Class;
class inode_Class;

class File{
private :
	int fd;
public :
	SuperBlock_Class *SuperBlock;FIT_Class *FIT;FBL_Class *FBL;
	File(const char * file_name2);/* this function open file of the VFS*/
	~File();					 /* this function close the file of the VFS*/
	unsigned int llseek(unsigned int offset,unsigned int origin);
	/*this function seek with the file descriptor to the given offset from the given origin
	 * and return the current position
	 */
	unsigned int s_read(void *buf,unsigned int count);
	/*this function reed with the file descriptor and return number of bytes read
	 *
	 */
	unsigned int s_write(void *buf,unsigned int count);
	/*this function reed with the file descriptor and return number of bytes read
	 *
	 */
	bool as_read (inode_Class * targetInode,void*  &buffer);
	bool as_write(inode_Class * targetInode,void*  buffer,unsigned int inputLength);

	bool as_write(void * buffer1,unsigned int inputLength,unsigned int *BLocks,unsigned int NumOfBlocks);
	bool as_read(char* &buffer1 , unsigned int *BLocks ,unsigned int NumOfBlocks);


	bool WriteInodeNumber(inode_Class * targetInode,unsigned int AddedInodeNumber);
	bool DeleteInodeNumber(inode_Class * targetInode,unsigned int AddedInodeNumber);
};


#endif /* FILE_H_ */
