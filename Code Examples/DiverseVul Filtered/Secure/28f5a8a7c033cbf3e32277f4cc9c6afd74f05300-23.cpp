static int ocfs2_sync_file(struct file *file, loff_t start, loff_t end,
			   int datasync)
{
	int err = 0;
	struct inode *inode = file->f_mapping->host;
	struct ocfs2_super *osb = OCFS2_SB(inode->i_sb);
	struct ocfs2_inode_info *oi = OCFS2_I(inode);
	journal_t *journal = osb->journal->j_journal;
	int ret;
	tid_t commit_tid;
	bool needs_barrier = false;

	trace_ocfs2_sync_file(inode, file, file->f_path.dentry,
			      OCFS2_I(inode)->ip_blkno,
			      file->f_path.dentry->d_name.len,
			      file->f_path.dentry->d_name.name,
			      (unsigned long long)datasync);

	if (ocfs2_is_hard_readonly(osb) || ocfs2_is_soft_readonly(osb))
		return -EROFS;

	err = file_write_and_wait_range(file, start, end);
	if (err)
		return err;

	commit_tid = datasync ? oi->i_datasync_tid : oi->i_sync_tid;
	if (journal->j_flags & JBD2_BARRIER &&
	    !jbd2_trans_will_send_data_barrier(journal, commit_tid))
		needs_barrier = true;
	err = jbd2_complete_transaction(journal, commit_tid);
	if (needs_barrier) {
		ret = blkdev_issue_flush(inode->i_sb->s_bdev, GFP_KERNEL, NULL);
		if (!err)
			err = ret;
	}

	if (err)
		mlog_errno(err);

	return (err < 0) ? -EIO : 0;
}