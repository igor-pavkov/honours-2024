int udf_setup_indirect_aext(struct inode *inode, udf_pblk_t block,
			    struct extent_position *epos)
{
	struct super_block *sb = inode->i_sb;
	struct buffer_head *bh;
	struct allocExtDesc *aed;
	struct extent_position nepos;
	struct kernel_lb_addr neloc;
	int ver, adsize;

	if (UDF_I(inode)->i_alloc_type == ICBTAG_FLAG_AD_SHORT)
		adsize = sizeof(struct short_ad);
	else if (UDF_I(inode)->i_alloc_type == ICBTAG_FLAG_AD_LONG)
		adsize = sizeof(struct long_ad);
	else
		return -EIO;

	neloc.logicalBlockNum = block;
	neloc.partitionReferenceNum = epos->block.partitionReferenceNum;

	bh = udf_tgetblk(sb, udf_get_lb_pblock(sb, &neloc, 0));
	if (!bh)
		return -EIO;
	lock_buffer(bh);
	memset(bh->b_data, 0x00, sb->s_blocksize);
	set_buffer_uptodate(bh);
	unlock_buffer(bh);
	mark_buffer_dirty_inode(bh, inode);

	aed = (struct allocExtDesc *)(bh->b_data);
	if (!UDF_QUERY_FLAG(sb, UDF_FLAG_STRICT)) {
		aed->previousAllocExtLocation =
				cpu_to_le32(epos->block.logicalBlockNum);
	}
	aed->lengthAllocDescs = cpu_to_le32(0);
	if (UDF_SB(sb)->s_udfrev >= 0x0200)
		ver = 3;
	else
		ver = 2;
	udf_new_tag(bh->b_data, TAG_IDENT_AED, ver, 1, block,
		    sizeof(struct tag));

	nepos.block = neloc;
	nepos.offset = sizeof(struct allocExtDesc);
	nepos.bh = bh;

	/*
	 * Do we have to copy current last extent to make space for indirect
	 * one?
	 */
	if (epos->offset + adsize > sb->s_blocksize) {
		struct kernel_lb_addr cp_loc;
		uint32_t cp_len;
		int cp_type;

		epos->offset -= adsize;
		cp_type = udf_current_aext(inode, epos, &cp_loc, &cp_len, 0);
		cp_len |= ((uint32_t)cp_type) << 30;

		__udf_add_aext(inode, &nepos, &cp_loc, cp_len, 1);
		udf_write_aext(inode, epos, &nepos.block,
			       sb->s_blocksize | EXT_NEXT_EXTENT_ALLOCDESCS, 0);
	} else {
		__udf_add_aext(inode, epos, &nepos.block,
			       sb->s_blocksize | EXT_NEXT_EXTENT_ALLOCDESCS, 0);
	}

	brelse(epos->bh);
	*epos = nepos;

	return 0;
}