static void ext4_write_super(struct super_block *sb)
{
	if (EXT4_SB(sb)->s_journal) {
		if (mutex_trylock(&sb->s_lock) != 0)
			BUG();
		sb->s_dirt = 0;
	} else {
		ext4_commit_super(sb, EXT4_SB(sb)->s_es, 1);
	}
}