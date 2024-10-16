int rds_cmsg_rdma_dest(struct rds_sock *rs, struct rds_message *rm,
			  struct cmsghdr *cmsg)
{
	unsigned long flags;
	struct rds_mr *mr;
	u32 r_key;
	int err = 0;

	if (cmsg->cmsg_len < CMSG_LEN(sizeof(rds_rdma_cookie_t)) ||
	    rm->m_rdma_cookie != 0)
		return -EINVAL;

	memcpy(&rm->m_rdma_cookie, CMSG_DATA(cmsg), sizeof(rm->m_rdma_cookie));

	/* We are reusing a previously mapped MR here. Most likely, the
	 * application has written to the buffer, so we need to explicitly
	 * flush those writes to RAM. Otherwise the HCA may not see them
	 * when doing a DMA from that buffer.
	 */
	r_key = rds_rdma_cookie_key(rm->m_rdma_cookie);

	spin_lock_irqsave(&rs->rs_rdma_lock, flags);
	mr = rds_mr_tree_walk(&rs->rs_rdma_keys, r_key, NULL);
	if (!mr)
		err = -EINVAL;	/* invalid r_key */
	else
		refcount_inc(&mr->r_refcount);
	spin_unlock_irqrestore(&rs->rs_rdma_lock, flags);

	if (mr) {
		mr->r_trans->sync_mr(mr->r_trans_private, DMA_TO_DEVICE);
		rm->rdma.op_rdma_mr = mr;
	}
	return err;
}