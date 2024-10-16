struct inode *inode_insert5(struct inode *inode, unsigned long hashval,
			    int (*test)(struct inode *, void *),
			    int (*set)(struct inode *, void *), void *data)
{
	struct hlist_head *head = inode_hashtable + hash(inode->i_sb, hashval);
	struct inode *old;

again:
	spin_lock(&inode_hash_lock);
	old = find_inode(inode->i_sb, head, test, data);
	if (unlikely(old)) {
		/*
		 * Uhhuh, somebody else created the same inode under us.
		 * Use the old inode instead of the preallocated one.
		 */
		spin_unlock(&inode_hash_lock);
		wait_on_inode(old);
		if (unlikely(inode_unhashed(old))) {
			iput(old);
			goto again;
		}
		return old;
	}

	if (set && unlikely(set(inode, data))) {
		inode = NULL;
		goto unlock;
	}

	/*
	 * Return the locked inode with I_NEW set, the
	 * caller is responsible for filling in the contents
	 */
	spin_lock(&inode->i_lock);
	inode->i_state |= I_NEW;
	hlist_add_head(&inode->i_hash, head);
	spin_unlock(&inode->i_lock);
unlock:
	spin_unlock(&inode_hash_lock);

	return inode;
}