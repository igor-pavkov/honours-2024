static void rds_destroy_mr(struct rds_mr *mr)
{
	struct rds_sock *rs = mr->r_sock;
	void *trans_private = NULL;
	unsigned long flags;

	rdsdebug("RDS: destroy mr key is %x refcnt %u\n",
			mr->r_key, refcount_read(&mr->r_refcount));

	if (test_and_set_bit(RDS_MR_DEAD, &mr->r_state))
		return;

	spin_lock_irqsave(&rs->rs_rdma_lock, flags);
	if (!RB_EMPTY_NODE(&mr->r_rb_node))
		rb_erase(&mr->r_rb_node, &rs->rs_rdma_keys);
	trans_private = mr->r_trans_private;
	mr->r_trans_private = NULL;
	spin_unlock_irqrestore(&rs->rs_rdma_lock, flags);

	if (trans_private)
		mr->r_trans->free_mr(trans_private, mr->r_invalidate);
}