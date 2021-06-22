# Points to Note/Consider for "Our Own FS" (to insert a name in future)
* Minimal requirements of the fs:
	1. Bootsector
	1. Data structure for handling data and addressing info
	1. Actual data segment
* The read-write headers + C files will need to be written which will be exported to the driver
* For easy driver handling, FUSE is a great option but it itself is a huge codebase and hence might have security flaws. However, it can be a good starting point for now.
* Test the same on different OS platforms.


# Random Experimentation with ext2 FS
## Conclusions
*(Note: everything is 0-indexed)*
1. 1024 bytes is boot offset
1. 1024 bytes is super struct
1. Block 0 is from 0-4095 bytes
1. Group descriptor block is block 1 (4096-8191 bytes)
1. Root inode is inode 2 of block group 0
