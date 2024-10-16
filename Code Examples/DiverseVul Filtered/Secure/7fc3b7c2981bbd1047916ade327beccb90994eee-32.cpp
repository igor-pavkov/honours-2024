static int udf_extend_file(struct inode *inode, loff_t newsize)
{

	struct extent_position epos;
	struct kernel_lb_addr eloc;
	uint32_t elen;
	int8_t etype;
	struct super_block *sb = inode->i_sb;
	sector_t first_block = newsize >> sb->s_blocksize_bits, offset;
	unsigned long partial_final_block;
	int adsize;
	struct udf_inode_info *iinfo = UDF_I(inode);
	struct kernel_long_ad extent;
	int err = 0;
	int within_final_block;

	if (iinfo->i_alloc_type == ICBTAG_FLAG_AD_SHORT)
		adsize = sizeof(struct short_ad);
	else if (iinfo->i_alloc_type == ICBTAG_FLAG_AD_LONG)
		adsize = sizeof(struct long_ad);
	else
		BUG();

	etype = inode_bmap(inode, first_block, &epos, &eloc, &elen, &offset);
	within_final_block = (etype != -1);

	if ((!epos.bh && epos.offset == udf_file_entry_alloc_offset(inode)) ||
	    (epos.bh && epos.offset == sizeof(struct allocExtDesc))) {
		/* File has no extents at all or has empty last
		 * indirect extent! Create a fake extent... */
		extent.extLocation.logicalBlockNum = 0;
		extent.extLocation.partitionReferenceNum = 0;
		extent.extLength = EXT_NOT_RECORDED_NOT_ALLOCATED;
	} else {
		epos.offset -= adsize;
		etype = udf_next_aext(inode, &epos, &extent.extLocation,
				      &extent.extLength, 0);
		extent.extLength |= etype << 30;
	}

	partial_final_block = newsize & (sb->s_blocksize - 1);

	/* File has extent covering the new size (could happen when extending
	 * inside a block)?
	 */
	if (within_final_block) {
		/* Extending file within the last file block */
		udf_do_extend_final_block(inode, &epos, &extent,
					  partial_final_block);
	} else {
		loff_t add = ((loff_t)offset << sb->s_blocksize_bits) |
			     partial_final_block;
		err = udf_do_extend_file(inode, &epos, &extent, add);
	}

	if (err < 0)
		goto out;
	err = 0;
	iinfo->i_lenExtents = newsize;
out:
	brelse(epos.bh);
	return err;
}