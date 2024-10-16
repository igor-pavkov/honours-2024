static void udf_readahead(struct readahead_control *rac)
{
	mpage_readahead(rac, udf_get_block);
}