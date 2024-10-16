static unsigned long ext4_get_stripe_size(struct ext4_sb_info *sbi)
{
	unsigned long stride = le16_to_cpu(sbi->s_es->s_raid_stride);
	unsigned long stripe_width =
			le32_to_cpu(sbi->s_es->s_raid_stripe_width);

	if (sbi->s_stripe && sbi->s_stripe <= sbi->s_blocks_per_group)
		return sbi->s_stripe;

	if (stripe_width <= sbi->s_blocks_per_group)
		return stripe_width;

	if (stride <= sbi->s_blocks_per_group)
		return stride;

	return 0;
}