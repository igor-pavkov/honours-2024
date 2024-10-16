static struct l2cap_chan *l2cap_sock_new_connection_cb(struct l2cap_chan *chan)
{
	struct sock *sk, *parent = chan->data;

	/* Check for backlog size */
	if (sk_acceptq_is_full(parent)) {
		BT_DBG("backlog full %d", parent->sk_ack_backlog);
		return NULL;
	}

	sk = l2cap_sock_alloc(sock_net(parent), NULL, BTPROTO_L2CAP,
								GFP_ATOMIC);
	if (!sk)
		return NULL;

	bt_sock_reclassify_lock(sk, BTPROTO_L2CAP);

	l2cap_sock_init(sk, parent);

	return l2cap_pi(sk)->chan;
}