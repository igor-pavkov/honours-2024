static inline bool d_is_positive(const struct dentry *dentry)
{
	return !d_is_negative(dentry);
}