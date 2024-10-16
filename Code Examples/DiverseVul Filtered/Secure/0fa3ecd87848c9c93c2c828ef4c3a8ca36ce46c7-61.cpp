static void iput_final(struct inode *inode)
{
	struct super_block *sb = inode->i_sb;
	const struct super_operations *op = inode->i_sb->s_op;
	int drop;

	WARN_ON(inode->i_state & I_NEW);

	if (op->drop_inode)
		drop = op->drop_inode(inode);
	else
		drop = generic_drop_inode(inode);

	if (!drop && (sb->s_flags & SB_ACTIVE)) {
		inode_add_lru(inode);
		spin_unlock(&inode->i_lock);
		return;
	}

	if (!drop) {
		inode->i_state |= I_WILL_FREE;
		spin_unlock(&inode->i_lock);
		write_inode_now(inode, 1);
		spin_lock(&inode->i_lock);
		WARN_ON(inode->i_state & I_NEW);
		inode->i_state &= ~I_WILL_FREE;
	}

	inode->i_state |= I_FREEING;
	if (!list_empty(&inode->i_lru))
		inode_lru_list_del(inode);
	spin_unlock(&inode->i_lock);

	evict(inode);
}