static int ocfs2_zero_extend_get_range(struct inode *inode,
				       struct buffer_head *di_bh,
				       u64 zero_start, u64 zero_end,
				       u64 *range_start, u64 *range_end)
{
	int rc = 0, needs_cow = 0;
	u32 p_cpos, zero_clusters = 0;
	u32 zero_cpos =
		zero_start >> OCFS2_SB(inode->i_sb)->s_clustersize_bits;
	u32 last_cpos = ocfs2_clusters_for_bytes(inode->i_sb, zero_end);
	unsigned int num_clusters = 0;
	unsigned int ext_flags = 0;

	while (zero_cpos < last_cpos) {
		rc = ocfs2_get_clusters(inode, zero_cpos, &p_cpos,
					&num_clusters, &ext_flags);
		if (rc) {
			mlog_errno(rc);
			goto out;
		}

		if (p_cpos && !(ext_flags & OCFS2_EXT_UNWRITTEN)) {
			zero_clusters = num_clusters;
			if (ext_flags & OCFS2_EXT_REFCOUNTED)
				needs_cow = 1;
			break;
		}

		zero_cpos += num_clusters;
	}
	if (!zero_clusters) {
		*range_end = 0;
		goto out;
	}

	while ((zero_cpos + zero_clusters) < last_cpos) {
		rc = ocfs2_get_clusters(inode, zero_cpos + zero_clusters,
					&p_cpos, &num_clusters,
					&ext_flags);
		if (rc) {
			mlog_errno(rc);
			goto out;
		}

		if (!p_cpos || (ext_flags & OCFS2_EXT_UNWRITTEN))
			break;
		if (ext_flags & OCFS2_EXT_REFCOUNTED)
			needs_cow = 1;
		zero_clusters += num_clusters;
	}
	if ((zero_cpos + zero_clusters) > last_cpos)
		zero_clusters = last_cpos - zero_cpos;

	if (needs_cow) {
		rc = ocfs2_refcount_cow(inode, di_bh, zero_cpos,
					zero_clusters, UINT_MAX);
		if (rc) {
			mlog_errno(rc);
			goto out;
		}
	}

	*range_start = ocfs2_clusters_to_bytes(inode->i_sb, zero_cpos);
	*range_end = ocfs2_clusters_to_bytes(inode->i_sb,
					     zero_cpos + zero_clusters);

out:
	return rc;
}