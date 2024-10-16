static void inotify_dev_event_dequeue(struct inotify_device *dev)
{
	if (!list_empty(&dev->events)) {
		struct inotify_kernel_event *kevent;
		kevent = inotify_dev_get_event(dev);
		remove_kevent(dev, kevent);
		free_kevent(kevent);
	}
}