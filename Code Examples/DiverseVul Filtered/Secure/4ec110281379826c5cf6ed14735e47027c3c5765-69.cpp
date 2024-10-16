handle_t *ext4_journal_start_sb(struct super_block *sb, int nblocks)
{
	journal_t *journal;

	if (sb->s_flags & MS_RDONLY)
		return ERR_PTR(-EROFS);

	/* Special case here: if the journal has aborted behind our
	 * backs (eg. EIO in the commit thread), then we still need to
	 * take the FS itself readonly cleanly. */
	journal = EXT4_SB(sb)->s_journal;
	if (journal) {
		if (is_journal_aborted(journal)) {
			ext4_abort(sb, __func__,
				   "Detected aborted journal");
			return ERR_PTR(-EROFS);
		}
		return jbd2_journal_start(journal, nblocks);
	}
	/*
	 * We're not journaling, return the appropriate indication.
	 */
	current->journal_info = EXT4_NOJOURNAL_HANDLE;
	return current->journal_info;
}