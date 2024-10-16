static ssize_t copy_event_to_user(struct inotify_kernel_event *kevent,
				  char __user *buf)
{
	size_t event_size = sizeof(struct inotify_event);

	if (copy_to_user(buf, &kevent->event, event_size))
		return -EFAULT;

	if (kevent->name) {
		buf += event_size;

		if (copy_to_user(buf, kevent->name, kevent->event.len))
			return -EFAULT;

		event_size += kevent->event.len;
	}
	return event_size;
}