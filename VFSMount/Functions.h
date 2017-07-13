/*
 * Functions.h
 *
 *  Created on: Jul 3, 2017
 *      Author: tourki
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_
#include <stdio.h>
#include "FBL.h"
#include "File.h"
#include "Inode.h"
#include "FAT.h"
#include "SuperBlock.h"
#include "stack.h"

#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <fstream>
void ParentAndFile(string target,string &parent ,string &filename);
inode_Class* touch(const char * filename,FAT_Class *FAT);
inode_Class* mkdir(const char * filename,FAT_Class *FAT);
void rm(string target,FAT_Class *FAT);
void pwdUpdate(string &pwd,FAT_Class &fat);
void cat(string target,FAT_Class *FAT);
void cd(const char * filename,FAT_Class *FAT);
bool cd_by_path(string x,FAT_Class *FAT);
unsigned int COPY_F_IN(string target, File *myfile,FAT_Class *fat);
unsigned int COPY_D_IN(string target, File *myfile,FAT_Class *fat);
unsigned int copy_File(string parent,string file_name,File *myfile,FAT_Class *fat);
unsigned int copy_DIR(string parent,string DIR_name,File *myfile,FAT_Class *fat);

unsigned int copy_File_out(string distination,FAT_Class::Link target,File *myfile,FAT_Class *fat);
unsigned int copy_DIR_out(string distination,FAT_Class::Link target,File *myfile,FAT_Class *fat);

unsigned int copy_DIR(string distination,FAT_Class::Link target,File *myfile,FAT_Class *fat);
unsigned int copy_File(string distination,FAT_Class::Link target,File *myfile,FAT_Class *fat);

bool mv(string distination,FAT_Class::Link target,File *myfile,FAT_Class *fat);


#endif /* FUNCTIONS_H_ */
