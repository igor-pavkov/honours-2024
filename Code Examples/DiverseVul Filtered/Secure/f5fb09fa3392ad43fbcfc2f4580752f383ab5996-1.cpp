static void minix_put_super(struct super_block *sb)
{
	int i;
	struct minix_sb_info *sbi = minix_sb(sb);

	if (!(sb->s_flags & MS_RDONLY)) {
		sbi->s_ms->s_state = sbi->s_mount_state;
		mark_buffer_dirty(sbi->s_sbh);
	}
	for (i = 0; i < sbi->s_imap_blocks; i++)
		brelse(sbi->s_imap[i]);
	for (i = 0; i < sbi->s_zmap_blocks; i++)
		brelse(sbi->s_zmap[i]);
	brelse (sbi->s_sbh);
	kfree(sbi->s_imap);
	sb->s_fs_info = NULL;
	kfree(sbi);

	return;
}