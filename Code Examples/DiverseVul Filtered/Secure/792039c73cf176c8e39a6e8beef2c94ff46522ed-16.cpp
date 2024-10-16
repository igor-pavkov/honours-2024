static int l2cap_sock_bind(struct socket *sock, struct sockaddr *addr, int alen)
{
	struct sock *sk = sock->sk;
	struct l2cap_chan *chan = l2cap_pi(sk)->chan;
	struct sockaddr_l2 la;
	int len, err = 0;

	BT_DBG("sk %p", sk);

	if (!addr || addr->sa_family != AF_BLUETOOTH)
		return -EINVAL;

	memset(&la, 0, sizeof(la));
	len = min_t(unsigned int, sizeof(la), alen);
	memcpy(&la, addr, len);

	if (la.l2_cid && la.l2_psm)
		return -EINVAL;

	lock_sock(sk);

	if (sk->sk_state != BT_OPEN) {
		err = -EBADFD;
		goto done;
	}

	if (la.l2_psm) {
		__u16 psm = __le16_to_cpu(la.l2_psm);

		/* PSM must be odd and lsb of upper byte must be 0 */
		if ((psm & 0x0101) != 0x0001) {
			err = -EINVAL;
			goto done;
		}

		/* Restrict usage of well-known PSMs */
		if (psm < 0x1001 && !capable(CAP_NET_BIND_SERVICE)) {
			err = -EACCES;
			goto done;
		}
	}

	if (la.l2_cid)
		err = l2cap_add_scid(chan, __le16_to_cpu(la.l2_cid));
	else
		err = l2cap_add_psm(chan, &la.l2_bdaddr, la.l2_psm);

	if (err < 0)
		goto done;

	if (__le16_to_cpu(la.l2_psm) == L2CAP_PSM_SDP ||
	    __le16_to_cpu(la.l2_psm) == L2CAP_PSM_RFCOMM)
		chan->sec_level = BT_SECURITY_SDP;

	bacpy(&bt_sk(sk)->src, &la.l2_bdaddr);

	chan->state = BT_BOUND;
	sk->sk_state = BT_BOUND;

done:
	release_sock(sk);
	return err;
}