static int read_raw_super_block(struct f2fs_sb_info *sbi,
			struct f2fs_super_block **raw_super,
			int *valid_super_block, int *recovery)
{
	struct super_block *sb = sbi->sb;
	int block;
	struct buffer_head *bh;
	struct f2fs_super_block *super;
	int err = 0;

	super = kzalloc(sizeof(struct f2fs_super_block), GFP_KERNEL);
	if (!super)
		return -ENOMEM;

	for (block = 0; block < 2; block++) {
		bh = sb_bread(sb, block);
		if (!bh) {
			f2fs_msg(sb, KERN_ERR, "Unable to read %dth superblock",
				block + 1);
			err = -EIO;
			continue;
		}

		/* sanity checking of raw super */
		if (sanity_check_raw_super(sbi, bh)) {
			f2fs_msg(sb, KERN_ERR,
				"Can't find valid F2FS filesystem in %dth superblock",
				block + 1);
			err = -EINVAL;
			brelse(bh);
			continue;
		}

		if (!*raw_super) {
			memcpy(super, bh->b_data + F2FS_SUPER_OFFSET,
							sizeof(*super));
			*valid_super_block = block;
			*raw_super = super;
		}
		brelse(bh);
	}

	/* Fail to read any one of the superblocks*/
	if (err < 0)
		*recovery = 1;

	/* No valid superblock */
	if (!*raw_super)
		kfree(super);
	else
		err = 0;

	return err;
}