static inline bool __force_buffered_io(struct inode *inode, int rw)
{
	return ((f2fs_encrypted_inode(inode) && S_ISREG(inode->i_mode)) ||
			(rw == WRITE && test_opt(F2FS_I_SB(inode), LFS)) ||
			F2FS_I_SB(inode)->s_ndevs);
}