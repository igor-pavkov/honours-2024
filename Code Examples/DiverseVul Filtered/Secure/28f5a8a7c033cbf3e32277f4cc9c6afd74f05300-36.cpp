static int ocfs2_cow_file_pos(struct inode *inode,
			      struct buffer_head *fe_bh,
			      u64 offset)
{
	int status;
	u32 phys, cpos = offset >> OCFS2_SB(inode->i_sb)->s_clustersize_bits;
	unsigned int num_clusters = 0;
	unsigned int ext_flags = 0;

	/*
	 * If the new offset is aligned to the range of the cluster, there is
	 * no space for ocfs2_zero_range_for_truncate to fill, so no need to
	 * CoW either.
	 */
	if ((offset & (OCFS2_SB(inode->i_sb)->s_clustersize - 1)) == 0)
		return 0;

	status = ocfs2_get_clusters(inode, cpos, &phys,
				    &num_clusters, &ext_flags);
	if (status) {
		mlog_errno(status);
		goto out;
	}

	if (!(ext_flags & OCFS2_EXT_REFCOUNTED))
		goto out;

	return ocfs2_refcount_cow(inode, fe_bh, cpos, 1, cpos+1);

out:
	return status;
}