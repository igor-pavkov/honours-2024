static ssize_t ocfs2_file_read_iter(struct kiocb *iocb,
				   struct iov_iter *to)
{
	int ret = 0, rw_level = -1, lock_level = 0;
	struct file *filp = iocb->ki_filp;
	struct inode *inode = file_inode(filp);

	trace_ocfs2_file_aio_read(inode, filp, filp->f_path.dentry,
			(unsigned long long)OCFS2_I(inode)->ip_blkno,
			filp->f_path.dentry->d_name.len,
			filp->f_path.dentry->d_name.name,
			to->nr_segs);	/* GRRRRR */


	if (!inode) {
		ret = -EINVAL;
		mlog_errno(ret);
		goto bail;
	}

	/*
	 * buffered reads protect themselves in ->readpage().  O_DIRECT reads
	 * need locks to protect pending reads from racing with truncate.
	 */
	if (iocb->ki_flags & IOCB_DIRECT) {
		ret = ocfs2_rw_lock(inode, 0);
		if (ret < 0) {
			mlog_errno(ret);
			goto bail;
		}
		rw_level = 0;
		/* communicate with ocfs2_dio_end_io */
		ocfs2_iocb_set_rw_locked(iocb, rw_level);
	}

	/*
	 * We're fine letting folks race truncates and extending
	 * writes with read across the cluster, just like they can
	 * locally. Hence no rw_lock during read.
	 *
	 * Take and drop the meta data lock to update inode fields
	 * like i_size. This allows the checks down below
	 * generic_file_aio_read() a chance of actually working.
	 */
	ret = ocfs2_inode_lock_atime(inode, filp->f_path.mnt, &lock_level);
	if (ret < 0) {
		mlog_errno(ret);
		goto bail;
	}
	ocfs2_inode_unlock(inode, lock_level);

	ret = generic_file_read_iter(iocb, to);
	trace_generic_file_aio_read_ret(ret);

	/* buffered aio wouldn't have proper lock coverage today */
	BUG_ON(ret == -EIOCBQUEUED && !(iocb->ki_flags & IOCB_DIRECT));

	/* see ocfs2_file_write_iter */
	if (ret == -EIOCBQUEUED || !ocfs2_iocb_is_rw_locked(iocb)) {
		rw_level = -1;
	}

bail:
	if (rw_level != -1)
		ocfs2_rw_unlock(inode, rw_level);

	return ret;
}