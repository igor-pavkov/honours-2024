static int parse_options(char *options, struct super_block *sb,
			 unsigned long *journal_devnum,
			 unsigned int *journal_ioprio,
			 ext4_fsblk_t *n_blocks_count, int is_remount)
{
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	char *p;
	substring_t args[MAX_OPT_ARGS];
	int data_opt = 0;
	int option;
#ifdef CONFIG_QUOTA
	int qtype, qfmt;
	char *qname;
#endif
	ext4_fsblk_t last_block;

	if (!options)
		return 1;

	while ((p = strsep(&options, ",")) != NULL) {
		int token;
		if (!*p)
			continue;

		token = match_token(p, tokens, args);
		switch (token) {
		case Opt_bsd_df:
			clear_opt(sbi->s_mount_opt, MINIX_DF);
			break;
		case Opt_minix_df:
			set_opt(sbi->s_mount_opt, MINIX_DF);
			break;
		case Opt_grpid:
			set_opt(sbi->s_mount_opt, GRPID);
			break;
		case Opt_nogrpid:
			clear_opt(sbi->s_mount_opt, GRPID);
			break;
		case Opt_resuid:
			if (match_int(&args[0], &option))
				return 0;
			sbi->s_resuid = option;
			break;
		case Opt_resgid:
			if (match_int(&args[0], &option))
				return 0;
			sbi->s_resgid = option;
			break;
		case Opt_sb:
			/* handled by get_sb_block() instead of here */
			/* *sb_block = match_int(&args[0]); */
			break;
		case Opt_err_panic:
			clear_opt(sbi->s_mount_opt, ERRORS_CONT);
			clear_opt(sbi->s_mount_opt, ERRORS_RO);
			set_opt(sbi->s_mount_opt, ERRORS_PANIC);
			break;
		case Opt_err_ro:
			clear_opt(sbi->s_mount_opt, ERRORS_CONT);
			clear_opt(sbi->s_mount_opt, ERRORS_PANIC);
			set_opt(sbi->s_mount_opt, ERRORS_RO);
			break;
		case Opt_err_cont:
			clear_opt(sbi->s_mount_opt, ERRORS_RO);
			clear_opt(sbi->s_mount_opt, ERRORS_PANIC);
			set_opt(sbi->s_mount_opt, ERRORS_CONT);
			break;
		case Opt_nouid32:
			set_opt(sbi->s_mount_opt, NO_UID32);
			break;
		case Opt_debug:
			set_opt(sbi->s_mount_opt, DEBUG);
			break;
		case Opt_oldalloc:
			set_opt(sbi->s_mount_opt, OLDALLOC);
			break;
		case Opt_orlov:
			clear_opt(sbi->s_mount_opt, OLDALLOC);
			break;
#ifdef CONFIG_EXT4_FS_XATTR
		case Opt_user_xattr:
			set_opt(sbi->s_mount_opt, XATTR_USER);
			break;
		case Opt_nouser_xattr:
			clear_opt(sbi->s_mount_opt, XATTR_USER);
			break;
#else
		case Opt_user_xattr:
		case Opt_nouser_xattr:
			printk(KERN_ERR "EXT4 (no)user_xattr options "
			       "not supported\n");
			break;
#endif
#ifdef CONFIG_EXT4_FS_POSIX_ACL
		case Opt_acl:
			set_opt(sbi->s_mount_opt, POSIX_ACL);
			break;
		case Opt_noacl:
			clear_opt(sbi->s_mount_opt, POSIX_ACL);
			break;
#else
		case Opt_acl:
		case Opt_noacl:
			printk(KERN_ERR "EXT4 (no)acl options "
			       "not supported\n");
			break;
#endif
		case Opt_reservation:
			set_opt(sbi->s_mount_opt, RESERVATION);
			break;
		case Opt_noreservation:
			clear_opt(sbi->s_mount_opt, RESERVATION);
			break;
		case Opt_journal_update:
			/* @@@ FIXME */
			/* Eventually we will want to be able to create
			   a journal file here.  For now, only allow the
			   user to specify an existing inode to be the
			   journal file. */
			if (is_remount) {
				printk(KERN_ERR "EXT4-fs: cannot specify "
				       "journal on remount\n");
				return 0;
			}
			set_opt(sbi->s_mount_opt, UPDATE_JOURNAL);
			break;
		case Opt_journal_dev:
			if (is_remount) {
				printk(KERN_ERR "EXT4-fs: cannot specify "
				       "journal on remount\n");
				return 0;
			}
			if (match_int(&args[0], &option))
				return 0;
			*journal_devnum = option;
			break;
		case Opt_journal_checksum:
			set_opt(sbi->s_mount_opt, JOURNAL_CHECKSUM);
			break;
		case Opt_journal_async_commit:
			set_opt(sbi->s_mount_opt, JOURNAL_ASYNC_COMMIT);
			set_opt(sbi->s_mount_opt, JOURNAL_CHECKSUM);
			break;
		case Opt_noload:
			set_opt(sbi->s_mount_opt, NOLOAD);
			break;
		case Opt_commit:
			if (match_int(&args[0], &option))
				return 0;
			if (option < 0)
				return 0;
			if (option == 0)
				option = JBD2_DEFAULT_MAX_COMMIT_AGE;
			sbi->s_commit_interval = HZ * option;
			break;
		case Opt_max_batch_time:
			if (match_int(&args[0], &option))
				return 0;
			if (option < 0)
				return 0;
			if (option == 0)
				option = EXT4_DEF_MAX_BATCH_TIME;
			sbi->s_max_batch_time = option;
			break;
		case Opt_min_batch_time:
			if (match_int(&args[0], &option))
				return 0;
			if (option < 0)
				return 0;
			sbi->s_min_batch_time = option;
			break;
		case Opt_data_journal:
			data_opt = EXT4_MOUNT_JOURNAL_DATA;
			goto datacheck;
		case Opt_data_ordered:
			data_opt = EXT4_MOUNT_ORDERED_DATA;
			goto datacheck;
		case Opt_data_writeback:
			data_opt = EXT4_MOUNT_WRITEBACK_DATA;
		datacheck:
			if (is_remount) {
				if ((sbi->s_mount_opt & EXT4_MOUNT_DATA_FLAGS)
						!= data_opt) {
					printk(KERN_ERR
						"EXT4-fs: cannot change data "
						"mode on remount\n");
					return 0;
				}
			} else {
				sbi->s_mount_opt &= ~EXT4_MOUNT_DATA_FLAGS;
				sbi->s_mount_opt |= data_opt;
			}
			break;
		case Opt_data_err_abort:
			set_opt(sbi->s_mount_opt, DATA_ERR_ABORT);
			break;
		case Opt_data_err_ignore:
			clear_opt(sbi->s_mount_opt, DATA_ERR_ABORT);
			break;
#ifdef CONFIG_QUOTA
		case Opt_usrjquota:
			qtype = USRQUOTA;
			goto set_qf_name;
		case Opt_grpjquota:
			qtype = GRPQUOTA;
set_qf_name:
			if ((sb_any_quota_enabled(sb) ||
			     sb_any_quota_suspended(sb)) &&
			    !sbi->s_qf_names[qtype]) {
				printk(KERN_ERR
				       "EXT4-fs: Cannot change journaled "
				       "quota options when quota turned on.\n");
				return 0;
			}
			qname = match_strdup(&args[0]);
			if (!qname) {
				printk(KERN_ERR
					"EXT4-fs: not enough memory for "
					"storing quotafile name.\n");
				return 0;
			}
			if (sbi->s_qf_names[qtype] &&
			    strcmp(sbi->s_qf_names[qtype], qname)) {
				printk(KERN_ERR
					"EXT4-fs: %s quota file already "
					"specified.\n", QTYPE2NAME(qtype));
				kfree(qname);
				return 0;
			}
			sbi->s_qf_names[qtype] = qname;
			if (strchr(sbi->s_qf_names[qtype], '/')) {
				printk(KERN_ERR
					"EXT4-fs: quotafile must be on "
					"filesystem root.\n");
				kfree(sbi->s_qf_names[qtype]);
				sbi->s_qf_names[qtype] = NULL;
				return 0;
			}
			set_opt(sbi->s_mount_opt, QUOTA);
			break;
		case Opt_offusrjquota:
			qtype = USRQUOTA;
			goto clear_qf_name;
		case Opt_offgrpjquota:
			qtype = GRPQUOTA;
clear_qf_name:
			if ((sb_any_quota_enabled(sb) ||
			     sb_any_quota_suspended(sb)) &&
			    sbi->s_qf_names[qtype]) {
				printk(KERN_ERR "EXT4-fs: Cannot change "
					"journaled quota options when "
					"quota turned on.\n");
				return 0;
			}
			/*
			 * The space will be released later when all options
			 * are confirmed to be correct
			 */
			sbi->s_qf_names[qtype] = NULL;
			break;
		case Opt_jqfmt_vfsold:
			qfmt = QFMT_VFS_OLD;
			goto set_qf_format;
		case Opt_jqfmt_vfsv0:
			qfmt = QFMT_VFS_V0;
set_qf_format:
			if ((sb_any_quota_enabled(sb) ||
			     sb_any_quota_suspended(sb)) &&
			    sbi->s_jquota_fmt != qfmt) {
				printk(KERN_ERR "EXT4-fs: Cannot change "
					"journaled quota options when "
					"quota turned on.\n");
				return 0;
			}
			sbi->s_jquota_fmt = qfmt;
			break;
		case Opt_quota:
		case Opt_usrquota:
			set_opt(sbi->s_mount_opt, QUOTA);
			set_opt(sbi->s_mount_opt, USRQUOTA);
			break;
		case Opt_grpquota:
			set_opt(sbi->s_mount_opt, QUOTA);
			set_opt(sbi->s_mount_opt, GRPQUOTA);
			break;
		case Opt_noquota:
			if (sb_any_quota_enabled(sb)) {
				printk(KERN_ERR "EXT4-fs: Cannot change quota "
					"options when quota turned on.\n");
				return 0;
			}
			clear_opt(sbi->s_mount_opt, QUOTA);
			clear_opt(sbi->s_mount_opt, USRQUOTA);
			clear_opt(sbi->s_mount_opt, GRPQUOTA);
			break;
#else
		case Opt_quota:
		case Opt_usrquota:
		case Opt_grpquota:
			printk(KERN_ERR
				"EXT4-fs: quota options not supported.\n");
			break;
		case Opt_usrjquota:
		case Opt_grpjquota:
		case Opt_offusrjquota:
		case Opt_offgrpjquota:
		case Opt_jqfmt_vfsold:
		case Opt_jqfmt_vfsv0:
			printk(KERN_ERR
				"EXT4-fs: journaled quota options not "
				"supported.\n");
			break;
		case Opt_noquota:
			break;
#endif
		case Opt_abort:
			set_opt(sbi->s_mount_opt, ABORT);
			break;
		case Opt_barrier:
			if (match_int(&args[0], &option))
				return 0;
			if (option)
				set_opt(sbi->s_mount_opt, BARRIER);
			else
				clear_opt(sbi->s_mount_opt, BARRIER);
			break;
		case Opt_ignore:
			break;
		case Opt_resize:
			if (!is_remount) {
				printk("EXT4-fs: resize option only available "
					"for remount\n");
				return 0;
			}
			if (match_int(&args[0], &option) != 0)
				return 0;
			*n_blocks_count = option;
			break;
		case Opt_nobh:
			set_opt(sbi->s_mount_opt, NOBH);
			break;
		case Opt_bh:
			clear_opt(sbi->s_mount_opt, NOBH);
			break;
		case Opt_extents:
			if (!EXT4_HAS_INCOMPAT_FEATURE(sb,
					EXT4_FEATURE_INCOMPAT_EXTENTS)) {
				ext4_warning(sb, __func__,
					"extents feature not enabled "
					"on this filesystem, use tune2fs");
				return 0;
			}
			set_opt(sbi->s_mount_opt, EXTENTS);
			break;
		case Opt_noextents:
			/*
			 * When e2fsprogs support resizing an already existing
			 * ext3 file system to greater than 2**32 we need to
			 * add support to block allocator to handle growing
			 * already existing block  mapped inode so that blocks
			 * allocated for them fall within 2**32
			 */
			last_block = ext4_blocks_count(sbi->s_es) - 1;
			if (last_block  > 0xffffffffULL) {
				printk(KERN_ERR "EXT4-fs: Filesystem too "
						"large to mount with "
						"-o noextents options\n");
				return 0;
			}
			clear_opt(sbi->s_mount_opt, EXTENTS);
			break;
		case Opt_i_version:
			set_opt(sbi->s_mount_opt, I_VERSION);
			sb->s_flags |= MS_I_VERSION;
			break;
		case Opt_nodelalloc:
			clear_opt(sbi->s_mount_opt, DELALLOC);
			break;
		case Opt_stripe:
			if (match_int(&args[0], &option))
				return 0;
			if (option < 0)
				return 0;
			sbi->s_stripe = option;
			break;
		case Opt_delalloc:
			set_opt(sbi->s_mount_opt, DELALLOC);
			break;
		case Opt_inode_readahead_blks:
			if (match_int(&args[0], &option))
				return 0;
			if (option < 0 || option > (1 << 30))
				return 0;
			sbi->s_inode_readahead_blks = option;
			break;
		case Opt_journal_ioprio:
			if (match_int(&args[0], &option))
				return 0;
			if (option < 0 || option > 7)
				break;
			*journal_ioprio = IOPRIO_PRIO_VALUE(IOPRIO_CLASS_BE,
							    option);
			break;
		default:
			printk(KERN_ERR
			       "EXT4-fs: Unrecognized mount option \"%s\" "
			       "or missing value\n", p);
			return 0;
		}
	}
#ifdef CONFIG_QUOTA
	if (sbi->s_qf_names[USRQUOTA] || sbi->s_qf_names[GRPQUOTA]) {
		if ((sbi->s_mount_opt & EXT4_MOUNT_USRQUOTA) &&
		     sbi->s_qf_names[USRQUOTA])
			clear_opt(sbi->s_mount_opt, USRQUOTA);

		if ((sbi->s_mount_opt & EXT4_MOUNT_GRPQUOTA) &&
		     sbi->s_qf_names[GRPQUOTA])
			clear_opt(sbi->s_mount_opt, GRPQUOTA);

		if ((sbi->s_qf_names[USRQUOTA] &&
				(sbi->s_mount_opt & EXT4_MOUNT_GRPQUOTA)) ||
		    (sbi->s_qf_names[GRPQUOTA] &&
				(sbi->s_mount_opt & EXT4_MOUNT_USRQUOTA))) {
			printk(KERN_ERR "EXT4-fs: old and new quota "
					"format mixing.\n");
			return 0;
		}

		if (!sbi->s_jquota_fmt) {
			printk(KERN_ERR "EXT4-fs: journaled quota format "
					"not specified.\n");
			return 0;
		}
	} else {
		if (sbi->s_jquota_fmt) {
			printk(KERN_ERR "EXT4-fs: journaled quota format "
					"specified with no journaling "
					"enabled.\n");
			return 0;
		}
	}
#endif
	return 1;
}