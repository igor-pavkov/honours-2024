static int dev_status(struct file *filp, struct dm_ioctl *param, size_t param_size)
{
	struct mapped_device *md;

	md = find_device(param);
	if (!md)
		return -ENXIO;

	__dev_status(md, param);
	dm_put(md);

	return 0;
}