static int dm_open(struct inode *inode, struct file *filp)
{
	int r;
	struct dm_file *priv;

	r = nonseekable_open(inode, filp);
	if (unlikely(r))
		return r;

	priv = filp->private_data = kmalloc(sizeof(struct dm_file), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->global_event_nr = atomic_read(&dm_global_event_nr);

	return 0;
}