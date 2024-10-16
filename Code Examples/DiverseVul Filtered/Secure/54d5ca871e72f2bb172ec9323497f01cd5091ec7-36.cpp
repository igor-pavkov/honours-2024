static inline unsigned d_count(const struct dentry *dentry)
{
	return dentry->d_lockref.count;
}