static void tcp_ecn_openreq_child(struct tcp_sock *tp,
				  const struct request_sock *req)
{
	tp->ecn_flags = inet_rsk(req)->ecn_ok ? TCP_ECN_OK : 0;
}