static ssize_t ocfs2_file_write_iter(struct kiocb *iocb,
				    struct iov_iter *from)
{
	int direct_io, rw_level;
	ssize_t written = 0;
	ssize_t ret;
	size_t count = iov_iter_count(from);
	struct file *file = iocb->ki_filp;
	struct inode *inode = file_inode(file);
	struct ocfs2_super *osb = OCFS2_SB(inode->i_sb);
	int full_coherency = !(osb->s_mount_opt &
			       OCFS2_MOUNT_COHERENCY_BUFFERED);
	void *saved_ki_complete = NULL;
	int append_write = ((iocb->ki_pos + count) >=
			i_size_read(inode) ? 1 : 0);

	trace_ocfs2_file_aio_write(inode, file, file->f_path.dentry,
		(unsigned long long)OCFS2_I(inode)->ip_blkno,
		file->f_path.dentry->d_name.len,
		file->f_path.dentry->d_name.name,
		(unsigned int)from->nr_segs);	/* GRRRRR */

	if (count == 0)
		return 0;

	direct_io = iocb->ki_flags & IOCB_DIRECT ? 1 : 0;

	inode_lock(inode);

	/*
	 * Concurrent O_DIRECT writes are allowed with
	 * mount_option "coherency=buffered".
	 * For append write, we must take rw EX.
	 */
	rw_level = (!direct_io || full_coherency || append_write);

	ret = ocfs2_rw_lock(inode, rw_level);
	if (ret < 0) {
		mlog_errno(ret);
		goto out_mutex;
	}

	/*
	 * O_DIRECT writes with "coherency=full" need to take EX cluster
	 * inode_lock to guarantee coherency.
	 */
	if (direct_io && full_coherency) {
		/*
		 * We need to take and drop the inode lock to force
		 * other nodes to drop their caches.  Buffered I/O
		 * already does this in write_begin().
		 */
		ret = ocfs2_inode_lock(inode, NULL, 1);
		if (ret < 0) {
			mlog_errno(ret);
			goto out;
		}

		ocfs2_inode_unlock(inode, 1);
	}

	ret = generic_write_checks(iocb, from);
	if (ret <= 0) {
		if (ret)
			mlog_errno(ret);
		goto out;
	}
	count = ret;

	ret = ocfs2_prepare_inode_for_write(file, iocb->ki_pos, count);
	if (ret < 0) {
		mlog_errno(ret);
		goto out;
	}

	if (direct_io && !is_sync_kiocb(iocb) &&
	    ocfs2_is_io_unaligned(inode, count, iocb->ki_pos)) {
		/*
		 * Make it a sync io if it's an unaligned aio.
		 */
		saved_ki_complete = xchg(&iocb->ki_complete, NULL);
	}

	/* communicate with ocfs2_dio_end_io */
	ocfs2_iocb_set_rw_locked(iocb, rw_level);

	written = __generic_file_write_iter(iocb, from);
	/* buffered aio wouldn't have proper lock coverage today */
	BUG_ON(written == -EIOCBQUEUED && !(iocb->ki_flags & IOCB_DIRECT));

	/*
	 * deep in g_f_a_w_n()->ocfs2_direct_IO we pass in a ocfs2_dio_end_io
	 * function pointer which is called when o_direct io completes so that
	 * it can unlock our rw lock.
	 * Unfortunately there are error cases which call end_io and others
	 * that don't.  so we don't have to unlock the rw_lock if either an
	 * async dio is going to do it in the future or an end_io after an
	 * error has already done it.
	 */
	if ((written == -EIOCBQUEUED) || (!ocfs2_iocb_is_rw_locked(iocb))) {
		rw_level = -1;
	}

	if (unlikely(written <= 0))
		goto out;

	if (((file->f_flags & O_DSYNC) && !direct_io) ||
	    IS_SYNC(inode)) {
		ret = filemap_fdatawrite_range(file->f_mapping,
					       iocb->ki_pos - written,
					       iocb->ki_pos - 1);
		if (ret < 0)
			written = ret;

		if (!ret) {
			ret = jbd2_journal_force_commit(osb->journal->j_journal);
			if (ret < 0)
				written = ret;
		}

		if (!ret)
			ret = filemap_fdatawait_range(file->f_mapping,
						      iocb->ki_pos - written,
						      iocb->ki_pos - 1);
	}

out:
	if (saved_ki_complete)
		xchg(&iocb->ki_complete, saved_ki_complete);

	if (rw_level != -1)
		ocfs2_rw_unlock(inode, rw_level);

out_mutex:
	inode_unlock(inode);

	if (written)
		ret = written;
	return ret;
}