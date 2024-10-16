void rds_rdma_unuse(struct rds_sock *rs, u32 r_key, int force)
{
	struct rds_mr *mr;
	unsigned long flags;
	int zot_me = 0;

	spin_lock_irqsave(&rs->rs_rdma_lock, flags);
	mr = rds_mr_tree_walk(&rs->rs_rdma_keys, r_key, NULL);
	if (!mr) {
		pr_debug("rds: trying to unuse MR with unknown r_key %u!\n",
			 r_key);
		spin_unlock_irqrestore(&rs->rs_rdma_lock, flags);
		return;
	}

	if (mr->r_use_once || force) {
		rb_erase(&mr->r_rb_node, &rs->rs_rdma_keys);
		RB_CLEAR_NODE(&mr->r_rb_node);
		zot_me = 1;
	}
	spin_unlock_irqrestore(&rs->rs_rdma_lock, flags);

	/* May have to issue a dma_sync on this memory region.
	 * Note we could avoid this if the operation was a RDMA READ,
	 * but at this point we can't tell. */
	if (mr->r_trans->sync_mr)
		mr->r_trans->sync_mr(mr->r_trans_private, DMA_FROM_DEVICE);

	/* If the MR was marked as invalidate, this will
	 * trigger an async flush. */
	if (zot_me) {
		rds_destroy_mr(mr);
		rds_mr_put(mr);
	}
}