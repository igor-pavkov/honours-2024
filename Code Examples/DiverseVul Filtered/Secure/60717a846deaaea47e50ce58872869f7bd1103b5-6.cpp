int ntfs_attr_map_whole_runlist(ntfs_attr *na)
{
	VCN next_vcn, last_vcn, highest_vcn;
	ntfs_attr_search_ctx *ctx;
	ntfs_volume *vol = na->ni->vol;
	ATTR_RECORD *a;
	int ret = -1;
	int not_mapped;

	ntfs_log_enter("Entering for inode %llu, attr 0x%x.\n",
		       (unsigned long long)na->ni->mft_no, le32_to_cpu(na->type));

		/* avoid multiple full runlist mappings */
	if (NAttrFullyMapped(na)) {
		ret = 0;
		goto out;
	}
	ctx = ntfs_attr_get_search_ctx(na->ni, NULL);
	if (!ctx)
		goto out;

	/* Map all attribute extents one by one. */
	next_vcn = last_vcn = highest_vcn = 0;
	a = NULL;
	while (1) {
		runlist_element *rl;

		not_mapped = 0;
		if (ntfs_rl_vcn_to_lcn(na->rl, next_vcn) == LCN_RL_NOT_MAPPED)
			not_mapped = 1;

		if (ntfs_attr_lookup(na->type, na->name, na->name_len,
				CASE_SENSITIVE, next_vcn, NULL, 0, ctx))
			break;

		a = ctx->attr;

		if (not_mapped) {
			/* Decode the runlist. */
			rl = ntfs_mapping_pairs_decompress(na->ni->vol,
								a, na->rl);
			if (!rl)
				goto err_out;
			na->rl = rl;
		}

		/* Are we in the first extent? */
		if (!next_vcn) {
			 if (a->lowest_vcn) {
				 errno = EIO;
				 ntfs_log_perror("First extent of inode %llu "
					"attribute has non-zero lowest_vcn",
					(unsigned long long)na->ni->mft_no);
				 goto err_out;
			}
			/* Get the last vcn in the attribute. */
			last_vcn = sle64_to_cpu(a->allocated_size) >>
					vol->cluster_size_bits;
		}

		/* Get the lowest vcn for the next extent. */
		highest_vcn = sle64_to_cpu(a->highest_vcn);
		next_vcn = highest_vcn + 1;

		/* Only one extent or error, which we catch below. */
		if (next_vcn <= 0) {
			errno = ENOENT;
			break;
		}

		/* Avoid endless loops due to corruption. */
		if (next_vcn < sle64_to_cpu(a->lowest_vcn)) {
			errno = EIO;
			ntfs_log_perror("Inode %llu has corrupt attribute list",
					(unsigned long long)na->ni->mft_no);
			goto err_out;
		}
	}
	if (!a) {
		ntfs_log_perror("Couldn't find attribute for runlist mapping");
		goto err_out;
	}
		/*
		 * Cannot check highest_vcn when the last runlist has
		 * been modified earlier, as runlists and sizes may be
		 * updated without highest_vcn being in sync, when
		 * HOLES_DELAY is used
		 */
	if (not_mapped && highest_vcn && highest_vcn != last_vcn - 1) {
		errno = EIO;
		ntfs_log_perror("Failed to load full runlist: inode: %llu "
				"highest_vcn: 0x%llx last_vcn: 0x%llx",
				(unsigned long long)na->ni->mft_no, 
				(long long)highest_vcn, (long long)last_vcn);
		goto err_out;
	}
	if (errno == ENOENT) {
		NAttrSetFullyMapped(na);
		ret = 0;
	}
err_out:	
	ntfs_attr_put_search_ctx(ctx);
out:
	ntfs_log_leave("\n");
	return ret;
}