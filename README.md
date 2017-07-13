# Virtual-File-System
designing a virtual file system (VFS) supporting operations of adding/deleting files, the system
stores the files on an actual file on the hard disk.
the system allow a user to:
  1- Copy files and folders from hard disk to VFS and from VFS to the hard disk.
  2- Remove folders and files.
  3- List files, their size, creation dates.
  4- Measure the throughput of the copying operations.
the project consist of number of classes 
  1- File class : used to read and write data to the VFS all the other classes write in the VFS file using this class.
  2- FAT class : used to put the files and folders in a tree to make it easy to go throw the files, change the current
    working directory , list the files and so on.
  3- SuperBlock class : used to store and manage information about the file system like the free block numbers the starting
    and ending address of every part of the file system , number of the current inodes ,free data blocks and so on.
  4- FIT class : or Free inodes table keep track of the available inode numbers which inode number is free to be used and which 
      is not.
  5- Inode class : every file or a directory has its meta data ( name,first data block number,nnumber of allocated data blocks
      creation date,mode "file or directoy" ) stored in an inode which is stored in the file system and loaded at the mount of 
      the VFS to the FAT class.
  6- FBL class : keep track of the free data blocks and also give a file blocks seqence using the first block.
  7- Extention class : used to extend the FIT when it is ran out of inode numbers to give.
  8- Function File : contain the code of the main function to be excuted in the virtual shell after mounting the VFS.
  9- EVFS File : contain the mount code to be excuted to mount the VFS and start the virtual shell.
  10- othe data structure classes used in the above classes and files like Linkec list , queue and stack.
  
 the system provide the following commands:
  1- "cp -i sources distination " : copy files and directories TO VFS.
  2- "cp -o sources distination " : copy files and directories FROM VFS.
  3- "cp    sources distination " : copy files and directories inside the VFS.
  4- "mv -i sources distination " : move files and directories TO VFS.
  5- "mv -o sources distination " : move files and directories FROM VFS.
  6- "mv    sources distination " : move files and directory inside the VFS.
  7- "rm    targets             " : remove files and directories.
  8- "cd    target              " : change the current directory to the target directory.
  9- "ls    target              " : list my current directory files and directories.
  10-"cat   target              " : show the content of the target file.
