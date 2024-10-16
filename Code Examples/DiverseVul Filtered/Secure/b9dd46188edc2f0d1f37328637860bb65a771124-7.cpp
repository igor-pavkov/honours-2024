int f2fs_commit_super(struct f2fs_sb_info *sbi, bool recover)
{
	struct buffer_head *bh;
	int err;

	if ((recover && f2fs_readonly(sbi->sb)) ||
				bdev_read_only(sbi->sb->s_bdev)) {
		set_sbi_flag(sbi, SBI_NEED_SB_WRITE);
		return -EROFS;
	}

	/* write back-up superblock first */
	bh = sb_getblk(sbi->sb, sbi->valid_super_block ? 0: 1);
	if (!bh)
		return -EIO;
	err = __f2fs_commit_super(bh, F2FS_RAW_SUPER(sbi));
	brelse(bh);

	/* if we are in recovery path, skip writing valid superblock */
	if (recover || err)
		return err;

	/* write current valid superblock */
	bh = sb_getblk(sbi->sb, sbi->valid_super_block);
	if (!bh)
		return -EIO;
	err = __f2fs_commit_super(bh, F2FS_RAW_SUPER(sbi));
	brelse(bh);
	return err;
}