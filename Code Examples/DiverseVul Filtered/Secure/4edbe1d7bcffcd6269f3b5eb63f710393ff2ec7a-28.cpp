static int dev_arm_poll(struct file *filp, struct dm_ioctl *param, size_t param_size)
{
	struct dm_file *priv = filp->private_data;

	priv->global_event_nr = atomic_read(&dm_global_event_nr);

	return 0;
}