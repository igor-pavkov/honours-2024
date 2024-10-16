int ocfs2_add_inode_data(struct ocfs2_super *osb,
			 struct inode *inode,
			 u32 *logical_offset,
			 u32 clusters_to_add,
			 int mark_unwritten,
			 struct buffer_head *fe_bh,
			 handle_t *handle,
			 struct ocfs2_alloc_context *data_ac,
			 struct ocfs2_alloc_context *meta_ac,
			 enum ocfs2_alloc_restarted *reason_ret)
{
	int ret;
	struct ocfs2_extent_tree et;

	ocfs2_init_dinode_extent_tree(&et, INODE_CACHE(inode), fe_bh);
	ret = ocfs2_add_clusters_in_btree(handle, &et, logical_offset,
					  clusters_to_add, mark_unwritten,
					  data_ac, meta_ac, reason_ret);

	return ret;
}