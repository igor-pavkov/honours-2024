static void tcp_rcv_nxt_update(struct tcp_sock *tp, u32 seq)
{
	u32 delta = seq - tp->rcv_nxt;

	sock_owned_by_me((struct sock *)tp);
	u64_stats_update_begin_raw(&tp->syncp);
	tp->bytes_received += delta;
	u64_stats_update_end_raw(&tp->syncp);
	tp->rcv_nxt = seq;
}