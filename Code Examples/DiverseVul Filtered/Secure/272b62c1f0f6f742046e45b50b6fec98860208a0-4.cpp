static int ocfs2_try_to_free_truncate_log(struct ocfs2_super *osb,
					  unsigned int needed)
{
	tid_t target;
	int ret = 0;
	unsigned int truncated_clusters;

	mutex_lock(&osb->osb_tl_inode->i_mutex);
	truncated_clusters = osb->truncated_clusters;
	mutex_unlock(&osb->osb_tl_inode->i_mutex);

	/*
	 * Check whether we can succeed in allocating if we free
	 * the truncate log.
	 */
	if (truncated_clusters < needed)
		goto out;

	ret = ocfs2_flush_truncate_log(osb);
	if (ret) {
		mlog_errno(ret);
		goto out;
	}

	if (jbd2_journal_start_commit(osb->journal->j_journal, &target)) {
		jbd2_log_wait_commit(osb->journal->j_journal, target);
		ret = 1;
	}
out:
	return ret;
}