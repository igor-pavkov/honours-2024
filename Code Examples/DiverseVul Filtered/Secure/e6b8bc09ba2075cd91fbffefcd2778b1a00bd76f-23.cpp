static inline unsigned dx_root_limit(struct inode *dir, unsigned infosize)
{
	unsigned entry_space = dir->i_sb->s_blocksize - EXT4_DIR_REC_LEN(1) -
		EXT4_DIR_REC_LEN(2) - infosize;
	return entry_space / sizeof(struct dx_entry);
}