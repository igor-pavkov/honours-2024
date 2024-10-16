ssize_t generic_file_aio_write_nolock(struct kiocb *iocb,
		const struct iovec *iov, unsigned long nr_segs, loff_t pos)
{
	struct file *file = iocb->ki_filp;
	struct address_space *mapping = file->f_mapping;
	struct inode *inode = mapping->host;
	ssize_t ret;

	BUG_ON(iocb->ki_pos != pos);

	ret = __generic_file_aio_write_nolock(iocb, iov, nr_segs,
			&iocb->ki_pos);

	if (ret > 0 && ((file->f_flags & O_SYNC) || IS_SYNC(inode))) {
		ssize_t err;

		err = sync_page_range_nolock(inode, mapping, pos, ret);
		if (err < 0)
			ret = err;
	}
	return ret;
}