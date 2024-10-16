runlist_element *ntfs_attr_find_vcn(ntfs_attr *na, const VCN vcn)
{
	runlist_element *rl;
	BOOL is_retry = FALSE;

	if (!na || !NAttrNonResident(na) || vcn < 0) {
		errno = EINVAL;
		return NULL;
	}

	ntfs_log_trace("Entering for inode 0x%llx, attr 0x%x, vcn %llx\n",
		       (unsigned long long)na->ni->mft_no, le32_to_cpu(na->type),
		       (long long)vcn);
retry:
	rl = na->rl;
	if (!rl)
		goto map_rl;
	if (vcn < rl[0].vcn)
		goto map_rl;
	while (rl->length) {
		if (vcn < rl[1].vcn) {
			if (rl->lcn >= (LCN)LCN_HOLE)
				return rl;
			break;
		}
		rl++;
	}
	switch (rl->lcn) {
	case (LCN)LCN_RL_NOT_MAPPED:
		goto map_rl;
	case (LCN)LCN_ENOENT:
		errno = ENOENT;
		break;
	case (LCN)LCN_EINVAL:
		errno = EINVAL;
		break;
	default:
		errno = EIO;
		break;
	}
	return NULL;
map_rl:
	/* The @vcn is in an unmapped region, map the runlist and retry. */
	if (!is_retry && !ntfs_attr_map_runlist(na, vcn)) {
		is_retry = TRUE;
		goto retry;
	}
	/*
	 * If we already retried or the mapping attempt failed something has
	 * gone badly wrong. EINVAL and ENOENT coming from a failed mapping
	 * attempt are equivalent to errors for us as they should not happen
	 * in our code paths.
	 */
	if (is_retry || errno == EINVAL || errno == ENOENT)
		errno = EIO;
	return NULL;
}