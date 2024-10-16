static void ocfs2_dio_end_io(struct kiocb *iocb,
			     loff_t offset,
			     ssize_t bytes,
			     void *private,
			     int ret,
			     bool is_async)
{
	struct inode *inode = iocb->ki_filp->f_path.dentry->d_inode;
	int level;

	/* this io's submitter should not have unlocked this before we could */
	BUG_ON(!ocfs2_iocb_is_rw_locked(iocb));

	if (ocfs2_iocb_is_sem_locked(iocb)) {
		up_read(&inode->i_alloc_sem);
		ocfs2_iocb_clear_sem_locked(iocb);
	}

	ocfs2_iocb_clear_rw_locked(iocb);

	level = ocfs2_iocb_rw_locked_level(iocb);
	ocfs2_rw_unlock(inode, level);

	if (is_async)
		aio_complete(iocb, ret, 0);
}