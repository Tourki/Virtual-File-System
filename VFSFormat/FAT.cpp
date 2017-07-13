/*
 * FAT.cpp
 *
 *  Created on: Jun 19, 2017
 *      Author: tourki
 */

#include "FAT.h"
FAT_Class::FAT_Class(File  *MyFile1)
{
	MyFile=MyFile1;
	inode_Class *rootinode=new inode_Class;
	rootinode->get_inodeNumber_Data(MyFile,MyFile->SuperBlock,0,MyFile->FIT);/*get the inode data of the root*/
	root=FAT_Load(*rootinode);
	cd=root;
	delete rootinode;
}



FAT_Class::Link FAT_Class::FAT_Load(inode_Class node){
	Link AddedEntity=new Entity;
	assert(AddedEntity);
	AddedEntity->inode=node;//add the data to the entity
	if(AddedEntity->inode.Mode=='f')//if the added node is a file
		return AddedEntity;
	 //the added node is a director
	else if(AddedEntity->inode.Mode=='d'){
		void*buffer;
		/*get the inode numbers of the directory*/
		MyFile->as_read(&AddedEntity->inode,buffer);
		unsigned int *childern_inodes=(unsigned int*)buffer;
		int i=0;
		unsigned int max_inode_number=MyFile->SuperBlock->inode_Numbers+MyFile->SuperBlock->inode_Numbers*MyFile->SuperBlock->extension_nums;
		unsigned int actualsize=((node.Number_Blocks?(node.Number_Blocks==1?0:node.Number_Blocks-1):0)*MyFile->SuperBlock->Data_blocksize)+(node.EndingOffset);
		while(childern_inodes[i]<max_inode_number&&childern_inodes[i]!=0&&i<(actualsize/sizeof(unsigned int))){/*as the inode number is not 0*/
			inode_Class child_inode;
			unsigned int tempd=childern_inodes[i];
			child_inode.get_inodeNumber_Data(MyFile,MyFile->SuperBlock,childern_inodes[i],MyFile->FIT);/*get the inode data*/
			Link temp=FAT_Load(child_inode);/*recursion on the inode and save the entity pointer to link parents and childern*/
			if(temp){//if temp has a value
				AddedEntity->childern.insert(temp);
				temp->parent=AddedEntity;
			}
			i++;
		}
		return AddedEntity;
	}
	else
		return NULL;
}



string to_lower(const char * target){
	string output;
	int i=0;
	while (target[i]){//start from the first char of the argument to the last
		output+=tolower(target[i]);
		i++;
	}
	return output;
}

FAT_Class::Link FAT_Class::Search(const char * target){
	Link check;
	bool flag=cd->childern.first(check);
	while(flag){
		if((strcmp(target,check->inode.FileName)==0))
			return check;
		flag=cd->childern.next(check);
	}
	return NULL;
}

void FAT_Class::DeleteEntity(Link target){
	assert(target);
	target->parent->childern.Delete(target);//remove the child from parent linked list
	if(target->inode.Mode=='f'){//deleting file
		target->inode.delete_inode(MyFile->FIT,MyFile->SuperBlock,MyFile->FBL);//release inode number
		MyFile->FBL->delete_file(&target->inode);//release inode data blocks
		MyFile->DeleteInodeNumber(&target->parent->inode,target->inode.inode_number);
	}
	else{//the target is a directory
		x: Link check;
		bool flag=target->childern.first(check);//delete the children one by one
		while(flag){
			DeleteEntity(check);
			flag=target->childern.next(check);
			goto x;
		}
		target->inode.delete_inode(MyFile->FIT,MyFile->SuperBlock,MyFile->FBL);//after deleting all the childern delete the folder node
		MyFile->FBL->delete_file(&target->inode);//release inode data blocks
		MyFile->DeleteInodeNumber(&target->parent->inode,target->inode.inode_number);
	}
}

bool FAT_Class::DeleteWithName(const char * target){
	Link targetEntity=Search(target);
	if(targetEntity){//target found
		DeleteEntity(targetEntity);
		return true;
	}
	return false;
}

bool FAT_Class::Change_Directory(const char * nextDirectory){
	if(!strcmp(nextDirectory,"..")){
		if(cd!=root){
			cd=cd->parent;return true;
		}
		else{
			cd=root;return true;
		}

	}
	else{
	Link target=Search(nextDirectory);
	if(target && target->inode.Mode=='d'){
		cd=target;	return true;
	}
	return false;
	}
}

