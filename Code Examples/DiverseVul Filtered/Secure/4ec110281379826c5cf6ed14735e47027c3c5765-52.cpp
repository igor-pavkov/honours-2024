static int ext4_remount(struct super_block *sb, int *flags, char *data)
{
	struct ext4_super_block *es;
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	ext4_fsblk_t n_blocks_count = 0;
	unsigned long old_sb_flags;
	struct ext4_mount_options old_opts;
	ext4_group_t g;
	unsigned int journal_ioprio = DEFAULT_JOURNAL_IOPRIO;
	int err;
#ifdef CONFIG_QUOTA
	int i;
#endif

	/* Store the original options */
	old_sb_flags = sb->s_flags;
	old_opts.s_mount_opt = sbi->s_mount_opt;
	old_opts.s_resuid = sbi->s_resuid;
	old_opts.s_resgid = sbi->s_resgid;
	old_opts.s_commit_interval = sbi->s_commit_interval;
	old_opts.s_min_batch_time = sbi->s_min_batch_time;
	old_opts.s_max_batch_time = sbi->s_max_batch_time;
#ifdef CONFIG_QUOTA
	old_opts.s_jquota_fmt = sbi->s_jquota_fmt;
	for (i = 0; i < MAXQUOTAS; i++)
		old_opts.s_qf_names[i] = sbi->s_qf_names[i];
#endif
	if (sbi->s_journal && sbi->s_journal->j_task->io_context)
		journal_ioprio = sbi->s_journal->j_task->io_context->ioprio;

	/*
	 * Allow the "check" option to be passed as a remount option.
	 */
	if (!parse_options(data, sb, NULL, &journal_ioprio,
			   &n_blocks_count, 1)) {
		err = -EINVAL;
		goto restore_opts;
	}

	if (sbi->s_mount_opt & EXT4_MOUNT_ABORT)
		ext4_abort(sb, __func__, "Abort forced by user");

	sb->s_flags = (sb->s_flags & ~MS_POSIXACL) |
		((sbi->s_mount_opt & EXT4_MOUNT_POSIX_ACL) ? MS_POSIXACL : 0);

	es = sbi->s_es;

	if (sbi->s_journal) {
		ext4_init_journal_params(sb, sbi->s_journal);
		set_task_ioprio(sbi->s_journal->j_task, journal_ioprio);
	}

	if ((*flags & MS_RDONLY) != (sb->s_flags & MS_RDONLY) ||
		n_blocks_count > ext4_blocks_count(es)) {
		if (sbi->s_mount_opt & EXT4_MOUNT_ABORT) {
			err = -EROFS;
			goto restore_opts;
		}

		if (*flags & MS_RDONLY) {
			/*
			 * First of all, the unconditional stuff we have to do
			 * to disable replay of the journal when we next remount
			 */
			sb->s_flags |= MS_RDONLY;

			/*
			 * OK, test if we are remounting a valid rw partition
			 * readonly, and if so set the rdonly flag and then
			 * mark the partition as valid again.
			 */
			if (!(es->s_state & cpu_to_le16(EXT4_VALID_FS)) &&
			    (sbi->s_mount_state & EXT4_VALID_FS))
				es->s_state = cpu_to_le16(sbi->s_mount_state);

			/*
			 * We have to unlock super so that we can wait for
			 * transactions.
			 */
			if (sbi->s_journal) {
				unlock_super(sb);
				ext4_mark_recovery_complete(sb, es);
				lock_super(sb);
			}
		} else {
			int ret;
			if ((ret = EXT4_HAS_RO_COMPAT_FEATURE(sb,
					~EXT4_FEATURE_RO_COMPAT_SUPP))) {
				printk(KERN_WARNING "EXT4-fs: %s: couldn't "
				       "remount RDWR because of unsupported "
				       "optional features (%x).\n", sb->s_id,
				(le32_to_cpu(sbi->s_es->s_feature_ro_compat) &
					~EXT4_FEATURE_RO_COMPAT_SUPP));
				err = -EROFS;
				goto restore_opts;
			}

			/*
			 * Make sure the group descriptor checksums
			 * are sane.  If they aren't, refuse to
			 * remount r/w.
			 */
			for (g = 0; g < sbi->s_groups_count; g++) {
				struct ext4_group_desc *gdp =
					ext4_get_group_desc(sb, g, NULL);

				if (!ext4_group_desc_csum_verify(sbi, g, gdp)) {
					printk(KERN_ERR
	       "EXT4-fs: ext4_remount: "
		"Checksum for group %u failed (%u!=%u)\n",
		g, le16_to_cpu(ext4_group_desc_csum(sbi, g, gdp)),
					       le16_to_cpu(gdp->bg_checksum));
					err = -EINVAL;
					goto restore_opts;
				}
			}

			/*
			 * If we have an unprocessed orphan list hanging
			 * around from a previously readonly bdev mount,
			 * require a full umount/remount for now.
			 */
			if (es->s_last_orphan) {
				printk(KERN_WARNING "EXT4-fs: %s: couldn't "
				       "remount RDWR because of unprocessed "
				       "orphan inode list.  Please "
				       "umount/remount instead.\n",
				       sb->s_id);
				err = -EINVAL;
				goto restore_opts;
			}

			/*
			 * Mounting a RDONLY partition read-write, so reread
			 * and store the current valid flag.  (It may have
			 * been changed by e2fsck since we originally mounted
			 * the partition.)
			 */
			if (sbi->s_journal)
				ext4_clear_journal_err(sb, es);
			sbi->s_mount_state = le16_to_cpu(es->s_state);
			if ((err = ext4_group_extend(sb, es, n_blocks_count)))
				goto restore_opts;
			if (!ext4_setup_super(sb, es, 0))
				sb->s_flags &= ~MS_RDONLY;
		}
	}
	if (sbi->s_journal == NULL)
		ext4_commit_super(sb, es, 1);

#ifdef CONFIG_QUOTA
	/* Release old quota file names */
	for (i = 0; i < MAXQUOTAS; i++)
		if (old_opts.s_qf_names[i] &&
		    old_opts.s_qf_names[i] != sbi->s_qf_names[i])
			kfree(old_opts.s_qf_names[i]);
#endif
	return 0;
restore_opts:
	sb->s_flags = old_sb_flags;
	sbi->s_mount_opt = old_opts.s_mount_opt;
	sbi->s_resuid = old_opts.s_resuid;
	sbi->s_resgid = old_opts.s_resgid;
	sbi->s_commit_interval = old_opts.s_commit_interval;
	sbi->s_min_batch_time = old_opts.s_min_batch_time;
	sbi->s_max_batch_time = old_opts.s_max_batch_time;
#ifdef CONFIG_QUOTA
	sbi->s_jquota_fmt = old_opts.s_jquota_fmt;
	for (i = 0; i < MAXQUOTAS; i++) {
		if (sbi->s_qf_names[i] &&
		    old_opts.s_qf_names[i] != sbi->s_qf_names[i])
			kfree(sbi->s_qf_names[i]);
		sbi->s_qf_names[i] = old_opts.s_qf_names[i];
	}
#endif
	return err;
}