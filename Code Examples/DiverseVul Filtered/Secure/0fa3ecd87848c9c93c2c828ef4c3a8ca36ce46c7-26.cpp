void inode_set_flags(struct inode *inode, unsigned int flags,
		     unsigned int mask)
{
	unsigned int old_flags, new_flags;

	WARN_ON_ONCE(flags & ~mask);
	do {
		old_flags = READ_ONCE(inode->i_flags);
		new_flags = (old_flags & ~mask) | flags;
	} while (unlikely(cmpxchg(&inode->i_flags, old_flags,
				  new_flags) != old_flags));
}