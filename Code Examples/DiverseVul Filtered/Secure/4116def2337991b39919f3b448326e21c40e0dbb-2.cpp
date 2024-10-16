static int rds_cmsg_recv(struct rds_incoming *inc, struct msghdr *msg,
			 struct rds_sock *rs)
{
	int ret = 0;

	if (inc->i_rdma_cookie) {
		ret = put_cmsg(msg, SOL_RDS, RDS_CMSG_RDMA_DEST,
				sizeof(inc->i_rdma_cookie), &inc->i_rdma_cookie);
		if (ret)
			return ret;
	}

	if ((inc->i_rx_tstamp.tv_sec != 0) &&
	    sock_flag(rds_rs_to_sk(rs), SOCK_RCVTSTAMP)) {
		ret = put_cmsg(msg, SOL_SOCKET, SCM_TIMESTAMP,
			       sizeof(struct timeval),
			       &inc->i_rx_tstamp);
		if (ret)
			return ret;
	}

	return 0;
}