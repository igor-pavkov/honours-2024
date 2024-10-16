static int __btrfs_submit_bio_done(struct inode *inode, int rw, struct bio *bio,
			  int mirror_num, unsigned long bio_flags,
			  u64 bio_offset)
{
	struct btrfs_root *root = BTRFS_I(inode)->root;
	int ret;

	ret = btrfs_map_bio(root, rw, bio, mirror_num, 1);
	if (ret) {
		bio->bi_error = ret;
		bio_endio(bio);
	}
	return ret;
}