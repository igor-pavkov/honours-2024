static int parse_options(char *options, struct super_block *sb,
			 unsigned long *journal_devnum,
			 unsigned int *journal_ioprio,
			 int is_remount)
{
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	char *p;
	substring_t args[MAX_OPT_ARGS];
	int token;

	if (!options)
		return 1;

	while ((p = strsep(&options, ",")) != NULL) {
		if (!*p)
			continue;
		/*
		 * Initialize args struct so we know whether arg was
		 * found; some options take optional arguments.
		 */
		args[0].to = args[0].from = NULL;
		token = match_token(p, tokens, args);
		if (handle_mount_opt(sb, p, token, args, journal_devnum,
				     journal_ioprio, is_remount) < 0)
			return 0;
	}
#ifdef CONFIG_QUOTA
	/*
	 * We do the test below only for project quotas. 'usrquota' and
	 * 'grpquota' mount options are allowed even without quota feature
	 * to support legacy quotas in quota files.
	 */
	if (test_opt(sb, PRJQUOTA) && !ext4_has_feature_project(sb)) {
		ext4_msg(sb, KERN_ERR, "Project quota feature not enabled. "
			 "Cannot enable project quota enforcement.");
		return 0;
	}
	if (sbi->s_qf_names[USRQUOTA] || sbi->s_qf_names[GRPQUOTA]) {
		if (test_opt(sb, USRQUOTA) && sbi->s_qf_names[USRQUOTA])
			clear_opt(sb, USRQUOTA);

		if (test_opt(sb, GRPQUOTA) && sbi->s_qf_names[GRPQUOTA])
			clear_opt(sb, GRPQUOTA);

		if (test_opt(sb, GRPQUOTA) || test_opt(sb, USRQUOTA)) {
			ext4_msg(sb, KERN_ERR, "old and new quota "
					"format mixing");
			return 0;
		}

		if (!sbi->s_jquota_fmt) {
			ext4_msg(sb, KERN_ERR, "journaled quota format "
					"not specified");
			return 0;
		}
	}
#endif
	if (test_opt(sb, DIOREAD_NOLOCK)) {
		int blocksize =
			BLOCK_SIZE << le32_to_cpu(sbi->s_es->s_log_block_size);

		if (blocksize < PAGE_SIZE) {
			ext4_msg(sb, KERN_ERR, "can't mount with "
				 "dioread_nolock if block size != PAGE_SIZE");
			return 0;
		}
	}
	if (test_opt(sb, DATA_FLAGS) == EXT4_MOUNT_ORDERED_DATA &&
	    test_opt(sb, JOURNAL_ASYNC_COMMIT)) {
		ext4_msg(sb, KERN_ERR, "can't mount with journal_async_commit "
			 "in data=ordered mode");
		return 0;
	}
	return 1;
}