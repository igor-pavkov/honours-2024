static void tcp_snd_una_update(struct tcp_sock *tp, u32 ack)
{
	u32 delta = ack - tp->snd_una;

	sock_owned_by_me((struct sock *)tp);
	u64_stats_update_begin_raw(&tp->syncp);
	tp->bytes_acked += delta;
	u64_stats_update_end_raw(&tp->syncp);
	tp->snd_una = ack;
}