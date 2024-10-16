int ntfs_attr_map_runlist(ntfs_attr *na, VCN vcn)
{
	LCN lcn;
	ntfs_attr_search_ctx *ctx;

	ntfs_log_trace("Entering for inode 0x%llx, attr 0x%x, vcn 0x%llx.\n",
		(unsigned long long)na->ni->mft_no, le32_to_cpu(na->type), (long long)vcn);

	lcn = ntfs_rl_vcn_to_lcn(na->rl, vcn);
	if (lcn >= 0 || lcn == LCN_HOLE || lcn == LCN_ENOENT)
		return 0;

	ctx = ntfs_attr_get_search_ctx(na->ni, NULL);
	if (!ctx)
		return -1;

	/* Find the attribute in the mft record. */
	if (!ntfs_attr_lookup(na->type, na->name, na->name_len, CASE_SENSITIVE,
			vcn, NULL, 0, ctx)) {
		runlist_element *rl;

		/* Decode the runlist. */
		rl = ntfs_mapping_pairs_decompress(na->ni->vol, ctx->attr,
				na->rl);
		if (rl) {
			na->rl = rl;
			ntfs_attr_put_search_ctx(ctx);
			return 0;
		}
	}
	
	ntfs_attr_put_search_ctx(ctx);
	return -1;
}