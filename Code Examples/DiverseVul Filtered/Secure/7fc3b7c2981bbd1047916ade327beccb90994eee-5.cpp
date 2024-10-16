int8_t udf_next_aext(struct inode *inode, struct extent_position *epos,
		     struct kernel_lb_addr *eloc, uint32_t *elen, int inc)
{
	int8_t etype;
	unsigned int indirections = 0;

	while ((etype = udf_current_aext(inode, epos, eloc, elen, inc)) ==
	       (EXT_NEXT_EXTENT_ALLOCDESCS >> 30)) {
		udf_pblk_t block;

		if (++indirections > UDF_MAX_INDIR_EXTS) {
			udf_err(inode->i_sb,
				"too many indirect extents in inode %lu\n",
				inode->i_ino);
			return -1;
		}

		epos->block = *eloc;
		epos->offset = sizeof(struct allocExtDesc);
		brelse(epos->bh);
		block = udf_get_lb_pblock(inode->i_sb, &epos->block, 0);
		epos->bh = udf_tread(inode->i_sb, block);
		if (!epos->bh) {
			udf_debug("reading block %u failed!\n", block);
			return -1;
		}
	}

	return etype;
}