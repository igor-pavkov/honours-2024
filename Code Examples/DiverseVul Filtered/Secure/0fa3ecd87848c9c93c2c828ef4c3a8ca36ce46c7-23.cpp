void inode_add_lru(struct inode *inode)
{
	if (!(inode->i_state & (I_DIRTY_ALL | I_SYNC |
				I_FREEING | I_WILL_FREE)) &&
	    !atomic_read(&inode->i_count) && inode->i_sb->s_flags & SB_ACTIVE)
		inode_lru_list_add(inode);
}