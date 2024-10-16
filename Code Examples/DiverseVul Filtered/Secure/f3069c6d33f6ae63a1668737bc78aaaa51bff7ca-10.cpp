int rds_free_mr(struct rds_sock *rs, char __user *optval, int optlen)
{
	struct rds_free_mr_args args;
	struct rds_mr *mr;
	unsigned long flags;

	if (optlen != sizeof(struct rds_free_mr_args))
		return -EINVAL;

	if (copy_from_user(&args, (struct rds_free_mr_args __user *)optval,
			   sizeof(struct rds_free_mr_args)))
		return -EFAULT;

	/* Special case - a null cookie means flush all unused MRs */
	if (args.cookie == 0) {
		if (!rs->rs_transport || !rs->rs_transport->flush_mrs)
			return -EINVAL;
		rs->rs_transport->flush_mrs();
		return 0;
	}

	/* Look up the MR given its R_key and remove it from the rbtree
	 * so nobody else finds it.
	 * This should also prevent races with rds_rdma_unuse.
	 */
	spin_lock_irqsave(&rs->rs_rdma_lock, flags);
	mr = rds_mr_tree_walk(&rs->rs_rdma_keys, rds_rdma_cookie_key(args.cookie), NULL);
	if (mr) {
		rb_erase(&mr->r_rb_node, &rs->rs_rdma_keys);
		RB_CLEAR_NODE(&mr->r_rb_node);
		if (args.flags & RDS_RDMA_INVALIDATE)
			mr->r_invalidate = 1;
	}
	spin_unlock_irqrestore(&rs->rs_rdma_lock, flags);

	if (!mr)
		return -EINVAL;

	/*
	 * call rds_destroy_mr() ourselves so that we're sure it's done by the time
	 * we return.  If we let rds_mr_put() do it it might not happen until
	 * someone else drops their ref.
	 */
	rds_destroy_mr(mr);
	rds_mr_put(mr);
	return 0;
}