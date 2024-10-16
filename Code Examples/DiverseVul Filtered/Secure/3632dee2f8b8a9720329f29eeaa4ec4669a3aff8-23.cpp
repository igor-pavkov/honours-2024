static unsigned int inotify_poll(struct file *file, poll_table *wait)
{
	struct inotify_device *dev = file->private_data;
	int ret = 0;

	poll_wait(file, &dev->wq, wait);
	mutex_lock(&dev->ev_mutex);
	if (!list_empty(&dev->events))
		ret = POLLIN | POLLRDNORM;
	mutex_unlock(&dev->ev_mutex);

	return ret;
}