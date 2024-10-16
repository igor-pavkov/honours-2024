int ext4_group_desc_csum_verify(struct ext4_sb_info *sbi, __u32 block_group,
				struct ext4_group_desc *gdp)
{
	if ((sbi->s_es->s_feature_ro_compat &
	     cpu_to_le32(EXT4_FEATURE_RO_COMPAT_GDT_CSUM)) &&
	    (gdp->bg_checksum != ext4_group_desc_csum(sbi, block_group, gdp)))
		return 0;

	return 1;
}