void dccp_close(struct sock *sk, long timeout)
{
	struct dccp_sock *dp = dccp_sk(sk);
	struct sk_buff *skb;
	u32 data_was_unread = 0;
	int state;

	lock_sock(sk);

	sk->sk_shutdown = SHUTDOWN_MASK;

	if (sk->sk_state == DCCP_LISTEN) {
		dccp_set_state(sk, DCCP_CLOSED);

		/* Special case. */
		inet_csk_listen_stop(sk);

		goto adjudge_to_death;
	}

	sk_stop_timer(sk, &dp->dccps_xmit_timer);

	/*
	 * We need to flush the recv. buffs.  We do this only on the
	 * descriptor close, not protocol-sourced closes, because the
	  *reader process may not have drained the data yet!
	 */
	while ((skb = __skb_dequeue(&sk->sk_receive_queue)) != NULL) {
		data_was_unread += skb->len;
		__kfree_skb(skb);
	}

	if (data_was_unread) {
		/* Unread data was tossed, send an appropriate Reset Code */
		DCCP_WARN("DCCP: ABORT -- %u bytes unread\n", data_was_unread);
		dccp_send_reset(sk, DCCP_RESET_CODE_ABORTED);
		dccp_set_state(sk, DCCP_CLOSED);
	} else if (sock_flag(sk, SOCK_LINGER) && !sk->sk_lingertime) {
		/* Check zero linger _after_ checking for unread data. */
		sk->sk_prot->disconnect(sk, 0);
	} else if (sk->sk_state != DCCP_CLOSED) {
		dccp_terminate_connection(sk);
	}

	sk_stream_wait_close(sk, timeout);

adjudge_to_death:
	state = sk->sk_state;
	sock_hold(sk);
	sock_orphan(sk);
	atomic_inc(sk->sk_prot->orphan_count);

	/*
	 * It is the last release_sock in its life. It will remove backlog.
	 */
	release_sock(sk);
	/*
	 * Now socket is owned by kernel and we acquire BH lock
	 * to finish close. No need to check for user refs.
	 */
	local_bh_disable();
	bh_lock_sock(sk);
	WARN_ON(sock_owned_by_user(sk));

	/* Have we already been destroyed by a softirq or backlog? */
	if (state != DCCP_CLOSED && sk->sk_state == DCCP_CLOSED)
		goto out;

	if (sk->sk_state == DCCP_CLOSED)
		inet_csk_destroy_sock(sk);

	/* Otherwise, socket is reprieved until protocol close. */

out:
	bh_unlock_sock(sk);
	local_bh_enable();
	sock_put(sk);
}