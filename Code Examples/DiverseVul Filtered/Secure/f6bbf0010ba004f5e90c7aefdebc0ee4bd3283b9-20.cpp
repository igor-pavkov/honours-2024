static ssize_t vhost_vdpa_chr_write_iter(struct kiocb *iocb,
					 struct iov_iter *from)
{
	struct file *file = iocb->ki_filp;
	struct vhost_vdpa *v = file->private_data;
	struct vhost_dev *dev = &v->vdev;

	return vhost_chr_write_iter(dev, from);
}