void udf_evict_inode(struct inode *inode)
{
	struct udf_inode_info *iinfo = UDF_I(inode);
	int want_delete = 0;

	if (!is_bad_inode(inode)) {
		if (!inode->i_nlink) {
			want_delete = 1;
			udf_setsize(inode, 0);
			udf_update_inode(inode, IS_SYNC(inode));
		}
		if (iinfo->i_alloc_type != ICBTAG_FLAG_AD_IN_ICB &&
		    inode->i_size != iinfo->i_lenExtents) {
			udf_warn(inode->i_sb,
				 "Inode %lu (mode %o) has inode size %llu different from extent length %llu. Filesystem need not be standards compliant.\n",
				 inode->i_ino, inode->i_mode,
				 (unsigned long long)inode->i_size,
				 (unsigned long long)iinfo->i_lenExtents);
		}
	}
	truncate_inode_pages_final(&inode->i_data);
	invalidate_inode_buffers(inode);
	clear_inode(inode);
	kfree(iinfo->i_data);
	iinfo->i_data = NULL;
	udf_clear_extent_cache(inode);
	if (want_delete) {
		udf_free_inode(inode);
	}
}