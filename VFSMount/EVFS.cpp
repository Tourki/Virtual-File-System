//============================================================================
// Name        : 1.cpp
// Author      : Zizo
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <stdio.h>
#include "FBL.h"
#include "File.h"
#include "Inode.h"
#include "FAT.h"
#include "SuperBlock.h"
#include "Functions.h"
#include "stack.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;

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


int main(int argc,char**argv) {

	/********************copy code******************/
	/*
	 * dir
	 * dir in dir
	 * file
	 *
	 * check if the file is already exist in the same directory
	 * if not
	 * create inode
	 * check file or directory
	 * set all its variables
	 * allocate the required blocks
	 * get all it's data into a buffferwrite the buffer into the data blocks allocated
	 *
	 *for files "from my system to VFS	"
	 *		cp Source Directory
	 *
	 */

	File myfile(argv[1]);
	FAT_Class fat(&myfile);
	FAT_Class::Link current;
	string SOURCE,DISTENATION;

	string input,sub,token,delimiter=" ",*argu_values;
	int argu_num;
	string pwdString;
	pwdUpdate(pwdString,fat);
	while(1){
		cout<<pwdString<<"$ ";
		argu_num=0;
		getline(cin,input);
		stringstream ss(input);
		stringstream qq(input);
		while(ss>>sub)
			argu_num++;
		argu_values=new string[argu_num];
		argu_num=0;
		while(qq>>argu_values[argu_num])
			argu_num++;
		if(!argu_num);//just enter go on
		else if(argu_values[0]=="cp"){
			if(argu_values[1]=="-i"){//copy in
				/*
				 *
				 * handle errors here
				 * copy multiple files in one command
				 * cpin /home/muhammad/file3 /h
				 * cpin /home/muhammad/h/ /home/muhammad/trial /home/muhammad/trial/file3 /
				 *
				 */
				if(argu_num < 4){
					cout<<"cp: too few arguments"<<endl;
					continue;
				}
				else {
					unsigned int bytes=0;
					time_t start,end;
					time (&start);
					for (int i = 2; i < argu_num-1; i++) {
						current=fat.cd;

						SOURCE=argu_values[i];
						DISTENATION=argu_values[argu_num-1];

						struct stat path_stat;//used for getting mode of file
						stat(SOURCE.c_str(), &path_stat);

						if(cd_by_path(DISTENATION,&fat)){//if the path is found then go on
							if(S_ISREG(path_stat.st_mode)==1)//0 Dir 1 File
								bytes+=COPY_F_IN(SOURCE,&myfile,&fat);
							else if(S_ISREG(path_stat.st_mode)==0)
								bytes+=COPY_D_IN(SOURCE,&myfile,&fat);
							else
								cout<<"cp: "<<SOURCE<<" : no such file or directory"<<endl;
						}
						else//Distination not found
							cout<<"cp: "<<DISTENATION<<": no such directory"<<endl;
						fat.cd=current;
					}
					time (&end);
					allsize data(bytes);
					cout<<"Elapsed Time: "<<end-start<<" sec"<<endl;
					cout<<"Average speed: "<<(float)data.value/(end-start)<<" "<<data.symbol<<"/sec"<<endl;
				}
			}

			else if(argu_values[1]=="-o"){ //copy out
				if(argu_num < 4){
					cout<<"cp: too few arguments"<<endl;
					continue;
				}
				else{
					unsigned int bytes=0;
					time_t start,end;
					time (&start);
					for (int i = 2; i < argu_num-1; i++) {
						current=fat.cd;

						SOURCE=argu_values[i];
						DISTENATION=argu_values[argu_num-1];

						string parent,filename;
						ParentAndFile(SOURCE,parent,filename);

						if(cd_by_path(parent,&fat)){//if the path is found then go on
							FAT_Class::Link target=fat.Search(filename.c_str());
							if(target){//0 Dir 1 File
								if(target->inode.Mode=='f')
									bytes+=copy_File_out(DISTENATION,target,&myfile,&fat);
								else
									bytes+=copy_DIR_out(DISTENATION,target,&myfile,&fat);
							}
							else
								cout<<"cp: "<<filename<<" : no such file or directory at"<<parent<<endl;
						}
						else
							cout<<"cp: "<<SOURCE<<" : no such file or directory at"<<endl;

						fat.cd=current;
					}
					time (&end);
					cout<<"Elapsed Time: "<<end-start<<" sec"<<endl;
					allsize data(bytes);
					cout<<"Average speed: "<<(float)data.value/(end-start)<<" "<<data.symbol<<"/sec"<<endl;
				}

			}

			else{//copy inside the vfs
				if(argu_num < 3){
					cout<<"cp: too few arguments"<<endl;
					continue;
				}
				else{
					unsigned int bytes=0;
					time_t start,end;
					time (&start);
					for (int i = 1; i < argu_num-1; i++) {
						current=fat.cd;

						SOURCE=argu_values[i];
						DISTENATION=argu_values[argu_num-1];

						string parent,filename;
						ParentAndFile(SOURCE,parent,filename);

						if(cd_by_path(parent,&fat)){//if the path is found then go on
							FAT_Class::Link target=fat.Search(filename.c_str());
							fat.cd=current;
							if(target){//0 Dir 1 File
								if(target->inode.Mode=='f')
									bytes+=copy_File(DISTENATION,target,&myfile,&fat);
								else
									bytes+=copy_DIR(DISTENATION,target,&myfile,&fat);
							}
							else
								cout<<"cp: "<<filename<<" : no such file or directory at"<<parent<<endl;
						}
						else
							cout<<"cp: "<<SOURCE<<" : no such file or directory at"<<endl;

						fat.cd=current;
					}
					time (&end);
					cout<<"Elapsed Time: "<<end-start<<" sec"<<endl;
					allsize data(bytes);
					cout<<"Average speed: "<<(float)data.value/(end-start)<<" "<<data.symbol<<"/sec"<<endl;
				}

			}



		}




		else if(argu_values[0]=="mv"){
			if(argu_values[1]=="-i"){
				/*
				 *
				 * handle errors here
				 * copy multiple files in one command
				 * cpin /home/muhammad/file3 /h
				 * cpin /home/muhammad/h/ /home/muhammad/trial /home/muhammad/trial/file3 /
				 *
				 */
				if(argu_num < 4){
					cout<<"mv: too few arguments"<<endl;
					continue;
				}
				else {
					unsigned int bytes=0;
					time_t start,end;
					time (&start);
					for (int i = 2; i < argu_num-1; i++) {
						current=fat.cd;

						SOURCE=argu_values[i];
						DISTENATION=argu_values[argu_num-1];

						struct stat path_stat;//used for getting mode of file
						stat(SOURCE.c_str(), &path_stat);
						string command="rm -rf "+SOURCE;
						if(cd_by_path(DISTENATION,&fat)){//if the path is found then go on
							if(S_ISREG(path_stat.st_mode)==1){//0 Dir 1 File
								bytes+=COPY_F_IN(SOURCE,&myfile,&fat);
								system(command.c_str());
							}
							else if(S_ISREG(path_stat.st_mode)==0){
								bytes+=COPY_D_IN(SOURCE,&myfile,&fat);
								system(command.c_str());
							}
							else
								cout<<"cp: "<<SOURCE<<" : no such file or directory"<<endl;
						}
						else//Distination not found
							cout<<"cp: "<<DISTENATION<<": no such directory"<<endl;
						fat.cd=current;
					}
					time (&end);
					cout<<"Elapsed Time: "<<end-start<<" sec"<<endl;
					allsize data(bytes);
					cout<<"Average speed: "<<(float)data.value/(end-start)<<" "<<data.symbol<<"/sec"<<endl;
				}
			}

			else if(argu_values[1]=="-o"){
				if(argu_num < 4){
					cout<<"mv: too few arguments"<<endl;
					continue;
				}
				else{
					unsigned int bytes=0;
					time_t start,end;
					time (&start);
					for (int i = 2; i < argu_num-1; i++) {
						current=fat.cd;

						SOURCE=argu_values[i];
						DISTENATION=argu_values[argu_num-1];

						string parent,filename;
						ParentAndFile(SOURCE,parent,filename);

						if(cd_by_path(parent,&fat)){//if the path is found then go on
							FAT_Class::Link target=fat.Search(filename.c_str());
							if(target){//0 Dir 1 File
								if(target->inode.Mode=='f')
									bytes+=copy_File_out(DISTENATION,target,&myfile,&fat);
								else
									bytes+=copy_DIR_out(DISTENATION,target,&myfile,&fat);
								if(bytes)fat.DeleteEntity(target);//the difference between cpout and mvout is the delete part
							}
							else
								cout<<"mv: "<<filename<<" : no such file or directory at"<<parent<<endl;
						}
						else
							cout<<"mv: "<<SOURCE<<" : no such file or directory at"<<endl;


						fat.cd=current;
					}
					time (&end);
					cout<<"Elapsed Time: "<<end-start<<" sec"<<endl;
					allsize data(bytes);
					cout<<"Average speed: "<<(float)data.value/(end-start)<<" "<<data.symbol<<"/sec"<<endl;
				}

			}

			else{//mv inside the file system
				if(argu_num < 3){
					cout<<"mv: too few arguments"<<endl;
					continue;
				}
				else{
					unsigned int bytes=0;
					time_t start,end;
					time (&start);
					for (int i = 1; i < argu_num-1; i++) {
						current=fat.cd;

						SOURCE=argu_values[i];
						DISTENATION=argu_values[argu_num-1];

						string parent,filename;
						ParentAndFile(SOURCE,parent,filename);

						if(cd_by_path(parent,&fat)){//if the path is found then go on
							FAT_Class::Link target=fat.Search(filename.c_str());
							fat.cd=current;
							if(target){//0 Dir 1 File
								if(!mv(DISTENATION,target,&myfile,&fat))
									cout<<"mv: "<<DISTENATION<<" : no such file or directory"<<endl;
							}
							else
								cout<<"mv: "<<filename<<" : no such file or directory at"<<parent<<endl;
						}
						else
							cout<<"mv: "<<SOURCE<<" : no such file or directory at"<<endl;


						fat.cd=current;
					}
					time (&end);
					cout<<"Elapsed Time: "<<end-start<<" sec"<<endl;
					allsize data(bytes);
					cout<<"Average speed: "<<(float)data.value/(end-start)<<" "<<data.symbol<<"/sec"<<endl;
				}


			}

		}



		else if(argu_values[0]=="rm"){
			for (int i = 1; i < argu_num; i++) {
				current=fat.cd;
				string Distination=argu_values[i];
				rm(Distination,&fat);
				fat.cd=current;
			}

		}
		else if(argu_values[0]=="touch"){
			if(argu_num<2)
				cout<<"too few argument"<<endl;
			else{
				for (int i = 1; i < argu_num; i++) {
							current=fat.cd;

							SOURCE=argu_values[i];

							string parent,filename;
							ParentAndFile(SOURCE,parent,filename);

							if(cd_by_path(parent,&fat)){//if the path is found then go on
								touch(filename.c_str(),&fat);
							}
							else
								cout<<"touch: "<<SOURCE<<" : no such file or directory at"<<endl;

							fat.cd=current;
						}
			}
		}

		else if(argu_values[0]=="mkdir"){
			if(argu_num<2)
				cout<<"too few argument"<<endl;
			else{
				for (int i = 1; i < argu_num; i++) {
							current=fat.cd;

							SOURCE=argu_values[i];

							string parent,filename;
							ParentAndFile(SOURCE,parent,filename);

							if(cd_by_path(parent,&fat)){//if the path is found then go on
								mkdir(filename.c_str(),&fat);
							}
							else
								cout<<"mkdir: "<<SOURCE<<" : no such file or directory at"<<endl;

							fat.cd=current;
						}
			}
		}

		else if(argu_values[0]=="ls"){
			if(argu_num==1){
				fat.List();
			}
			else{
				/*
				 * error list even if the path is wrong
				 */
				current=fat.cd;

				DISTENATION=argu_values[1];
				if(cd_by_path(DISTENATION,&fat))//if the path found then ls
					fat.List();
				else
					cout<<"ls: "<<DISTENATION<<" : no such file or directory"<<endl;
				fat.cd=current;
			}
		}

		else if(argu_values[0]=="cat"){
			if(argu_num < 2){
				cout<<"cat: too few arguments"<<endl;
				continue;
			}
			else{
				current=fat.cd;
				cat(argu_values[1],&fat);
				fat.cd=current;
			}
		}

		else if(argu_values[0]=="cd"){
			if(argu_num<2)
				cd_by_path((char*)"/",&fat);
			else{
				string Distination =argu_values[1];
				if(!cd_by_path(Distination,&fat))
					cout<<"cd: "<<Distination<<" : no such file or directory"<<endl;
			}
			pwdUpdate(pwdString,fat);
		}

		else if(argu_values[0]=="exit"){
			if(argu_num==1)
				break;
			cout<<"exit: this command does not take any arguments"<<endl;

		}
		else
			cout<< input<<": command not found"<<endl;
		delete[]argu_values;

	}

	delete[]argu_values;
	myfile.SuperBlock->sync_super(&myfile);
	myfile.FBL->sync_FBL();
	myfile.FIT->sync_FIT(&myfile,myfile.SuperBlock,myfile.FBL);
	return 0;
}
