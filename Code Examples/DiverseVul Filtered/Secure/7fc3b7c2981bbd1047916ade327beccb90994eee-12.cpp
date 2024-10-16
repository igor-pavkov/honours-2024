int udf_add_aext(struct inode *inode, struct extent_position *epos,
		 struct kernel_lb_addr *eloc, uint32_t elen, int inc)
{
	int adsize;
	struct super_block *sb = inode->i_sb;

	if (UDF_I(inode)->i_alloc_type == ICBTAG_FLAG_AD_SHORT)
		adsize = sizeof(struct short_ad);
	else if (UDF_I(inode)->i_alloc_type == ICBTAG_FLAG_AD_LONG)
		adsize = sizeof(struct long_ad);
	else
		return -EIO;

	if (epos->offset + (2 * adsize) > sb->s_blocksize) {
		int err;
		udf_pblk_t new_block;

		new_block = udf_new_block(sb, NULL,
					  epos->block.partitionReferenceNum,
					  epos->block.logicalBlockNum, &err);
		if (!new_block)
			return -ENOSPC;

		err = udf_setup_indirect_aext(inode, new_block, epos);
		if (err)
			return err;
	}

	return __udf_add_aext(inode, epos, eloc, elen, inc);
}