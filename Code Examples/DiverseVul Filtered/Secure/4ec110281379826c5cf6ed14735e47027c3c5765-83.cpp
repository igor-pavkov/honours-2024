static int ext4_setup_super(struct super_block *sb, struct ext4_super_block *es,
			    int read_only)
{
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	int res = 0;

	if (le32_to_cpu(es->s_rev_level) > EXT4_MAX_SUPP_REV) {
		printk(KERN_ERR "EXT4-fs warning: revision level too high, "
		       "forcing read-only mode\n");
		res = MS_RDONLY;
	}
	if (read_only)
		return res;
	if (!(sbi->s_mount_state & EXT4_VALID_FS))
		printk(KERN_WARNING "EXT4-fs warning: mounting unchecked fs, "
		       "running e2fsck is recommended\n");
	else if ((sbi->s_mount_state & EXT4_ERROR_FS))
		printk(KERN_WARNING
		       "EXT4-fs warning: mounting fs with errors, "
		       "running e2fsck is recommended\n");
	else if ((__s16) le16_to_cpu(es->s_max_mnt_count) >= 0 &&
		 le16_to_cpu(es->s_mnt_count) >=
		 (unsigned short) (__s16) le16_to_cpu(es->s_max_mnt_count))
		printk(KERN_WARNING
		       "EXT4-fs warning: maximal mount count reached, "
		       "running e2fsck is recommended\n");
	else if (le32_to_cpu(es->s_checkinterval) &&
		(le32_to_cpu(es->s_lastcheck) +
			le32_to_cpu(es->s_checkinterval) <= get_seconds()))
		printk(KERN_WARNING
		       "EXT4-fs warning: checktime reached, "
		       "running e2fsck is recommended\n");
	if (!sbi->s_journal) 
		es->s_state &= cpu_to_le16(~EXT4_VALID_FS);
	if (!(__s16) le16_to_cpu(es->s_max_mnt_count))
		es->s_max_mnt_count = cpu_to_le16(EXT4_DFL_MAX_MNT_COUNT);
	le16_add_cpu(&es->s_mnt_count, 1);
	es->s_mtime = cpu_to_le32(get_seconds());
	ext4_update_dynamic_rev(sb);
	if (sbi->s_journal)
		EXT4_SET_INCOMPAT_FEATURE(sb, EXT4_FEATURE_INCOMPAT_RECOVER);

	ext4_commit_super(sb, es, 1);
	if (test_opt(sb, DEBUG))
		printk(KERN_INFO "[EXT4 FS bs=%lu, gc=%u, "
				"bpg=%lu, ipg=%lu, mo=%04lx]\n",
			sb->s_blocksize,
			sbi->s_groups_count,
			EXT4_BLOCKS_PER_GROUP(sb),
			EXT4_INODES_PER_GROUP(sb),
			sbi->s_mount_opt);

	if (EXT4_SB(sb)->s_journal) {
		printk(KERN_INFO "EXT4 FS on %s, %s journal on %s\n",
		       sb->s_id, EXT4_SB(sb)->s_journal->j_inode ? "internal" :
		       "external", EXT4_SB(sb)->s_journal->j_devname);
	} else {
		printk(KERN_INFO "EXT4 FS on %s, no journal\n", sb->s_id);
	}
	return res;
}