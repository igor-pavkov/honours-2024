static int ext4_load_journal(struct super_block *sb,
			     struct ext4_super_block *es,
			     unsigned long journal_devnum)
{
	journal_t *journal;
	unsigned int journal_inum = le32_to_cpu(es->s_journal_inum);
	dev_t journal_dev;
	int err = 0;
	int really_read_only;

	BUG_ON(!EXT4_HAS_COMPAT_FEATURE(sb, EXT4_FEATURE_COMPAT_HAS_JOURNAL));

	if (journal_devnum &&
	    journal_devnum != le32_to_cpu(es->s_journal_dev)) {
		printk(KERN_INFO "EXT4-fs: external journal device major/minor "
			"numbers have changed\n");
		journal_dev = new_decode_dev(journal_devnum);
	} else
		journal_dev = new_decode_dev(le32_to_cpu(es->s_journal_dev));

	really_read_only = bdev_read_only(sb->s_bdev);

	/*
	 * Are we loading a blank journal or performing recovery after a
	 * crash?  For recovery, we need to check in advance whether we
	 * can get read-write access to the device.
	 */

	if (EXT4_HAS_INCOMPAT_FEATURE(sb, EXT4_FEATURE_INCOMPAT_RECOVER)) {
		if (sb->s_flags & MS_RDONLY) {
			printk(KERN_INFO "EXT4-fs: INFO: recovery "
					"required on readonly filesystem.\n");
			if (really_read_only) {
				printk(KERN_ERR "EXT4-fs: write access "
					"unavailable, cannot proceed.\n");
				return -EROFS;
			}
			printk(KERN_INFO "EXT4-fs: write access will "
			       "be enabled during recovery.\n");
		}
	}

	if (journal_inum && journal_dev) {
		printk(KERN_ERR "EXT4-fs: filesystem has both journal "
		       "and inode journals!\n");
		return -EINVAL;
	}

	if (journal_inum) {
		if (!(journal = ext4_get_journal(sb, journal_inum)))
			return -EINVAL;
	} else {
		if (!(journal = ext4_get_dev_journal(sb, journal_dev)))
			return -EINVAL;
	}

	if (journal->j_flags & JBD2_BARRIER)
		printk(KERN_INFO "EXT4-fs: barriers enabled\n");
	else
		printk(KERN_INFO "EXT4-fs: barriers disabled\n");

	if (!really_read_only && test_opt(sb, UPDATE_JOURNAL)) {
		err = jbd2_journal_update_format(journal);
		if (err)  {
			printk(KERN_ERR "EXT4-fs: error updating journal.\n");
			jbd2_journal_destroy(journal);
			return err;
		}
	}

	if (!EXT4_HAS_INCOMPAT_FEATURE(sb, EXT4_FEATURE_INCOMPAT_RECOVER))
		err = jbd2_journal_wipe(journal, !really_read_only);
	if (!err)
		err = jbd2_journal_load(journal);

	if (err) {
		printk(KERN_ERR "EXT4-fs: error loading journal.\n");
		jbd2_journal_destroy(journal);
		return err;
	}

	EXT4_SB(sb)->s_journal = journal;
	ext4_clear_journal_err(sb, es);

	if (journal_devnum &&
	    journal_devnum != le32_to_cpu(es->s_journal_dev)) {
		es->s_journal_dev = cpu_to_le32(journal_devnum);
		sb->s_dirt = 1;

		/* Make sure we flush the recovery flag to disk. */
		ext4_commit_super(sb, es, 1);
	}

	return 0;
}