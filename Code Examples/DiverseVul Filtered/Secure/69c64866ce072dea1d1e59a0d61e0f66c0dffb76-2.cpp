int dccp_disconnect(struct sock *sk, int flags)
{
	struct inet_connection_sock *icsk = inet_csk(sk);
	struct inet_sock *inet = inet_sk(sk);
	struct dccp_sock *dp = dccp_sk(sk);
	int err = 0;
	const int old_state = sk->sk_state;

	if (old_state != DCCP_CLOSED)
		dccp_set_state(sk, DCCP_CLOSED);

	/*
	 * This corresponds to the ABORT function of RFC793, sec. 3.8
	 * TCP uses a RST segment, DCCP a Reset packet with Code 2, "Aborted".
	 */
	if (old_state == DCCP_LISTEN) {
		inet_csk_listen_stop(sk);
	} else if (dccp_need_reset(old_state)) {
		dccp_send_reset(sk, DCCP_RESET_CODE_ABORTED);
		sk->sk_err = ECONNRESET;
	} else if (old_state == DCCP_REQUESTING)
		sk->sk_err = ECONNRESET;

	dccp_clear_xmit_timers(sk);
	ccid_hc_rx_delete(dp->dccps_hc_rx_ccid, sk);
	ccid_hc_tx_delete(dp->dccps_hc_tx_ccid, sk);
	dp->dccps_hc_rx_ccid = NULL;
	dp->dccps_hc_tx_ccid = NULL;

	__skb_queue_purge(&sk->sk_receive_queue);
	__skb_queue_purge(&sk->sk_write_queue);
	if (sk->sk_send_head != NULL) {
		__kfree_skb(sk->sk_send_head);
		sk->sk_send_head = NULL;
	}

	inet->inet_dport = 0;

	if (!(sk->sk_userlocks & SOCK_BINDADDR_LOCK))
		inet_reset_saddr(sk);

	sk->sk_shutdown = 0;
	sock_reset_flag(sk, SOCK_DONE);

	icsk->icsk_backoff = 0;
	inet_csk_delack_init(sk);
	__sk_dst_reset(sk);

	WARN_ON(inet->inet_num && !icsk->icsk_bind_hash);

	sk->sk_error_report(sk);
	return err;
}