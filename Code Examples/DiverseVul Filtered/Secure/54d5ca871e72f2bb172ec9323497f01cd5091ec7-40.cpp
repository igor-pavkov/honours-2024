static inline bool d_really_is_negative(const struct dentry *dentry)
{
	return dentry->d_inode == NULL;
}