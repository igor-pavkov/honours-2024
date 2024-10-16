static int ext4_quota_on_mount(struct super_block *sb, int type)
{
	return vfs_quota_on_mount(sb, EXT4_SB(sb)->s_qf_names[type],
			EXT4_SB(sb)->s_jquota_fmt, type);
}