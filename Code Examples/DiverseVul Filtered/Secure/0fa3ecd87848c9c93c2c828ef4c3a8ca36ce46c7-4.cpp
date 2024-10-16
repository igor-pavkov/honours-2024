struct inode *iget5_locked(struct super_block *sb, unsigned long hashval,
		int (*test)(struct inode *, void *),
		int (*set)(struct inode *, void *), void *data)
{
	struct inode *inode = ilookup5(sb, hashval, test, data);

	if (!inode) {
		struct inode *new = new_inode(sb);

		if (new) {
			inode = inode_insert5(new, hashval, test, set, data);
			if (unlikely(inode != new))
				iput(new);
		}
	}
	return inode;
}