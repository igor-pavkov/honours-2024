static int ntfs_attr_map_partial_runlist(ntfs_attr *na, VCN vcn)
{
	VCN last_vcn;
	VCN highest_vcn;
	VCN needed;
	runlist_element *rl;
	ATTR_RECORD *a;
	BOOL startseen;
	ntfs_attr_search_ctx *ctx;
	BOOL done;
	BOOL newrunlist;

	if (NAttrFullyMapped(na))
		return 0;

	ctx = ntfs_attr_get_search_ctx(na->ni, NULL);
	if (!ctx)
		return -1;

	/* Get the last vcn in the attribute. */
	last_vcn = na->allocated_size >> na->ni->vol->cluster_size_bits;

	needed = vcn;
	highest_vcn = 0;
	startseen = FALSE;
	done = FALSE;
	rl = (runlist_element*)NULL;
	do {
		newrunlist = FALSE;
		/* Find the attribute in the mft record. */
		if (!ntfs_attr_lookup(na->type, na->name, na->name_len, CASE_SENSITIVE,
				needed, NULL, 0, ctx)) {

			a = ctx->attr;
				/* Decode and merge the runlist. */
			if (ntfs_rl_vcn_to_lcn(na->rl, needed)
						== LCN_RL_NOT_MAPPED) {
				rl = ntfs_mapping_pairs_decompress(na->ni->vol,
					a, na->rl);
				newrunlist = TRUE;
			} else
				rl = na->rl;
			if (rl) {
				na->rl = rl;
				highest_vcn = sle64_to_cpu(a->highest_vcn);
				if (highest_vcn < needed) {
				/* corruption detection on unchanged runlists */
					if (newrunlist
					    && ((highest_vcn + 1) < last_vcn)) {
						ntfs_log_error("Corrupt attribute list\n");
						rl = (runlist_element*)NULL;
						errno = EIO;
					}
					done = TRUE;
				}
				needed = highest_vcn + 1;
				if (!a->lowest_vcn)
					startseen = TRUE;
			}
		} else {
			done = TRUE;
		}
	} while (rl && !done && (needed < last_vcn));
	ntfs_attr_put_search_ctx(ctx);
		/*
		 * Make sure we reached the end, unless the last
		 * runlist was modified earlier (using HOLES_DELAY
		 * leads to have a visibility over attributes which
		 * have not yet been fully updated)
		 */
	if (done && newrunlist && (needed < last_vcn)) {
		ntfs_log_error("End of runlist not reached\n");
		rl = (runlist_element*)NULL;
		errno = EIO;
	}
		/* mark fully mapped if we did so */
	if (rl && startseen)
		NAttrSetFullyMapped(na);
	return (rl ? 0 : -1);
}