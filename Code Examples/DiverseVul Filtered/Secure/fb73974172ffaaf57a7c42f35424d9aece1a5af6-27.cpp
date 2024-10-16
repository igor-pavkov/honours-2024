static inline u32 open_file_to_av(struct file *file)
{
	u32 av = file_to_av(file);
	struct inode *inode = file_inode(file);

	if (selinux_policycap_openperm() &&
	    inode->i_sb->s_magic != SOCKFS_MAGIC)
		av |= FILE__OPEN;

	return av;
}