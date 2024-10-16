static inline bool d_is_fallthru(const struct dentry *dentry)
{
	return dentry->d_flags & DCACHE_FALLTHRU;
}