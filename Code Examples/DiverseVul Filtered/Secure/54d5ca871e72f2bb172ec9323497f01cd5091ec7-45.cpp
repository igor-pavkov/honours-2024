static inline struct dentry *dget_dlock(struct dentry *dentry)
{
	if (dentry)
		dentry->d_lockref.count++;
	return dentry;
}