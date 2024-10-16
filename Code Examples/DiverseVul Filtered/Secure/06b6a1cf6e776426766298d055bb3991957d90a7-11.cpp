void rds_clear_recv_queue(struct rds_sock *rs)
{
	struct sock *sk = rds_rs_to_sk(rs);
	struct rds_incoming *inc, *tmp;
	unsigned long flags;

	write_lock_irqsave(&rs->rs_recv_lock, flags);
	list_for_each_entry_safe(inc, tmp, &rs->rs_recv_queue, i_item) {
		rds_recv_rcvbuf_delta(rs, sk, inc->i_conn->c_lcong,
				      -be32_to_cpu(inc->i_hdr.h_len),
				      inc->i_hdr.h_dport);
		list_del_init(&inc->i_item);
		rds_inc_put(inc);
	}
	write_unlock_irqrestore(&rs->rs_recv_lock, flags);
}