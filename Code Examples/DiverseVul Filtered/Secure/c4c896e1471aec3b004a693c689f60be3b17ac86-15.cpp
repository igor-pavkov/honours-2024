static void sco_conn_ready(struct sco_conn *conn)
{
	struct sock *parent;
	struct sock *sk = conn->sk;

	BT_DBG("conn %p", conn);

	sco_conn_lock(conn);

	if (sk) {
		sco_sock_clear_timer(sk);
		bh_lock_sock(sk);
		sk->sk_state = BT_CONNECTED;
		sk->sk_state_change(sk);
		bh_unlock_sock(sk);
	} else {
		parent = sco_get_sock_listen(conn->src);
		if (!parent)
			goto done;

		bh_lock_sock(parent);

		sk = sco_sock_alloc(sock_net(parent), NULL,
				BTPROTO_SCO, GFP_ATOMIC);
		if (!sk) {
			bh_unlock_sock(parent);
			goto done;
		}

		sco_sock_init(sk, parent);

		bacpy(&bt_sk(sk)->src, conn->src);
		bacpy(&bt_sk(sk)->dst, conn->dst);

		hci_conn_hold(conn->hcon);
		__sco_chan_add(conn, sk, parent);

		sk->sk_state = BT_CONNECTED;

		/* Wake up parent */
		parent->sk_data_ready(parent, 1);

		bh_unlock_sock(parent);
	}

done:
	sco_conn_unlock(conn);
}