static struct pending_req *alloc_req(struct xen_blkif_ring *ring)
{
	struct pending_req *req = NULL;
	unsigned long flags;

	spin_lock_irqsave(&ring->pending_free_lock, flags);
	if (!list_empty(&ring->pending_free)) {
		req = list_entry(ring->pending_free.next, struct pending_req,
				 free_list);
		list_del(&req->free_list);
	}
	spin_unlock_irqrestore(&ring->pending_free_lock, flags);
	return req;
}