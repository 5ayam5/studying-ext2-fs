# Random Experimentation with ext2 FS
## Conclusions
*(Note: everything is 0-indexed)*
1. 1024 bytes is boot offset
1. 1024 bytes is super struct
1. Block 0 is from 0-4095 bytes
1. Group descriptor block is block 1 (4096-8191 bytes)
1. Root inode is inode 2 of block group 0
