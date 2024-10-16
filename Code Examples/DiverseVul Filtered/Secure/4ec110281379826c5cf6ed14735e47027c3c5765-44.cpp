__u32 ext4_used_dirs_count(struct super_block *sb,
			      struct ext4_group_desc *bg)
{
	return le16_to_cpu(bg->bg_used_dirs_count_lo) |
		(EXT4_DESC_SIZE(sb) >= EXT4_MIN_DESC_SIZE_64BIT ?
		(__u32)le16_to_cpu(bg->bg_used_dirs_count_hi) << 16 : 0);
}