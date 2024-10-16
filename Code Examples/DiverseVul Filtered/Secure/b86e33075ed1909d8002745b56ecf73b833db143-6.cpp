static ssize_t f2fs_direct_IO(struct kiocb *iocb, struct iov_iter *iter)
{
	struct address_space *mapping = iocb->ki_filp->f_mapping;
	struct inode *inode = mapping->host;
	size_t count = iov_iter_count(iter);
	loff_t offset = iocb->ki_pos;
	int rw = iov_iter_rw(iter);
	int err;

	err = check_direct_IO(inode, iter, offset);
	if (err)
		return err;

	if (__force_buffered_io(inode, rw))
		return 0;

	trace_f2fs_direct_IO_enter(inode, offset, count, rw);

	down_read(&F2FS_I(inode)->dio_rwsem[rw]);
	err = blockdev_direct_IO(iocb, inode, iter, get_data_block_dio);
	up_read(&F2FS_I(inode)->dio_rwsem[rw]);

	if (rw == WRITE) {
		if (err > 0)
			set_inode_flag(inode, FI_UPDATE_WRITE);
		else if (err < 0)
			f2fs_write_failed(mapping, offset + count);
	}

	trace_f2fs_direct_IO_exit(inode, offset, count, rw, err);

	return err;
}