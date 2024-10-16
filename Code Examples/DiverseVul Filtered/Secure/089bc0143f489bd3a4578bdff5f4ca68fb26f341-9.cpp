static void free_req(struct xen_blkif_ring *ring, struct pending_req *req)
{
	unsigned long flags;
	int was_empty;

	spin_lock_irqsave(&ring->pending_free_lock, flags);
	was_empty = list_empty(&ring->pending_free);
	list_add(&req->free_list, &ring->pending_free);
	spin_unlock_irqrestore(&ring->pending_free_lock, flags);
	if (was_empty)
		wake_up(&ring->pending_free_wq);
}