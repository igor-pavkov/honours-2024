static __poll_t perf_poll(struct file *file, poll_table *wait)
{
	struct perf_event *event = file->private_data;
	struct perf_buffer *rb;
	__poll_t events = EPOLLHUP;

	poll_wait(file, &event->waitq, wait);

	if (is_event_hup(event))
		return events;

	/*
	 * Pin the event->rb by taking event->mmap_mutex; otherwise
	 * perf_event_set_output() can swizzle our rb and make us miss wakeups.
	 */
	mutex_lock(&event->mmap_mutex);
	rb = event->rb;
	if (rb)
		events = atomic_xchg(&rb->poll, 0);
	mutex_unlock(&event->mmap_mutex);
	return events;
}