static ssize_t udf_direct_IO(struct kiocb *iocb, struct iov_iter *iter)
{
	struct file *file = iocb->ki_filp;
	struct address_space *mapping = file->f_mapping;
	struct inode *inode = mapping->host;
	size_t count = iov_iter_count(iter);
	ssize_t ret;

	ret = blockdev_direct_IO(iocb, inode, iter, udf_get_block);
	if (unlikely(ret < 0 && iov_iter_rw(iter) == WRITE))
		udf_write_failed(mapping, iocb->ki_pos + count);
	return ret;
}