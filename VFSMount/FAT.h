/*
 * FAT.h
 *
 *  Created on: Jun 19, 2017
 *      Author: tourki
 */

#ifndef FAT_H_
#define FAT_H_
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;
#include "LinkedList.h"
#include "SuperBlock.h"
#include "Inode.h"
#include "File.h"
#include "FIT.h"
#include "FBL.h"
#include "queue.h"
class FAT_Class{
public:
	File  *MyFile;
struct Entity;
typedef Entity* Link;
struct Entity{
	inode_Class inode;
	Link parent;
	LinkedList <Link> childern;
};
Link root,cd;

/*======================== Functions =================================*/

/*The Fat contractor sets the pointer of the superblock , the file , the fit and the fbl
 * */
FAT_Class(File  *MyFile1);
Link FAT_Load(inode_Class node);

/*The search function search the current directory for a file or a floder givern by the mode character
 * and if found it returns a link to the entity of this file
 * */
Link Search(const char * target);

/*The DeleteEntity function delete an Entity form the fat if it is a file it deletes the inode using delete_inode function
 * which also release its data blocks and if it a folder it delete the subEntitys first then deletes the folder inode
 * */
void DeleteEntity(Link target);

/*The DeleteWithName searches the current directory using Search function for the file or folder and deletes it using The DeleteEntity function
 * */
bool DeleteWithName(const char * target);

/*The Change_Directory Function searches the current directory for a folder using the Search function and changes the cd pointer to that folder
 * and if the folder name wasn't found it returns false
 * */
bool Change_Directory(const char * nextDirectory);

/*The CreateEntity creates a new entity under the current directory and returns a pointer to it
 * */
Link InsertEntity(Link Entity);
Link CreateEntity(const char * FileName,char mode);
void List();
struct Results{
	unsigned int TotalBlocksSize,AccualSize;
};
void FolderSize(FAT_Class::Link target,Results &result);
Link FindFileWithFirstBlock(Link target,unsigned int FirstBlock);
};

#endif /* FAT_H_ */
