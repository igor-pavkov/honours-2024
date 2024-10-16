static __poll_t hiddev_poll(struct file *file, poll_table *wait)
{
	struct hiddev_list *list = file->private_data;

	poll_wait(file, &list->hiddev->wait, wait);
	if (list->head != list->tail)
		return EPOLLIN | EPOLLRDNORM;
	if (!list->hiddev->exist)
		return EPOLLERR | EPOLLHUP;
	return 0;
}