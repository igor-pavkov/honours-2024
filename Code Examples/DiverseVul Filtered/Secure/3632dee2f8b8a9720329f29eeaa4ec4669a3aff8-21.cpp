static struct inotify_kernel_event *get_one_event(struct inotify_device *dev,
						  size_t count)
{
	size_t event_size = sizeof(struct inotify_event);
	struct inotify_kernel_event *kevent;

	if (list_empty(&dev->events))
		return NULL;

	kevent = inotify_dev_get_event(dev);
	if (kevent->name)
		event_size += kevent->event.len;

	if (event_size > count)
		return ERR_PTR(-EINVAL);

	remove_kevent(dev, kevent);
	return kevent;
}