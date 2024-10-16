udf_pblk_t udf_block_map(struct inode *inode, sector_t block)
{
	struct kernel_lb_addr eloc;
	uint32_t elen;
	sector_t offset;
	struct extent_position epos = {};
	udf_pblk_t ret;

	down_read(&UDF_I(inode)->i_data_sem);

	if (inode_bmap(inode, block, &epos, &eloc, &elen, &offset) ==
						(EXT_RECORDED_ALLOCATED >> 30))
		ret = udf_get_lb_pblock(inode->i_sb, &eloc, offset);
	else
		ret = 0;

	up_read(&UDF_I(inode)->i_data_sem);
	brelse(epos.bh);

	if (UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_VARCONV))
		return udf_fixed_to_variable(ret);
	else
		return ret;
}