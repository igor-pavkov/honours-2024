static int ntfs_attr_fill_hole(ntfs_attr *na, s64 count, s64 *ofs, 
			       runlist_element **rl, VCN *update_from)
{
	s64 to_write;
	s64 need;
	ntfs_volume *vol = na->ni->vol;
	int eo, ret = -1;
	runlist *rlc;
	LCN lcn_seek_from = -1;
	VCN cur_vcn, from_vcn;

	if (na->ni->mft_no == FILE_Bitmap) {
		/*
		 * Filling a hole in the main bitmap implies allocating
		 * clusters, which is likely to imply updating the
		 * bitmap in a cluster being allocated.
		 * Not supported now, could lead to endless recursions.
		 */
		ntfs_log_error("Corrupt $BitMap not fully allocated\n");
		errno = EIO;
		goto err_out;
	}
	to_write = min(count, ((*rl)->length << vol->cluster_size_bits) - *ofs);
	
	cur_vcn = (*rl)->vcn;
	from_vcn = (*rl)->vcn + (*ofs >> vol->cluster_size_bits);
	
	ntfs_log_trace("count: %lld, cur_vcn: %lld, from: %lld, to: %lld, ofs: "
		       "%lld\n", (long long)count, (long long)cur_vcn, 
		       (long long)from_vcn, (long long)to_write, (long long)*ofs);
	 
	/* Map the runlist to be able to update mapping pairs later. */
#if PARTIAL_RUNLIST_UPDATING
	if (!na->rl) {
		if (ntfs_attr_map_whole_runlist(na))
			goto err_out;
	} else {
		/* make sure the run ahead of hole is mapped */
		if ((*rl)->lcn == LCN_HOLE) {
			if (ntfs_attr_map_partial_runlist(na,
				(cur_vcn ? cur_vcn - 1 : cur_vcn)))
					goto err_out;
		}
	}
#else
	if (ntfs_attr_map_whole_runlist(na))
		goto err_out;
#endif
	 
	/* Restore @*rl, it probably get lost during runlist mapping. */
	*rl = ntfs_attr_find_vcn(na, cur_vcn);
	if (!*rl) {
		ntfs_log_error("Failed to find run after mapping runlist. "
			       "Please report to %s.\n", NTFS_DEV_LIST);
		errno = EIO;
		goto err_out;
	}
	
	/* Search backwards to find the best lcn to start seek from. */
	rlc = *rl;
	while (rlc->vcn) {
		rlc--;
		if (rlc->lcn >= 0) {
				/*
				 * avoid fragmenting a compressed file
				 * Windows does not do that, and that may
				 * not be desirable for files which can
				 * be updated
				 */
			if (na->data_flags & ATTR_COMPRESSION_MASK)
				lcn_seek_from = rlc->lcn + rlc->length;
			else
				lcn_seek_from = rlc->lcn + (from_vcn - rlc->vcn);
			break;
		}
	}
	if (lcn_seek_from == -1) {
		/* Backwards search failed, search forwards. */
		rlc = *rl;
		while (rlc->length) {
			rlc++;
			if (rlc->lcn >= 0) {
				lcn_seek_from = rlc->lcn - (rlc->vcn - from_vcn);
				if (lcn_seek_from < -1)
					lcn_seek_from = -1;
				break;
			}
		}
	}
	
	need = ((*ofs + to_write - 1) >> vol->cluster_size_bits)
			 + 1 + (*rl)->vcn - from_vcn;
	if ((na->data_flags & ATTR_COMPRESSION_MASK)
	    && (need < na->compression_block_clusters)) {
		/*
		 * for a compressed file, be sure to allocate the full
		 * compression block, as we may need space to decompress
		 * existing compressed data.
		 * So allocate the space common to compression block
		 * and existing hole.
		 */
		VCN alloc_vcn;

		if ((from_vcn & -na->compression_block_clusters) <= (*rl)->vcn)
			alloc_vcn = (*rl)->vcn;
		else
			alloc_vcn = from_vcn & -na->compression_block_clusters;
		need = (alloc_vcn | (na->compression_block_clusters - 1))
			+ 1 - alloc_vcn;
		if (need > (*rl)->length) {
			ntfs_log_error("Cannot allocate %lld clusters"
					" within a hole of %lld\n",
					(long long)need,
					(long long)(*rl)->length);
			errno = EIO;
			goto err_out;
		}
		rlc = ntfs_cluster_alloc(vol, alloc_vcn, need,
				 lcn_seek_from, DATA_ZONE);
	} else
		rlc = ntfs_cluster_alloc(vol, from_vcn, need,
				 lcn_seek_from, DATA_ZONE);
	if (!rlc)
		goto err_out;
	if (na->data_flags & (ATTR_COMPRESSION_MASK | ATTR_IS_SPARSE))
		na->compressed_size += need << vol->cluster_size_bits;
	
	*rl = ntfs_runlists_merge(na->rl, rlc);
	NAttrSetRunlistDirty(na);
		/*
		 * For a compressed attribute, we must be sure there are two
		 * available entries, so reserve them before it gets too late.
		 */
	if (*rl && (na->data_flags & ATTR_COMPRESSION_MASK)) {
		runlist_element *oldrl = na->rl;
		na->rl = *rl;
		*rl = ntfs_rl_extend(na,*rl,2);
		if (!*rl) na->rl = oldrl; /* restore to original if failed */
	}
	if (!*rl) {
		eo = errno;
		ntfs_log_perror("Failed to merge runlists");
		if (ntfs_cluster_free_from_rl(vol, rlc)) {
			ntfs_log_perror("Failed to free hot clusters. "
					"Please run chkdsk /f");
		}
		errno = eo;
		goto err_out;
	}
	na->unused_runs = 2;
	na->rl = *rl;
	if ((*update_from == -1) || (from_vcn < *update_from))
		*update_from = from_vcn;
	*rl = ntfs_attr_find_vcn(na, cur_vcn);
	if (!*rl) {
		/*
		 * It's definitely a BUG, if we failed to find @cur_vcn, because
		 * we missed it during instantiating of the hole.
		 */
		ntfs_log_error("Failed to find run after hole instantiation. "
			       "Please report to %s.\n", NTFS_DEV_LIST);
		errno = EIO;
		goto err_out;
	}
	/* If leaved part of the hole go to the next run. */
	if ((*rl)->lcn < 0)
		(*rl)++;
	/* Now LCN shoudn't be less than 0. */
	if ((*rl)->lcn < 0) {
		ntfs_log_error("BUG! LCN is lesser than 0. "
			       "Please report to the %s.\n", NTFS_DEV_LIST);
		errno = EIO;
		goto err_out;
	}
	if (*ofs) {
		/* Clear non-sparse region from @cur_vcn to @*ofs. */
		if (ntfs_attr_fill_zero(na, cur_vcn << vol->cluster_size_bits,
					*ofs))
			goto err_out;
	}
	if ((*rl)->vcn < cur_vcn) {
		/*
		 * Clusters that replaced hole are merged with
		 * previous run, so we need to update offset.
		 */
		*ofs += (cur_vcn - (*rl)->vcn) << vol->cluster_size_bits;
	}
	if ((*rl)->vcn > cur_vcn) {
		/*
		 * We left part of the hole, so we need to update offset
		 */
		*ofs -= ((*rl)->vcn - cur_vcn) << vol->cluster_size_bits;
	}
	
	ret = 0;
err_out:
	return ret;
}