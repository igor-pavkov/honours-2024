static int ext4_check_descriptors(struct super_block *sb)
{
	struct ext4_sb_info *sbi = EXT4_SB(sb);
	ext4_fsblk_t first_block = le32_to_cpu(sbi->s_es->s_first_data_block);
	ext4_fsblk_t last_block;
	ext4_fsblk_t block_bitmap;
	ext4_fsblk_t inode_bitmap;
	ext4_fsblk_t inode_table;
	int flexbg_flag = 0;
	ext4_group_t i;

	if (EXT4_HAS_INCOMPAT_FEATURE(sb, EXT4_FEATURE_INCOMPAT_FLEX_BG))
		flexbg_flag = 1;

	ext4_debug("Checking group descriptors");

	for (i = 0; i < sbi->s_groups_count; i++) {
		struct ext4_group_desc *gdp = ext4_get_group_desc(sb, i, NULL);

		if (i == sbi->s_groups_count - 1 || flexbg_flag)
			last_block = ext4_blocks_count(sbi->s_es) - 1;
		else
			last_block = first_block +
				(EXT4_BLOCKS_PER_GROUP(sb) - 1);

		block_bitmap = ext4_block_bitmap(sb, gdp);
		if (block_bitmap < first_block || block_bitmap > last_block) {
			printk(KERN_ERR "EXT4-fs: ext4_check_descriptors: "
			       "Block bitmap for group %u not in group "
			       "(block %llu)!\n", i, block_bitmap);
			return 0;
		}
		inode_bitmap = ext4_inode_bitmap(sb, gdp);
		if (inode_bitmap < first_block || inode_bitmap > last_block) {
			printk(KERN_ERR "EXT4-fs: ext4_check_descriptors: "
			       "Inode bitmap for group %u not in group "
			       "(block %llu)!\n", i, inode_bitmap);
			return 0;
		}
		inode_table = ext4_inode_table(sb, gdp);
		if (inode_table < first_block ||
		    inode_table + sbi->s_itb_per_group - 1 > last_block) {
			printk(KERN_ERR "EXT4-fs: ext4_check_descriptors: "
			       "Inode table for group %u not in group "
			       "(block %llu)!\n", i, inode_table);
			return 0;
		}
		spin_lock(sb_bgl_lock(sbi, i));
		if (!ext4_group_desc_csum_verify(sbi, i, gdp)) {
			printk(KERN_ERR "EXT4-fs: ext4_check_descriptors: "
			       "Checksum for group %u failed (%u!=%u)\n",
			       i, le16_to_cpu(ext4_group_desc_csum(sbi, i,
			       gdp)), le16_to_cpu(gdp->bg_checksum));
			if (!(sb->s_flags & MS_RDONLY)) {
				spin_unlock(sb_bgl_lock(sbi, i));
				return 0;
			}
		}
		spin_unlock(sb_bgl_lock(sbi, i));
		if (!flexbg_flag)
			first_block += EXT4_BLOCKS_PER_GROUP(sb);
	}

	ext4_free_blocks_count_set(sbi->s_es, ext4_count_free_blocks(sb));
	sbi->s_es->s_free_inodes_count = cpu_to_le32(ext4_count_free_inodes(sb));
	return 1;
}