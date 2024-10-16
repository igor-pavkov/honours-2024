static inline void ext4_show_quota_options(struct seq_file *seq,
					   struct super_block *sb)
{
#if defined(CONFIG_QUOTA)
	struct ext4_sb_info *sbi = EXT4_SB(sb);

	if (sbi->s_jquota_fmt)
		seq_printf(seq, ",jqfmt=%s",
		(sbi->s_jquota_fmt == QFMT_VFS_OLD) ? "vfsold" : "vfsv0");

	if (sbi->s_qf_names[USRQUOTA])
		seq_printf(seq, ",usrjquota=%s", sbi->s_qf_names[USRQUOTA]);

	if (sbi->s_qf_names[GRPQUOTA])
		seq_printf(seq, ",grpjquota=%s", sbi->s_qf_names[GRPQUOTA]);

	if (sbi->s_mount_opt & EXT4_MOUNT_USRQUOTA)
		seq_puts(seq, ",usrquota");

	if (sbi->s_mount_opt & EXT4_MOUNT_GRPQUOTA)
		seq_puts(seq, ",grpquota");
#endif
}