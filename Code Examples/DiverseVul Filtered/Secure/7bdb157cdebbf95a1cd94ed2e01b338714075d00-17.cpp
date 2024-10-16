void ring_buffer_put(struct perf_buffer *rb)
{
	if (!refcount_dec_and_test(&rb->refcount))
		return;

	WARN_ON_ONCE(!list_empty(&rb->event_list));

	call_rcu(&rb->rcu_head, rb_free_rcu);
}