static void ext4_commit_super(struct super_block *sb,
			      struct ext4_super_block *es, int sync)
{
	struct buffer_head *sbh = EXT4_SB(sb)->s_sbh;

	if (!sbh)
		return;
	if (buffer_write_io_error(sbh)) {
		/*
		 * Oh, dear.  A previous attempt to write the
		 * superblock failed.  This could happen because the
		 * USB device was yanked out.  Or it could happen to
		 * be a transient write error and maybe the block will
		 * be remapped.  Nothing we can do but to retry the
		 * write and hope for the best.
		 */
		printk(KERN_ERR "ext4: previous I/O error to "
		       "superblock detected for %s.\n", sb->s_id);
		clear_buffer_write_io_error(sbh);
		set_buffer_uptodate(sbh);
	}
	es->s_wtime = cpu_to_le32(get_seconds());
	ext4_free_blocks_count_set(es, percpu_counter_sum_positive(
					&EXT4_SB(sb)->s_freeblocks_counter));
	es->s_free_inodes_count = cpu_to_le32(percpu_counter_sum_positive(
					&EXT4_SB(sb)->s_freeinodes_counter));

	BUFFER_TRACE(sbh, "marking dirty");
	mark_buffer_dirty(sbh);
	if (sync) {
		sync_dirty_buffer(sbh);
		if (buffer_write_io_error(sbh)) {
			printk(KERN_ERR "ext4: I/O error while writing "
			       "superblock for %s.\n", sb->s_id);
			clear_buffer_write_io_error(sbh);
			set_buffer_uptodate(sbh);
		}
	}
}