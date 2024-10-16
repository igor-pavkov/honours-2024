static bool __same_bdev(struct f2fs_sb_info *sbi,
				block_t blk_addr, struct bio *bio)
{
	return f2fs_target_device(sbi, blk_addr, NULL) == bio->bi_bdev;
}