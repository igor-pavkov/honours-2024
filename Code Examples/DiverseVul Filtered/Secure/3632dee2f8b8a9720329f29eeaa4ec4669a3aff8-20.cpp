static void remove_kevent(struct inotify_device *dev,
			  struct inotify_kernel_event *kevent)
{
	list_del(&kevent->list);

	dev->event_count--;
	dev->queue_size -= sizeof(struct inotify_event) + kevent->event.len;
}