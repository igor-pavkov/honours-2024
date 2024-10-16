static __poll_t iowarrior_poll(struct file *file, poll_table * wait)
{
	struct iowarrior *dev = file->private_data;
	__poll_t mask = 0;

	if (!dev->present)
		return EPOLLERR | EPOLLHUP;

	poll_wait(file, &dev->read_wait, wait);
	poll_wait(file, &dev->write_wait, wait);

	if (!dev->present)
		return EPOLLERR | EPOLLHUP;

	if (read_index(dev) != -1)
		mask |= EPOLLIN | EPOLLRDNORM;

	if (atomic_read(&dev->write_busy) < MAX_WRITES_IN_FLIGHT)
		mask |= EPOLLOUT | EPOLLWRNORM;
	return mask;
}