LCN ntfs_attr_vcn_to_lcn(ntfs_attr *na, const VCN vcn)
{
	LCN lcn;
	BOOL is_retry = FALSE;

	if (!na || !NAttrNonResident(na) || vcn < 0)
		return (LCN)LCN_EINVAL;

	ntfs_log_trace("Entering for inode 0x%llx, attr 0x%x.\n", (unsigned long
			long)na->ni->mft_no, le32_to_cpu(na->type));
retry:
	/* Convert vcn to lcn. If that fails map the runlist and retry once. */
	lcn = ntfs_rl_vcn_to_lcn(na->rl, vcn);
	if (lcn >= 0)
		return lcn;
	if (!is_retry && !ntfs_attr_map_runlist(na, vcn)) {
		is_retry = TRUE;
		goto retry;
	}
	/*
	 * If the attempt to map the runlist failed, or we are getting
	 * LCN_RL_NOT_MAPPED despite having mapped the attribute extent
	 * successfully, something is really badly wrong...
	 */
	if (!is_retry || lcn == (LCN)LCN_RL_NOT_MAPPED)
		return (LCN)LCN_EIO;
	/* lcn contains the appropriate error code. */
	return lcn;
}