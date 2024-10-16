static ssize_t aio_fsync(struct kiocb *iocb)
{
	struct file *file = iocb->ki_filp;
	ssize_t ret = -EINVAL;

	if (file->f_op->aio_fsync)
		ret = file->f_op->aio_fsync(iocb, 0);
	return ret;
}