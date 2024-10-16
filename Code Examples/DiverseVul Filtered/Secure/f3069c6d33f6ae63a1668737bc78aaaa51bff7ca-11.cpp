int rds_cmsg_rdma_map(struct rds_sock *rs, struct rds_message *rm,
			  struct cmsghdr *cmsg)
{
	if (cmsg->cmsg_len < CMSG_LEN(sizeof(struct rds_get_mr_args)) ||
	    rm->m_rdma_cookie != 0)
		return -EINVAL;

	return __rds_rdma_map(rs, CMSG_DATA(cmsg), &rm->m_rdma_cookie, &rm->rdma.op_rdma_mr);
}