static inline struct dentry *d_real(struct dentry *dentry)
{
	if (unlikely(dentry->d_flags & DCACHE_OP_REAL))
		return dentry->d_op->d_real(dentry, NULL);
	else
		return dentry;
}