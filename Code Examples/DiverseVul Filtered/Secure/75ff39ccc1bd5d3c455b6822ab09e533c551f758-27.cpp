static void tcp_add_reno_sack(struct sock *sk)
{
	struct tcp_sock *tp = tcp_sk(sk);
	u32 prior_sacked = tp->sacked_out;

	tp->sacked_out++;
	tcp_check_reno_reordering(sk, 0);
	if (tp->sacked_out > prior_sacked)
		tp->delivered++; /* Some out-of-order packet is delivered */
	tcp_verify_left_out(tp);
}