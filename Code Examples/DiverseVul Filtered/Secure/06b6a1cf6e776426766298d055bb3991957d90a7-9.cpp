static int rds_cmsg_recv(struct rds_incoming *inc, struct msghdr *msg)
{
	int ret = 0;

	if (inc->i_rdma_cookie) {
		ret = put_cmsg(msg, SOL_RDS, RDS_CMSG_RDMA_DEST,
				sizeof(inc->i_rdma_cookie), &inc->i_rdma_cookie);
		if (ret)
			return ret;
	}

	return 0;
}