static int ext4_quota_on(struct super_block *sb, int type, int format_id,
			 char *name, int remount)
{
	int err;
	struct path path;

	if (!test_opt(sb, QUOTA))
		return -EINVAL;
	/* When remounting, no checks are needed and in fact, name is NULL */
	if (remount)
		return vfs_quota_on(sb, type, format_id, name, remount);

	err = kern_path(name, LOOKUP_FOLLOW, &path);
	if (err)
		return err;

	/* Quotafile not on the same filesystem? */
	if (path.mnt->mnt_sb != sb) {
		path_put(&path);
		return -EXDEV;
	}
	/* Journaling quota? */
	if (EXT4_SB(sb)->s_qf_names[type]) {
		/* Quotafile not in fs root? */
		if (path.dentry->d_parent != sb->s_root)
			printk(KERN_WARNING
				"EXT4-fs: Quota file not on filesystem root. "
				"Journaled quota will not work.\n");
	}

	/*
	 * When we journal data on quota file, we have to flush journal to see
	 * all updates to the file when we bypass pagecache...
	 */
	if (EXT4_SB(sb)->s_journal &&
	    ext4_should_journal_data(path.dentry->d_inode)) {
		/*
		 * We don't need to lock updates but journal_flush() could
		 * otherwise be livelocked...
		 */
		jbd2_journal_lock_updates(EXT4_SB(sb)->s_journal);
		err = jbd2_journal_flush(EXT4_SB(sb)->s_journal);
		jbd2_journal_unlock_updates(EXT4_SB(sb)->s_journal);
		if (err) {
			path_put(&path);
			return err;
		}
	}

	err = vfs_quota_on_path(sb, type, format_id, &path);
	path_put(&path);
	return err;
}