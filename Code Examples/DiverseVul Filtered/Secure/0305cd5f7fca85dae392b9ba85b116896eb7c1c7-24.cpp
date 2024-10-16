static int btrfs_subio_endio_read(struct inode *inode,
				  struct btrfs_io_bio *io_bio, int err)
{
	bool skip_csum = BTRFS_I(inode)->flags & BTRFS_INODE_NODATASUM;

	if (skip_csum) {
		if (unlikely(err))
			return __btrfs_correct_data_nocsum(inode, io_bio);
		else
			return 0;
	} else {
		return __btrfs_subio_endio_read(inode, io_bio, err);
	}
}