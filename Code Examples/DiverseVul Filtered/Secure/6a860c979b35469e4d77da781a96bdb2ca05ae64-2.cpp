static long do_splice_to(struct file *in, loff_t *ppos,
			 struct pipe_inode_info *pipe, size_t len,
			 unsigned int flags)
{
	int ret;

	if (unlikely(!in->f_op || !in->f_op->splice_read))
		return -EINVAL;

	if (unlikely(!(in->f_mode & FMODE_READ)))
		return -EBADF;

	ret = rw_verify_area(READ, in, ppos, len);
	if (unlikely(ret < 0))
		return ret;

	ret = security_file_permission(in, MAY_READ);
	if (unlikely(ret < 0))
		return ret;

	return in->f_op->splice_read(in, ppos, pipe, len, flags);
}