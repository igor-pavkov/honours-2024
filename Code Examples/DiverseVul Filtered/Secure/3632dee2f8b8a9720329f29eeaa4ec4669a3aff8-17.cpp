static void inotify_dev_queue_event(struct inotify_watch *w, u32 wd, u32 mask,
				    u32 cookie, const char *name,
				    struct inode *ignored)
{
	struct inotify_user_watch *watch;
	struct inotify_device *dev;
	struct inotify_kernel_event *kevent, *last;

	watch = container_of(w, struct inotify_user_watch, wdata);
	dev = watch->dev;

	mutex_lock(&dev->ev_mutex);

	/* we can safely put the watch as we don't reference it while
	 * generating the event
	 */
	if (mask & IN_IGNORED || w->mask & IN_ONESHOT)
		put_inotify_watch(w); /* final put */

	/* coalescing: drop this event if it is a dupe of the previous */
	last = inotify_dev_get_last_event(dev);
	if (last && last->event.mask == mask && last->event.wd == wd &&
			last->event.cookie == cookie) {
		const char *lastname = last->name;

		if (!name && !lastname)
			goto out;
		if (name && lastname && !strcmp(lastname, name))
			goto out;
	}

	/* the queue overflowed and we already sent the Q_OVERFLOW event */
	if (unlikely(dev->event_count > dev->max_events))
		goto out;

	/* if the queue overflows, we need to notify user space */
	if (unlikely(dev->event_count == dev->max_events))
		kevent = kernel_event(-1, IN_Q_OVERFLOW, cookie, NULL);
	else
		kevent = kernel_event(wd, mask, cookie, name);

	if (unlikely(!kevent))
		goto out;

	/* queue the event and wake up anyone waiting */
	dev->event_count++;
	dev->queue_size += sizeof(struct inotify_event) + kevent->event.len;
	list_add_tail(&kevent->list, &dev->events);
	wake_up_interruptible(&dev->wq);
	kill_fasync(&dev->fa, SIGIO, POLL_IN);

out:
	mutex_unlock(&dev->ev_mutex);
}