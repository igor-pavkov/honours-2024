int ocfs2_remove_inode_range(struct inode *inode,
			     struct buffer_head *di_bh, u64 byte_start,
			     u64 byte_len)
{
	int ret = 0, flags = 0, done = 0, i;
	u32 trunc_start, trunc_len, trunc_end, trunc_cpos, phys_cpos;
	u32 cluster_in_el;
	struct ocfs2_super *osb = OCFS2_SB(inode->i_sb);
	struct ocfs2_cached_dealloc_ctxt dealloc;
	struct address_space *mapping = inode->i_mapping;
	struct ocfs2_extent_tree et;
	struct ocfs2_path *path = NULL;
	struct ocfs2_extent_list *el = NULL;
	struct ocfs2_extent_rec *rec = NULL;
	struct ocfs2_dinode *di = (struct ocfs2_dinode *)di_bh->b_data;
	u64 blkno, refcount_loc = le64_to_cpu(di->i_refcount_loc);

	ocfs2_init_dinode_extent_tree(&et, INODE_CACHE(inode), di_bh);
	ocfs2_init_dealloc_ctxt(&dealloc);

	trace_ocfs2_remove_inode_range(
			(unsigned long long)OCFS2_I(inode)->ip_blkno,
			(unsigned long long)byte_start,
			(unsigned long long)byte_len);

	if (byte_len == 0)
		return 0;

	if (OCFS2_I(inode)->ip_dyn_features & OCFS2_INLINE_DATA_FL) {
		ret = ocfs2_truncate_inline(inode, di_bh, byte_start,
					    byte_start + byte_len, 0);
		if (ret) {
			mlog_errno(ret);
			goto out;
		}
		/*
		 * There's no need to get fancy with the page cache
		 * truncate of an inline-data inode. We're talking
		 * about less than a page here, which will be cached
		 * in the dinode buffer anyway.
		 */
		unmap_mapping_range(mapping, 0, 0, 0);
		truncate_inode_pages(mapping, 0);
		goto out;
	}

	/*
	 * For reflinks, we may need to CoW 2 clusters which might be
	 * partially zero'd later, if hole's start and end offset were
	 * within one cluster(means is not exactly aligned to clustersize).
	 */

	if (ocfs2_is_refcount_inode(inode)) {
		ret = ocfs2_cow_file_pos(inode, di_bh, byte_start);
		if (ret) {
			mlog_errno(ret);
			goto out;
		}

		ret = ocfs2_cow_file_pos(inode, di_bh, byte_start + byte_len);
		if (ret) {
			mlog_errno(ret);
			goto out;
		}
	}

	trunc_start = ocfs2_clusters_for_bytes(osb->sb, byte_start);
	trunc_end = (byte_start + byte_len) >> osb->s_clustersize_bits;
	cluster_in_el = trunc_end;

	ret = ocfs2_zero_partial_clusters(inode, byte_start, byte_len);
	if (ret) {
		mlog_errno(ret);
		goto out;
	}

	path = ocfs2_new_path_from_et(&et);
	if (!path) {
		ret = -ENOMEM;
		mlog_errno(ret);
		goto out;
	}

	while (trunc_end > trunc_start) {

		ret = ocfs2_find_path(INODE_CACHE(inode), path,
				      cluster_in_el);
		if (ret) {
			mlog_errno(ret);
			goto out;
		}

		el = path_leaf_el(path);

		i = ocfs2_find_rec(el, trunc_end);
		/*
		 * Need to go to previous extent block.
		 */
		if (i < 0) {
			if (path->p_tree_depth == 0)
				break;

			ret = ocfs2_find_cpos_for_left_leaf(inode->i_sb,
							    path,
							    &cluster_in_el);
			if (ret) {
				mlog_errno(ret);
				goto out;
			}

			/*
			 * We've reached the leftmost extent block,
			 * it's safe to leave.
			 */
			if (cluster_in_el == 0)
				break;

			/*
			 * The 'pos' searched for previous extent block is
			 * always one cluster less than actual trunc_end.
			 */
			trunc_end = cluster_in_el + 1;

			ocfs2_reinit_path(path, 1);

			continue;

		} else
			rec = &el->l_recs[i];

		ocfs2_calc_trunc_pos(inode, el, rec, trunc_start, &trunc_cpos,
				     &trunc_len, &trunc_end, &blkno, &done);
		if (done)
			break;

		flags = rec->e_flags;
		phys_cpos = ocfs2_blocks_to_clusters(inode->i_sb, blkno);

		ret = ocfs2_remove_btree_range(inode, &et, trunc_cpos,
					       phys_cpos, trunc_len, flags,
					       &dealloc, refcount_loc, false);
		if (ret < 0) {
			mlog_errno(ret);
			goto out;
		}

		cluster_in_el = trunc_end;

		ocfs2_reinit_path(path, 1);
	}

	ocfs2_truncate_cluster_pages(inode, byte_start, byte_len);

out:
	ocfs2_free_path(path);
	ocfs2_schedule_truncate_log_flush(osb, 1);
	ocfs2_run_deallocs(osb, &dealloc);

	return ret;
}