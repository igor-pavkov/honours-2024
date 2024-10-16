static void ctx_rcu_free(struct rcu_head *head)
{
	struct kioctx *ctx = container_of(head, struct kioctx, rcu_head);
	unsigned nr_events = ctx->max_reqs;

	kmem_cache_free(kioctx_cachep, ctx);

	if (nr_events) {
		spin_lock(&aio_nr_lock);
		BUG_ON(aio_nr - nr_events > aio_nr);
		aio_nr -= nr_events;
		spin_unlock(&aio_nr_lock);
	}
}