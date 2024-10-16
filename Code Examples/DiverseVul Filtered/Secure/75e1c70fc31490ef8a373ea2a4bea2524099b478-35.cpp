static struct kiocb *__aio_get_req(struct kioctx *ctx)
{
	struct kiocb *req = NULL;
	struct aio_ring *ring;
	int okay = 0;

	req = kmem_cache_alloc(kiocb_cachep, GFP_KERNEL);
	if (unlikely(!req))
		return NULL;

	req->ki_flags = 0;
	req->ki_users = 2;
	req->ki_key = 0;
	req->ki_ctx = ctx;
	req->ki_cancel = NULL;
	req->ki_retry = NULL;
	req->ki_dtor = NULL;
	req->private = NULL;
	req->ki_iovec = NULL;
	INIT_LIST_HEAD(&req->ki_run_list);
	req->ki_eventfd = NULL;

	/* Check if the completion queue has enough free space to
	 * accept an event from this io.
	 */
	spin_lock_irq(&ctx->ctx_lock);
	ring = kmap_atomic(ctx->ring_info.ring_pages[0], KM_USER0);
	if (ctx->reqs_active < aio_ring_avail(&ctx->ring_info, ring)) {
		list_add(&req->ki_list, &ctx->active_reqs);
		ctx->reqs_active++;
		okay = 1;
	}
	kunmap_atomic(ring, KM_USER0);
	spin_unlock_irq(&ctx->ctx_lock);

	if (!okay) {
		kmem_cache_free(kiocb_cachep, req);
		req = NULL;
	}

	return req;
}