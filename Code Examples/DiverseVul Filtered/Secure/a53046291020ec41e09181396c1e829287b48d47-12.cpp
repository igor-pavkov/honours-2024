static void jfs_readahead(struct readahead_control *rac)
{
	mpage_readahead(rac, jfs_get_block);
}