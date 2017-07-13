/*
 * Functions.cpp

 *
 *  Created on: Jul 3, 2017
 *      Author: tourki
 */


#include "Functions.h"

unsigned int COPY_F_IN(string target, File *myfile,FAT_Class *fat){
	string parent,filename;
	ParentAndFile(target,parent,filename);
	return copy_File(parent.c_str(),filename.c_str(),myfile,fat);
}

unsigned int copy_File(string parent,string file_name,File *myfile,FAT_Class *fat){

	string fullpath=parent[parent.length()-1]=='/'?parent+file_name:parent+"/"+file_name;
	inode_Class *node=touch(file_name.c_str(),fat);
	if(!node)
		return 0;
	unsigned int end=0;
	char* buffer;
	int fd=open(fullpath.c_str(), O_RDONLY,0777);


	end=lseek(fd,0,SEEK_END);
	lseek(fd,0,SEEK_SET);
	if (end){//if the file not empty
		buffer=new char[end];
		read(fd,buffer,end);
		node->Number_Blocks=ceil((float)(end)/myfile->SuperBlock->Data_blocksize);
		unsigned int*blocks=myfile->FBL->get_blocks(node->Number_Blocks);
		node->First_Block_Number=blocks[0];//get the blocks from the fbl
		delete[]blocks;
		myfile->as_write(node,buffer,end);
		delete[] buffer;
		close(fd);
	}

	node->write_inode(myfile,myfile->SuperBlock,myfile->FIT,myfile->FBL);
	myfile->FIT->sync_FIT(myfile,myfile->SuperBlock,myfile->FBL);
	myfile->FBL->sync_FBL();

	return end;
}



unsigned int COPY_D_IN(string target, File *myfile,FAT_Class *fat){
	string parent,filename;
	ParentAndFile(target,parent,filename);
	unsigned int count=copy_DIR(parent,filename,myfile,fat);
	cd("..",fat);
	return count;
}

unsigned int copy_DIR(string parent,string DIR_name,File *myfile,FAT_Class *fat){
	unsigned int bytes=0;
	string fullpath=parent[parent.length()-1]=='/'?parent+DIR_name:parent+"/"+DIR_name;
	inode_Class *target=NULL;
	DIR *dptr;
	if(!(dptr = opendir(fullpath.c_str()))){//the file not found
		cout<<fullpath<<": no such file or directory"<<endl;
		return 0;
	}
	else if(!(target=mkdir(DIR_name.c_str(),fat)))
		return 0;

	cd(DIR_name.c_str(),fat);//go inside the folder to paste the other files in it

	struct dirent *dirp;
	string x;

	while ((dirp = readdir(dptr)))
		{
			x=dirp->d_name;
			if(x[0]=='.'||x[x.length()-1]=='~') continue;

			/*
			 * dirp->d_type==4 //folder
			 * dirp->d_type==8 //file
			 */

			if(dirp->d_type==4){
				bytes+=copy_DIR(fullpath,x,myfile,fat);
				cd("..",fat);
			}
			else if(dirp->d_type==8){
				bytes+=copy_File(fullpath,x,myfile,fat);

			}
		}

	closedir(dptr);
	target->write_inode(myfile,myfile->SuperBlock,myfile->FIT,myfile->FBL);
	myfile->FIT->sync_FIT(myfile,myfile->SuperBlock,myfile->FBL);
	myfile->FBL->sync_FBL();
	return bytes;
}







unsigned int copy_File_out(string distination,FAT_Class::Link target,File *myfile,FAT_Class *fat){
		string FullPathWithName=distination[distination.length()-1]=='/'?distination+(string)target->inode.FileName:distination+"/"+(string)target->inode.FileName;
		int fd=open(FullPathWithName.c_str(),O_CREAT | O_RDWR ,0777	);
		assert(fd);
		unsigned int actualSize=((target->inode.Number_Blocks?(target->inode.Number_Blocks==1?0:target->inode.Number_Blocks-1):0)*myfile->SuperBlock->Data_blocksize)+(target->inode.EndingOffset);
		void*buffer=new char[actualSize];
		myfile->as_read(&target->inode,buffer);
		write(fd,buffer,actualSize);
		delete[](char*)buffer;
		close(fd);
		return actualSize;
}

unsigned int copy_DIR_out(string distination,FAT_Class::Link target,File *myfile,FAT_Class *fat){
	unsigned int bytes=0;
	string FullPathWithName=distination[distination.length()-1]=='/'?distination+(string)target->inode.FileName:distination+"/"+(string)target->inode.FileName;
	if(mkdir(FullPathWithName.c_str(),0777)<0){
		cout<<"given destination doesn't exist"<<endl;
		return 0;
	}
	FAT_Class::Link temp;
	bool flag=target->childern.first(temp);
	while(flag){
		if(temp->inode.Mode=='f')
			bytes+=copy_File_out(FullPathWithName,temp,myfile,fat);
		else
			bytes+=copy_DIR_out(FullPathWithName,temp,myfile,fat);
		flag=target->childern.next(temp);
	}
	return bytes;
}


