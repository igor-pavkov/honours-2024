static int l2cap_sock_connect(struct socket *sock, struct sockaddr *addr, int alen, int flags)
{
	struct sock *sk = sock->sk;
	struct l2cap_chan *chan = l2cap_pi(sk)->chan;
	struct sockaddr_l2 la;
	int len, err = 0;

	BT_DBG("sk %p", sk);

	if (!addr || alen < sizeof(addr->sa_family) ||
	    addr->sa_family != AF_BLUETOOTH)
		return -EINVAL;

	memset(&la, 0, sizeof(la));
	len = min_t(unsigned int, sizeof(la), alen);
	memcpy(&la, addr, len);

	if (la.l2_cid && la.l2_psm)
		return -EINVAL;

	err = l2cap_chan_connect(chan, la.l2_psm, __le16_to_cpu(la.l2_cid),
				 &la.l2_bdaddr, la.l2_bdaddr_type);
	if (err)
		return err;

	lock_sock(sk);

	err = bt_sock_wait_state(sk, BT_CONNECTED,
			sock_sndtimeo(sk, flags & O_NONBLOCK));

	release_sock(sk);

	return err;
}