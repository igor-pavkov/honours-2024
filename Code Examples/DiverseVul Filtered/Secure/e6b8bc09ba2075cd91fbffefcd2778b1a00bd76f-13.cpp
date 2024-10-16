int ext4_orphan_add(handle_t *handle, struct inode *inode)
{
	struct super_block *sb = inode->i_sb;
	struct ext4_iloc iloc;
	int err = 0, rc;

	if (!ext4_handle_valid(handle))
		return 0;

	lock_super(sb);
	if (!list_empty(&EXT4_I(inode)->i_orphan))
		goto out_unlock;

	/* Orphan handling is only valid for files with data blocks
	 * being truncated, or files being unlinked. */

	/* @@@ FIXME: Observation from aviro:
	 * I think I can trigger J_ASSERT in ext4_orphan_add().  We block
	 * here (on lock_super()), so race with ext4_link() which might bump
	 * ->i_nlink. For, say it, character device. Not a regular file,
	 * not a directory, not a symlink and ->i_nlink > 0.
	 */
	J_ASSERT((S_ISREG(inode->i_mode) || S_ISDIR(inode->i_mode) ||
		  S_ISLNK(inode->i_mode)) || inode->i_nlink == 0);

	BUFFER_TRACE(EXT4_SB(sb)->s_sbh, "get_write_access");
	err = ext4_journal_get_write_access(handle, EXT4_SB(sb)->s_sbh);
	if (err)
		goto out_unlock;

	err = ext4_reserve_inode_write(handle, inode, &iloc);
	if (err)
		goto out_unlock;

	/* Insert this inode at the head of the on-disk orphan list... */
	NEXT_ORPHAN(inode) = le32_to_cpu(EXT4_SB(sb)->s_es->s_last_orphan);
	EXT4_SB(sb)->s_es->s_last_orphan = cpu_to_le32(inode->i_ino);
	err = ext4_handle_dirty_metadata(handle, inode, EXT4_SB(sb)->s_sbh);
	rc = ext4_mark_iloc_dirty(handle, inode, &iloc);
	if (!err)
		err = rc;

	/* Only add to the head of the in-memory list if all the
	 * previous operations succeeded.  If the orphan_add is going to
	 * fail (possibly taking the journal offline), we can't risk
	 * leaving the inode on the orphan list: stray orphan-list
	 * entries can cause panics at unmount time.
	 *
	 * This is safe: on error we're going to ignore the orphan list
	 * anyway on the next recovery. */
	if (!err)
		list_add(&EXT4_I(inode)->i_orphan, &EXT4_SB(sb)->s_orphan);

	jbd_debug(4, "superblock will point to %lu\n", inode->i_ino);
	jbd_debug(4, "orphan inode %lu will point to %d\n",
			inode->i_ino, NEXT_ORPHAN(inode));
out_unlock:
	unlock_super(sb);
	ext4_std_error(inode->i_sb, err);
	return err;
}