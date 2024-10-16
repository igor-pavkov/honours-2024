static int l2cap_sock_getsockopt(struct socket *sock, int level, int optname, char __user *optval, int __user *optlen)
{
	struct sock *sk = sock->sk;
	struct l2cap_chan *chan = l2cap_pi(sk)->chan;
	struct bt_security sec;
	struct bt_power pwr;
	int len, err = 0;

	BT_DBG("sk %p", sk);

	if (level == SOL_L2CAP)
		return l2cap_sock_getsockopt_old(sock, optname, optval, optlen);

	if (level != SOL_BLUETOOTH)
		return -ENOPROTOOPT;

	if (get_user(len, optlen))
		return -EFAULT;

	lock_sock(sk);

	switch (optname) {
	case BT_SECURITY:
		if (chan->chan_type != L2CAP_CHAN_CONN_ORIENTED &&
					chan->chan_type != L2CAP_CHAN_RAW) {
			err = -EINVAL;
			break;
		}

		memset(&sec, 0, sizeof(sec));
		if (chan->conn)
			sec.level = chan->conn->hcon->sec_level;
		else
			sec.level = chan->sec_level;

		if (sk->sk_state == BT_CONNECTED)
			sec.key_size = chan->conn->hcon->enc_key_size;

		len = min_t(unsigned int, len, sizeof(sec));
		if (copy_to_user(optval, (char *) &sec, len))
			err = -EFAULT;

		break;

	case BT_DEFER_SETUP:
		if (sk->sk_state != BT_BOUND && sk->sk_state != BT_LISTEN) {
			err = -EINVAL;
			break;
		}

		if (put_user(test_bit(BT_SK_DEFER_SETUP, &bt_sk(sk)->flags),
			     (u32 __user *) optval))
			err = -EFAULT;

		break;

	case BT_FLUSHABLE:
		if (put_user(test_bit(FLAG_FLUSHABLE, &chan->flags),
						(u32 __user *) optval))
			err = -EFAULT;

		break;

	case BT_POWER:
		if (sk->sk_type != SOCK_SEQPACKET && sk->sk_type != SOCK_STREAM
				&& sk->sk_type != SOCK_RAW) {
			err = -EINVAL;
			break;
		}

		pwr.force_active = test_bit(FLAG_FORCE_ACTIVE, &chan->flags);

		len = min_t(unsigned int, len, sizeof(pwr));
		if (copy_to_user(optval, (char *) &pwr, len))
			err = -EFAULT;

		break;

	case BT_CHANNEL_POLICY:
		if (!enable_hs) {
			err = -ENOPROTOOPT;
			break;
		}

		if (put_user(chan->chan_policy, (u32 __user *) optval))
			err = -EFAULT;
		break;

	default:
		err = -ENOPROTOOPT;
		break;
	}

	release_sock(sk);
	return err;
}