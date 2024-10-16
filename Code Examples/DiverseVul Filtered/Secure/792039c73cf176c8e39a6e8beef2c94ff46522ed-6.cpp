static int l2cap_sock_recvmsg(struct kiocb *iocb, struct socket *sock, struct msghdr *msg, size_t len, int flags)
{
	struct sock *sk = sock->sk;
	struct l2cap_pinfo *pi = l2cap_pi(sk);
	int err;

	lock_sock(sk);

	if (sk->sk_state == BT_CONNECT2 && test_bit(BT_SK_DEFER_SETUP,
						    &bt_sk(sk)->flags)) {
		sk->sk_state = BT_CONFIG;
		pi->chan->state = BT_CONFIG;

		__l2cap_connect_rsp_defer(pi->chan);
		release_sock(sk);
		return 0;
	}

	release_sock(sk);

	if (sock->type == SOCK_STREAM)
		err = bt_sock_stream_recvmsg(iocb, sock, msg, len, flags);
	else
		err = bt_sock_recvmsg(iocb, sock, msg, len, flags);

	if (pi->chan->mode != L2CAP_MODE_ERTM)
		return err;

	/* Attempt to put pending rx data in the socket buffer */

	lock_sock(sk);

	if (!test_bit(CONN_LOCAL_BUSY, &pi->chan->conn_state))
		goto done;

	if (pi->rx_busy_skb) {
		if (!sock_queue_rcv_skb(sk, pi->rx_busy_skb))
			pi->rx_busy_skb = NULL;
		else
			goto done;
	}

	/* Restore data flow when half of the receive buffer is
	 * available.  This avoids resending large numbers of
	 * frames.
	 */
	if (atomic_read(&sk->sk_rmem_alloc) <= sk->sk_rcvbuf >> 1)
		l2cap_chan_busy(pi->chan, 0);

done:
	release_sock(sk);
	return err;
}