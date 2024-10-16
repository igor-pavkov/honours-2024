static void ext4_write_super_lockfs(struct super_block *sb)
{
	sb->s_dirt = 0;

	if (!(sb->s_flags & MS_RDONLY)) {
		journal_t *journal = EXT4_SB(sb)->s_journal;

		if (journal) {
			/* Now we set up the journal barrier. */
			jbd2_journal_lock_updates(journal);

			/*
			 * We don't want to clear needs_recovery flag when we
			 * failed to flush the journal.
			 */
			if (jbd2_journal_flush(journal) < 0)
				return;
		}

		/* Journal blocked and flushed, clear needs_recovery flag. */
		EXT4_CLEAR_INCOMPAT_FEATURE(sb, EXT4_FEATURE_INCOMPAT_RECOVER);
		ext4_commit_super(sb, EXT4_SB(sb)->s_es, 1);
	}
}