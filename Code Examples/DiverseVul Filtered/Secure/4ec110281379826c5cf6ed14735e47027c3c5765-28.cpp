static ssize_t ext4_quota_write(struct super_block *sb, int type,
				const char *data, size_t len, loff_t off)
{
	struct inode *inode = sb_dqopt(sb)->files[type];
	ext4_lblk_t blk = off >> EXT4_BLOCK_SIZE_BITS(sb);
	int err = 0;
	int offset = off & (sb->s_blocksize - 1);
	int tocopy;
	int journal_quota = EXT4_SB(sb)->s_qf_names[type] != NULL;
	size_t towrite = len;
	struct buffer_head *bh;
	handle_t *handle = journal_current_handle();

	if (EXT4_SB(sb)->s_journal && !handle) {
		printk(KERN_WARNING "EXT4-fs: Quota write (off=%llu, len=%llu)"
			" cancelled because transaction is not started.\n",
			(unsigned long long)off, (unsigned long long)len);
		return -EIO;
	}
	mutex_lock_nested(&inode->i_mutex, I_MUTEX_QUOTA);
	while (towrite > 0) {
		tocopy = sb->s_blocksize - offset < towrite ?
				sb->s_blocksize - offset : towrite;
		bh = ext4_bread(handle, inode, blk, 1, &err);
		if (!bh)
			goto out;
		if (journal_quota) {
			err = ext4_journal_get_write_access(handle, bh);
			if (err) {
				brelse(bh);
				goto out;
			}
		}
		lock_buffer(bh);
		memcpy(bh->b_data+offset, data, tocopy);
		flush_dcache_page(bh->b_page);
		unlock_buffer(bh);
		if (journal_quota)
			err = ext4_handle_dirty_metadata(handle, NULL, bh);
		else {
			/* Always do at least ordered writes for quotas */
			err = ext4_jbd2_file_inode(handle, inode);
			mark_buffer_dirty(bh);
		}
		brelse(bh);
		if (err)
			goto out;
		offset = 0;
		towrite -= tocopy;
		data += tocopy;
		blk++;
	}
out:
	if (len == towrite) {
		mutex_unlock(&inode->i_mutex);
		return err;
	}
	if (inode->i_size < off+len-towrite) {
		i_size_write(inode, off+len-towrite);
		EXT4_I(inode)->i_disksize = inode->i_size;
	}
	inode->i_mtime = inode->i_ctime = CURRENT_TIME;
	ext4_mark_inode_dirty(handle, inode);
	mutex_unlock(&inode->i_mutex);
	return len - towrite;
}