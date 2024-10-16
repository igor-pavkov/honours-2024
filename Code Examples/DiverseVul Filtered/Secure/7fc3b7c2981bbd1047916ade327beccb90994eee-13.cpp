int __udf_add_aext(struct inode *inode, struct extent_position *epos,
		   struct kernel_lb_addr *eloc, uint32_t elen, int inc)
{
	struct udf_inode_info *iinfo = UDF_I(inode);
	struct allocExtDesc *aed;
	int adsize;

	if (iinfo->i_alloc_type == ICBTAG_FLAG_AD_SHORT)
		adsize = sizeof(struct short_ad);
	else if (iinfo->i_alloc_type == ICBTAG_FLAG_AD_LONG)
		adsize = sizeof(struct long_ad);
	else
		return -EIO;

	if (!epos->bh) {
		WARN_ON(iinfo->i_lenAlloc !=
			epos->offset - udf_file_entry_alloc_offset(inode));
	} else {
		aed = (struct allocExtDesc *)epos->bh->b_data;
		WARN_ON(le32_to_cpu(aed->lengthAllocDescs) !=
			epos->offset - sizeof(struct allocExtDesc));
		WARN_ON(epos->offset + adsize > inode->i_sb->s_blocksize);
	}

	udf_write_aext(inode, epos, eloc, elen, inc);

	if (!epos->bh) {
		iinfo->i_lenAlloc += adsize;
		mark_inode_dirty(inode);
	} else {
		aed = (struct allocExtDesc *)epos->bh->b_data;
		le32_add_cpu(&aed->lengthAllocDescs, adsize);
		if (!UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_STRICT) ||
				UDF_SB(inode->i_sb)->s_udfrev >= 0x0201)
			udf_update_tag(epos->bh->b_data,
					epos->offset + (inc ? 0 : adsize));
		else
			udf_update_tag(epos->bh->b_data,
					sizeof(struct allocExtDesc));
		mark_buffer_dirty_inode(epos->bh, inode);
	}

	return 0;
}