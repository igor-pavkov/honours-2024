void rds_inc_init(struct rds_incoming *inc, struct rds_connection *conn,
		  __be32 saddr)
{
	atomic_set(&inc->i_refcount, 1);
	INIT_LIST_HEAD(&inc->i_item);
	inc->i_conn = conn;
	inc->i_saddr = saddr;
	inc->i_rdma_cookie = 0;
	inc->i_rx_tstamp.tv_sec = 0;
	inc->i_rx_tstamp.tv_usec = 0;
}