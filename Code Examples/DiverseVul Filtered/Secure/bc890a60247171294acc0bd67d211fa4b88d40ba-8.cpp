static ext4_lblk_t ext4_ext_determine_hole(struct inode *inode,
					   struct ext4_ext_path *path,
					   ext4_lblk_t *lblk)
{
	int depth = ext_depth(inode);
	struct ext4_extent *ex;
	ext4_lblk_t len;

	ex = path[depth].p_ext;
	if (ex == NULL) {
		/* there is no extent yet, so gap is [0;-] */
		*lblk = 0;
		len = EXT_MAX_BLOCKS;
	} else if (*lblk < le32_to_cpu(ex->ee_block)) {
		len = le32_to_cpu(ex->ee_block) - *lblk;
	} else if (*lblk >= le32_to_cpu(ex->ee_block)
			+ ext4_ext_get_actual_len(ex)) {
		ext4_lblk_t next;

		*lblk = le32_to_cpu(ex->ee_block) + ext4_ext_get_actual_len(ex);
		next = ext4_ext_next_allocated_block(path);
		BUG_ON(next == *lblk);
		len = next - *lblk;
	} else {
		BUG();
	}
	return len;
}