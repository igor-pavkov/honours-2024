static ssize_t vhost_net_chr_write_iter(struct kiocb *iocb,
					struct iov_iter *from)
{
	struct file *file = iocb->ki_filp;
	struct vhost_net *n = file->private_data;
	struct vhost_dev *dev = &n->dev;

	return vhost_chr_write_iter(dev, from);
}