static ssize_t ocfs2_direct_IO(int rw,
			       struct kiocb *iocb,
			       const struct iovec *iov,
			       loff_t offset,
			       unsigned long nr_segs)
{
	struct file *file = iocb->ki_filp;
	struct inode *inode = file->f_path.dentry->d_inode->i_mapping->host;

	/*
	 * Fallback to buffered I/O if we see an inode without
	 * extents.
	 */
	if (OCFS2_I(inode)->ip_dyn_features & OCFS2_INLINE_DATA_FL)
		return 0;

	/* Fallback to buffered I/O if we are appending. */
	if (i_size_read(inode) <= offset)
		return 0;

	return __blockdev_direct_IO(rw, iocb, inode, inode->i_sb->s_bdev,
				    iov, offset, nr_segs,
				    ocfs2_direct_IO_get_blocks,
				    ocfs2_dio_end_io, NULL, 0);
}