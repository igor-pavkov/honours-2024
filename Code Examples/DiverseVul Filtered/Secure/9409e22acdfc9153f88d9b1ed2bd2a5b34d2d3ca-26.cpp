static int unlazy_walk(struct nameidata *nd, struct dentry *dentry, unsigned seq)
{
	struct dentry *parent = nd->path.dentry;

	BUG_ON(!(nd->flags & LOOKUP_RCU));

	nd->flags &= ~LOOKUP_RCU;
	if (unlikely(!legitimize_links(nd)))
		goto out2;
	if (unlikely(!legitimize_mnt(nd->path.mnt, nd->m_seq)))
		goto out2;
	if (unlikely(!lockref_get_not_dead(&parent->d_lockref)))
		goto out1;

	/*
	 * For a negative lookup, the lookup sequence point is the parents
	 * sequence point, and it only needs to revalidate the parent dentry.
	 *
	 * For a positive lookup, we need to move both the parent and the
	 * dentry from the RCU domain to be properly refcounted. And the
	 * sequence number in the dentry validates *both* dentry counters,
	 * since we checked the sequence number of the parent after we got
	 * the child sequence number. So we know the parent must still
	 * be valid if the child sequence number is still valid.
	 */
	if (!dentry) {
		if (read_seqcount_retry(&parent->d_seq, nd->seq))
			goto out;
		BUG_ON(nd->inode != parent->d_inode);
	} else {
		if (!lockref_get_not_dead(&dentry->d_lockref))
			goto out;
		if (read_seqcount_retry(&dentry->d_seq, seq))
			goto drop_dentry;
	}

	/*
	 * Sequence counts matched. Now make sure that the root is
	 * still valid and get it if required.
	 */
	if (nd->root.mnt && !(nd->flags & LOOKUP_ROOT)) {
		if (unlikely(!legitimize_path(nd, &nd->root, nd->root_seq))) {
			rcu_read_unlock();
			dput(dentry);
			return -ECHILD;
		}
	}

	rcu_read_unlock();
	return 0;

drop_dentry:
	rcu_read_unlock();
	dput(dentry);
	goto drop_root_mnt;
out2:
	nd->path.mnt = NULL;
out1:
	nd->path.dentry = NULL;
out:
	rcu_read_unlock();
drop_root_mnt:
	if (!(nd->flags & LOOKUP_ROOT))
		nd->root.mnt = NULL;
	return -ECHILD;
}