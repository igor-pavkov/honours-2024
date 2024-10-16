void rds_rdma_drop_keys(struct rds_sock *rs)
{
	struct rds_mr *mr;
	struct rb_node *node;
	unsigned long flags;

	/* Release any MRs associated with this socket */
	spin_lock_irqsave(&rs->rs_rdma_lock, flags);
	while ((node = rb_first(&rs->rs_rdma_keys))) {
		mr = rb_entry(node, struct rds_mr, r_rb_node);
		if (mr->r_trans == rs->rs_transport)
			mr->r_invalidate = 0;
		rb_erase(&mr->r_rb_node, &rs->rs_rdma_keys);
		RB_CLEAR_NODE(&mr->r_rb_node);
		spin_unlock_irqrestore(&rs->rs_rdma_lock, flags);
		rds_destroy_mr(mr);
		rds_mr_put(mr);
		spin_lock_irqsave(&rs->rs_rdma_lock, flags);
	}
	spin_unlock_irqrestore(&rs->rs_rdma_lock, flags);

	if (rs->rs_transport && rs->rs_transport->flush_mrs)
		rs->rs_transport->flush_mrs();
}