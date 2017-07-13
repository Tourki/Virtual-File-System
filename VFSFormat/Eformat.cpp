///*
// * format.cpp
// *
// *  Created on: Jun 20, 2017
// *      Author: muhammad
// */
//
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <cmath>
#include "SuperBlock.h"
#include "FBL.h"
#include "FIT.h"
#include "Inode.h"
#include "File.h"


#include <fstream>
using namespace std;

int main (int argc ,char *argv[]){

	ifstream ifs(argv[1]);
	SuperBlock_Class SB;
	unsigned long num_of_inodes=1000,Block_size=atoi(argv[2]),num_of_blocks,begin,
			end,size_of_the_file,super_block,fit,fat,fbl,data;


	/***************getting the file size***************/
		begin=ifs.tellg();
		ifs.seekg(0,ios::end);
		end=ifs.tellg();
		size_of_the_file=(end-begin);
		ifs.close();

	/***************getting blocks size ***************/
		super_block=sizeof(SuperBlock_Class);	cout<<"SUPER BLOCK SIZE = "<<super_block<<endl;
		fit=sizeof(unsigned int)*num_of_inodes;cout<<"FIT SIZE = "<<fit<<endl;
		fat=sizeof(inode_Class)*num_of_inodes;cout<<"fat SIZE = "<<fat<<endl;
		num_of_blocks=(size_of_the_file-(super_block+fit+fat))/(sizeof(int)+Block_size);
		fbl=sizeof(int)*(num_of_blocks+1); cout<<"fbl size = "<<fbl<<endl;
		data=num_of_blocks*Block_size;	cout<<"data SIZE = "<<data<<endl;

	/*************setting super_block data ***********/
		cout << "size of the file= " << (end-begin) << " byte" << endl;

		SB.Blocks_Number=num_of_blocks;	cout<<"blocks numbers = "<<num_of_blocks<<endl;
		SB.FIT_Start=super_block+1;		cout<<"FIT START = "<<SB.FIT_Start<<endl;
		SB.FIT_END=	SB.FIT_Start+fit;	cout<<"FIT END = "<<SB.FIT_END<<endl;
		SB.FAT_Start=SB.FIT_END+1;		cout<<"FAT START = "<<SB.FAT_Start<<endl;
		SB.FAT_END=SB.FAT_Start+fat;	cout<<"FAT END = "<<SB.FAT_END<<endl;
		SB.FBL_Start=SB.FAT_END+1;		cout<<"FBL START = "<<SB.FBL_Start<<endl;
		SB.FBL_END=SB.FBL_Start+fbl; 	cout<<"Fbl END = "<<SB.FBL_END<<endl;
		SB.DataBlocks_Start=SB.FBL_END+((1-(((float)SB.FBL_END/Block_size)-(SB.FBL_END/Block_size)))*Block_size);		cout<<"DATA BLOCKS START = "<<SB.DataBlocks_Start<<endl;
		SB.DataBlocks_END=SB.DataBlocks_Start+data;			cout<<"DATA BLOCKS END = "<<SB.DataBlocks_END<<endl;
		SB.Data_blocksize=Block_size;
		SB.FreeBlocks_Number=num_of_blocks;
		SB.inode_Numbers=num_of_inodes;
		SB.Extension_NumOfBlocks=ceil(float((5*(num_of_inodes)+sizeof(unsigned int)))/(Block_size));
		SB.first_Extension_BlockNum=0;
		SB.First_Extension=0;
		SB.extension_nums=0;
		SB.Current_inodesNum=1;		//there is always an inode for the root

	/********************sync super_block***********************/

		unsigned int fd=open(argv[1],O_RDWR,0777);
		write(fd,&SB,sizeof(SB));
		close(fd);

	/************************************************************/

		File myfile(argv[1]);

	/****************settig the root inode***********************/
		inode_Class root;

		time_t rawtime; //time of the ceation is the time of the format
		struct tm* timeinfo;
		time(&rawtime);
		timeinfo=localtime(&rawtime);
		strftime(root.CreationTime,sizeof(root.CreationTime),"%d-%m-%y %I:%M:%S",timeinfo);

		strcpy(root.FileName,"/");
		root.Mode='d';
		root.inode_number=0;
		root.EndingOffset=0;
		root.First_Block_Number=0;
		root.Number_Blocks=0;
		root.write_inode(&myfile,&SB,myfile.FIT,myfile.FBL);//write the root inode to the file
	/************************ syncing the FBL and FIT *******************************/

		FBL_Class FBL(&SB,&myfile);
		FBL.sync_FBL();
		FIT_Class FIT(&SB,&FBL);
		FIT.sync_FIT(&myfile,&SB,myfile.FBL);



}