unsigned int copy_File(string distination,FAT_Class::Link target,File *myfile,FAT_Class *fat){
	string filename=(string)target->inode.FileName;
	if(cd_by_path(distination,fat)){//go to the distination
		unsigned int actualSize=((target->inode.Number_Blocks?(target->inode.Number_Blocks==1?0:target->inode.Number_Blocks-1):0)*myfile->SuperBlock->Data_blocksize)+(target->inode.EndingOffset);

		if(target->inode.Number_Blocks){//if there is a data to be written
		inode_Class *node=touch(filename.c_str(),fat);//touch the file
		if(!node)
			return 0;
		node->Number_Blocks=target->inode.Number_Blocks;//get the same number of blocks
		unsigned int*blocks=myfile->FBL->get_blocks(node->Number_Blocks);
		node->First_Block_Number=blocks[0];//get the blocks from the fbl
		delete[]blocks;
		node->write_inode(myfile,myfile->SuperBlock,myfile->FIT,myfile->FBL);//write the new inode to the file

		void*buffer;
		myfile->as_read(&target->inode,buffer);//get the data of the target
		myfile->as_write(node,buffer,actualSize);//paste it to the distination file
		}

		myfile->FIT->sync_FIT(myfile,myfile->SuperBlock,myfile->FBL);
		myfile->FBL->sync_FBL();
		return actualSize;
	}
	else
		return 0 ;
}
unsigned int copy_DIR(string distination,FAT_Class::Link target,File *myfile,FAT_Class *fat){
	unsigned int bytes=0;
	string DIRname=(string)target->inode.FileName;
	if(cd_by_path(distination,fat)){//go to the distination
		inode_Class *node=mkdir(DIRname.c_str(),fat);//touch the file
		if(!node)
			return 0;
		string FullPathWithName=distination[distination.length()-1]=='/'?distination+DIRname:distination+"/"+DIRname;
		FAT_Class::Link temp;
		bool flag=target->childern.first(temp);
		while(flag){
			if(temp->inode.Mode=='f')
				bytes+=copy_File(FullPathWithName,temp,myfile,fat);
			else
				bytes+=copy_DIR(FullPathWithName,temp,myfile,fat);
			flag=target->childern.next(temp);
		}
		return bytes;
	}
	else
		return 0;
}


bool mv(string distination,FAT_Class::Link target,File *myfile,FAT_Class *fat){
	if(cd_by_path(distination,fat)){
		fat->InsertEntity(target);return true;
	}
	else return false;
}
void rm(string target,FAT_Class *FAT){
	string parent,filename=target;
	ParentAndFile(target,parent,filename);
	if(!parent.empty())
		cd_by_path((char*)parent.c_str(),FAT);
	if(!FAT->DeleteWithName(filename.c_str()))
		cout<<"rm: cannot remove ‘"<<filename<<"’: No such file or directory"<<endl;
}
void cat(string target,FAT_Class *FAT){
	string parent,filename=target;
	ParentAndFile(target,parent,filename);
	if(!parent.empty())
		cd_by_path((char*)parent.c_str(),FAT);
	FAT_Class::Link file=FAT->Search(filename.c_str());
	if(file){
		if(file->inode.Mode=='d')
			cout<<"cat: cannot open ‘"<<filename<<"’: is not a file"<<endl;
		else{
			if(file->inode.Number_Blocks){//if the file has any data
				void*buffer;
				FAT->MyFile->as_read(&file->inode,buffer);
				cout<<(char*)buffer<<endl;
				delete[] (char*)buffer;
			}
		}
	}
	else
		cout<<"cat: cannot open ‘"<<filename<<"’: No such file or directory"<<endl;
}



inode_Class* touch(const char * filename,FAT_Class *FAT){
	FAT_Class::Link temp=FAT->CreateEntity(filename,'f');
	if(!temp){
		cout<<"touch: cannot create '"<<filename<<"' a file with the same name exists choose another name for the file"<<endl;
		return NULL;
	}
	else
		return	&(temp->inode);

}
inode_Class* mkdir(const char * filename,FAT_Class *FAT){
	inode_Class *node;
	node=&(FAT->CreateEntity(filename,'d'))->inode;
	if(!node){
		cout<<"mkdir: cannot create '"<<filename<<"' a directory with the same name exists choose another name for the directory"<<endl;
		return NULL;
		}
	else
		return	node;

}


void cd(const char * filename,FAT_Class *FAT){
	FAT->Change_Directory(filename);
}


bool cd_by_path(string distination,FAT_Class *FAT){
	if(distination=="")
		return true;
	if(distination=="/"){
		FAT->cd=FAT->root;
		return true;
	}
	string parent,DIRname;
	ParentAndFile(distination,parent,DIRname);
	bool flag=1;
	if(!(parent==""))
		flag&=cd_by_path(parent,FAT);

	flag&=FAT->Change_Directory(DIRname.c_str());
	return flag;
}


void pwdUpdate(string &pwd,FAT_Class &fat){
	Stack <string> temp;
	FAT_Class::Link target=fat.cd;
	while(target){
		temp.push((string)target->inode.FileName);
		target=target->parent;
	}
	string dir;
	temp.pop(pwd);
	while(!temp.isEmpty()){
		temp.pop(dir);
		pwd+=dir+"/";
	}
}



void ParentAndFile(string target,string &parent ,string &filename){
	target[target.length()-1]=target[target.length()-1]=='/'?'\0':target[target.length()-1];
	filename=target;
	string token,delimiter = "/";;
	int pos = 0;
	while ((pos = filename.find(delimiter)) != std::string::npos) {
	    token = filename.substr(0, pos);
	    parent+=token+"/";
	    filename.erase(0, pos + delimiter.length());
	}
}





