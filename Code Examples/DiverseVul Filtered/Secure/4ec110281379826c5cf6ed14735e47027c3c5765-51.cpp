static void ext4_unlockfs(struct super_block *sb)
{
	if (EXT4_SB(sb)->s_journal && !(sb->s_flags & MS_RDONLY)) {
		lock_super(sb);
		/* Reser the needs_recovery flag before the fs is unlocked. */
		EXT4_SET_INCOMPAT_FEATURE(sb, EXT4_FEATURE_INCOMPAT_RECOVER);
		ext4_commit_super(sb, EXT4_SB(sb)->s_es, 1);
		unlock_super(sb);
		jbd2_journal_unlock_updates(EXT4_SB(sb)->s_journal);
	}
}