static void perf_event_init_userpage(struct perf_event *event)
{
	struct perf_event_mmap_page *userpg;
	struct perf_buffer *rb;

	rcu_read_lock();
	rb = rcu_dereference(event->rb);
	if (!rb)
		goto unlock;

	userpg = rb->user_page;

	/* Allow new userspace to detect that bit 0 is deprecated */
	userpg->cap_bit0_is_deprecated = 1;
	userpg->size = offsetof(struct perf_event_mmap_page, __reserved);
	userpg->data_offset = PAGE_SIZE;
	userpg->data_size = perf_data_size(rb);

unlock:
	rcu_read_unlock();
}