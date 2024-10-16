static int ext4_sync_fs(struct super_block *sb, int wait)
{
	int ret = 0;

	trace_mark(ext4_sync_fs, "dev %s wait %d", sb->s_id, wait);
	sb->s_dirt = 0;
	if (EXT4_SB(sb)->s_journal) {
		if (wait)
			ret = ext4_force_commit(sb);
		else
 			jbd2_journal_start_commit(EXT4_SB(sb)->s_journal, NULL);
	} else {
		ext4_commit_super(sb, EXT4_SB(sb)->s_es, wait);
	}
	return ret;
}