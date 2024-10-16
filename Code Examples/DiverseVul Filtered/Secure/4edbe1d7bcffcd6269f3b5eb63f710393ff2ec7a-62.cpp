static __poll_t dm_poll(struct file *filp, poll_table *wait)
{
	struct dm_file *priv = filp->private_data;
	__poll_t mask = 0;

	poll_wait(filp, &dm_global_eventq, wait);

	if ((int)(atomic_read(&dm_global_event_nr) - priv->global_event_nr) > 0)
		mask |= EPOLLIN;

	return mask;
}