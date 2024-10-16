static ssize_t aio_rw_vect_retry(struct kiocb *iocb)
{
	struct file *file = iocb->ki_filp;
	struct address_space *mapping = file->f_mapping;
	struct inode *inode = mapping->host;
	ssize_t (*rw_op)(struct kiocb *, const struct iovec *,
			 unsigned long, loff_t);
	ssize_t ret = 0;
	unsigned short opcode;

	if ((iocb->ki_opcode == IOCB_CMD_PREADV) ||
		(iocb->ki_opcode == IOCB_CMD_PREAD)) {
		rw_op = file->f_op->aio_read;
		opcode = IOCB_CMD_PREADV;
	} else {
		rw_op = file->f_op->aio_write;
		opcode = IOCB_CMD_PWRITEV;
	}

	/* This matches the pread()/pwrite() logic */
	if (iocb->ki_pos < 0)
		return -EINVAL;

	do {
		ret = rw_op(iocb, &iocb->ki_iovec[iocb->ki_cur_seg],
			    iocb->ki_nr_segs - iocb->ki_cur_seg,
			    iocb->ki_pos);
		if (ret > 0)
			aio_advance_iovec(iocb, ret);

	/* retry all partial writes.  retry partial reads as long as its a
	 * regular file. */
	} while (ret > 0 && iocb->ki_left > 0 &&
		 (opcode == IOCB_CMD_PWRITEV ||
		  (!S_ISFIFO(inode->i_mode) && !S_ISSOCK(inode->i_mode))));

	/* This means we must have transferred all that we could */
	/* No need to retry anymore */
	if ((ret == 0) || (iocb->ki_left == 0))
		ret = iocb->ki_nbytes - iocb->ki_left;

	/* If we managed to write some out we return that, rather than
	 * the eventual error. */
	if (opcode == IOCB_CMD_PWRITEV
	    && ret < 0 && ret != -EIOCBQUEUED && ret != -EIOCBRETRY
	    && iocb->ki_nbytes - iocb->ki_left)
		ret = iocb->ki_nbytes - iocb->ki_left;

	return ret;
}