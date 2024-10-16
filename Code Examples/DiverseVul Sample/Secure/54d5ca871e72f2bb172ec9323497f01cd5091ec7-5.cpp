static inline int simple_positive(struct dentry *dentry)
{
	return d_really_is_positive(dentry) && !d_unhashed(dentry);
}