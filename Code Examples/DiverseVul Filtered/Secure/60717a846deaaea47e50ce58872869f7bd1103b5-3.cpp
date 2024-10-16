void ntfs_attr_close(ntfs_attr *na)
{
	if (!na)
		return;
	if (NAttrNonResident(na) && na->rl)
		free(na->rl);
	/* Don't release if using an internal constant. */
	if (na->name != AT_UNNAMED && na->name != NTFS_INDEX_I30
				&& na->name != STREAM_SDS)
		free(na->name);
	free(na);
}