FAT_Class::Link FAT_Class::InsertEntity(Link Entity){
	if(Search(Entity->inode.FileName))//there is a file with the same name
			return NULL;
	Link oldparent=Entity->parent;
	/*first remove the link of the old parent*/
	Entity->parent->childern.Delete(Entity);//remove the child from parent linked list
	MyFile->DeleteInodeNumber(&Entity->parent->inode,Entity->inode.inode_number);
	/*second linking to the new parent*/
	Entity->parent=cd;//linking the new entity to it's parent
	Entity->parent->childern.insert(Entity);
	MyFile->WriteInodeNumber(&Entity->parent->inode,Entity->inode.inode_number);//write the inserted entity to the new parent

	Entity->parent->inode.write_inode(MyFile,MyFile->SuperBlock,MyFile->FIT,MyFile->FBL);//write the new parent inode
	oldparent->inode.write_inode(MyFile,MyFile->SuperBlock,MyFile->FIT,MyFile->FBL);//write the old parent inode
	return Entity;
}
FAT_Class::Link FAT_Class::CreateEntity(const char * FileName,char mode){
	if(Search(FileName))//there is a file with the same name
		return NULL;
	Link NewEntity=new Entity;
	assert(NewEntity);
	NewEntity->inode.New_inode(FileName,mode,MyFile->FIT,MyFile->SuperBlock,MyFile->FBL,MyFile);//adding file name and mode to the inode
	NewEntity->parent=cd;//linking the new entity to it's parent
	NewEntity->parent->childern.insert(NewEntity);
	MyFile->WriteInodeNumber(&NewEntity->parent->inode,NewEntity->inode.inode_number);
	NewEntity->inode.write_inode(MyFile,MyFile->SuperBlock,MyFile->FIT,MyFile->FBL);
	return NewEntity;
}



struct allsize{
    long double value;
    char symbol;
    allsize(long double bytes){
        long double kilobyte=bytes / 1024;
        long double megabyte=kilobyte / 1024;
        long double gigabyte=megabyte / 1024;
        value=gigabyte>1?gigabyte:(megabyte>1?megabyte:(kilobyte>1?kilobyte:bytes));
        symbol=gigabyte>1?'G':(megabyte>1?'M':(kilobyte>1?'K':'B'));
    }
};
void FAT_Class::List(){
	Link temp;
	bool flag=cd->childern.first(temp);
	while(flag){
		if(temp->inode.Mode=='f'){//if the listed is a file{
			stringstream ss;
			allsize SizeOnDisk((temp->inode.Number_Blocks)*(MyFile->SuperBlock->Data_blocksize));
			allsize Size(((temp->inode.Number_Blocks?(temp->inode.Number_Blocks==1?0:temp->inode.Number_Blocks-1):0)*MyFile->SuperBlock->Data_blocksize)+(temp->inode.EndingOffset));
			ss<<SizeOnDisk.value<<SizeOnDisk.symbol<<"("<<Size.value<<Size.symbol<<")";
			string BothSizes;
			ss>>BothSizes;
			cout<<setw(4)<<left<<temp->inode.Mode<<setw(10)<<left<<(temp->inode.inode_number)<<setw(20)<<left<<BothSizes<<setw(25)<<left<<temp->inode.CreationTime<<temp->inode.FileName<<endl;

		}
		else{//the listed is a directory
			Results results;
			results.TotalBlocksSize=0;
			results.AccualSize=0;
			FolderSize(temp,results);
			stringstream ss;
			allsize SizeOnDisk(results.TotalBlocksSize);
			allsize Size(results.AccualSize);
			ss<<SizeOnDisk.value<<SizeOnDisk.symbol<<"("<<Size.value<<Size.symbol<<")";
			string BothSizes;
			ss>>BothSizes;
			cout<<setw(4)<<left<<temp->inode.Mode<<setw(10)<<left<<(temp->inode.inode_number)<<setw(20)<<left<<BothSizes<<setw(25)<<left<<temp->inode.CreationTime<<temp->inode.FileName<<endl;
		}
		flag=cd->childern.next(temp);

	}

}

void FAT_Class::FolderSize(FAT_Class::Link target,Results &result){
	result.TotalBlocksSize+=target->inode.Number_Blocks*MyFile->SuperBlock->Data_blocksize;
	result.AccualSize+=((target->inode.Number_Blocks?(target->inode.Number_Blocks==1?0:target->inode.Number_Blocks-1):0)*MyFile->SuperBlock->Data_blocksize)+(target->inode.EndingOffset);

	Link temp;
	bool flag=target->childern.first(temp);
	while(flag){
		if(temp->inode.Mode=='f'){
			result.TotalBlocksSize+=temp->inode.Number_Blocks*MyFile->SuperBlock->Data_blocksize;
			result.AccualSize+=((temp->inode.Number_Blocks?(temp->inode.Number_Blocks==1?0:temp->inode.Number_Blocks-1):0)*MyFile->SuperBlock->Data_blocksize)+(temp->inode.EndingOffset);
		}
		else
			FolderSize(temp,result);
		 flag=target->childern.next(temp);
	}
}


FAT_Class::Link FAT_Class::FindFileWithFirstBlock(Link target,unsigned int FirstBlock){
	Link temp;
	bool flag=target->childern.first(temp);
	while(flag){
		if(temp->inode.Mode=='f'&&temp->inode.First_Block_Number==FirstBlock)
			return temp;
		else if(temp->inode.First_Block_Number==FirstBlock)
			return temp;
		else if(!(temp=FindFileWithFirstBlock(temp,FirstBlock)))
			return temp;
		flag=target->childern.next(temp);
	}
	return NULL;//no match found
}








