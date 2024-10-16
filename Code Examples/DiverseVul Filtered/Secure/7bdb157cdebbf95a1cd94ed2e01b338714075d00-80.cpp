struct perf_buffer *ring_buffer_get(struct perf_event *event)
{
	struct perf_buffer *rb;

	rcu_read_lock();
	rb = rcu_dereference(event->rb);
	if (rb) {
		if (!refcount_inc_not_zero(&rb->refcount))
			rb = NULL;
	}
	rcu_read_unlock();

	return rb;
}