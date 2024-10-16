static int l2cap_sock_setsockopt(struct socket *sock, int level, int optname, char __user *optval, unsigned int optlen)
{
	struct sock *sk = sock->sk;
	struct l2cap_chan *chan = l2cap_pi(sk)->chan;
	struct bt_security sec;
	struct bt_power pwr;
	struct l2cap_conn *conn;
	int len, err = 0;
	u32 opt;

	BT_DBG("sk %p", sk);

	if (level == SOL_L2CAP)
		return l2cap_sock_setsockopt_old(sock, optname, optval, optlen);

	if (level != SOL_BLUETOOTH)
		return -ENOPROTOOPT;

	lock_sock(sk);

	switch (optname) {
	case BT_SECURITY:
		if (chan->chan_type != L2CAP_CHAN_CONN_ORIENTED &&
					chan->chan_type != L2CAP_CHAN_RAW) {
			err = -EINVAL;
			break;
		}

		sec.level = BT_SECURITY_LOW;

		len = min_t(unsigned int, sizeof(sec), optlen);
		if (copy_from_user((char *) &sec, optval, len)) {
			err = -EFAULT;
			break;
		}

		if (sec.level < BT_SECURITY_LOW ||
					sec.level > BT_SECURITY_HIGH) {
			err = -EINVAL;
			break;
		}

		chan->sec_level = sec.level;

		if (!chan->conn)
			break;

		conn = chan->conn;

		/*change security for LE channels */
		if (chan->scid == L2CAP_CID_LE_DATA) {
			if (!conn->hcon->out) {
				err = -EINVAL;
				break;
			}

			if (smp_conn_security(conn, sec.level))
				break;
			sk->sk_state = BT_CONFIG;
			chan->state = BT_CONFIG;

		/* or for ACL link */
		} else if ((sk->sk_state == BT_CONNECT2 &&
			   test_bit(BT_SK_DEFER_SETUP, &bt_sk(sk)->flags)) ||
			   sk->sk_state == BT_CONNECTED) {
			if (!l2cap_chan_check_security(chan))
				set_bit(BT_SK_SUSPEND, &bt_sk(sk)->flags);
			else
				sk->sk_state_change(sk);
		} else {
			err = -EINVAL;
		}
		break;

	case BT_DEFER_SETUP:
		if (sk->sk_state != BT_BOUND && sk->sk_state != BT_LISTEN) {
			err = -EINVAL;
			break;
		}

		if (get_user(opt, (u32 __user *) optval)) {
			err = -EFAULT;
			break;
		}

		if (opt)
			set_bit(BT_SK_DEFER_SETUP, &bt_sk(sk)->flags);
		else
			clear_bit(BT_SK_DEFER_SETUP, &bt_sk(sk)->flags);
		break;

	case BT_FLUSHABLE:
		if (get_user(opt, (u32 __user *) optval)) {
			err = -EFAULT;
			break;
		}

		if (opt > BT_FLUSHABLE_ON) {
			err = -EINVAL;
			break;
		}

		if (opt == BT_FLUSHABLE_OFF) {
			struct l2cap_conn *conn = chan->conn;
			/* proceed further only when we have l2cap_conn and
			   No Flush support in the LM */
			if (!conn || !lmp_no_flush_capable(conn->hcon->hdev)) {
				err = -EINVAL;
				break;
			}
		}

		if (opt)
			set_bit(FLAG_FLUSHABLE, &chan->flags);
		else
			clear_bit(FLAG_FLUSHABLE, &chan->flags);
		break;

	case BT_POWER:
		if (chan->chan_type != L2CAP_CHAN_CONN_ORIENTED &&
					chan->chan_type != L2CAP_CHAN_RAW) {
			err = -EINVAL;
			break;
		}

		pwr.force_active = BT_POWER_FORCE_ACTIVE_ON;

		len = min_t(unsigned int, sizeof(pwr), optlen);
		if (copy_from_user((char *) &pwr, optval, len)) {
			err = -EFAULT;
			break;
		}

		if (pwr.force_active)
			set_bit(FLAG_FORCE_ACTIVE, &chan->flags);
		else
			clear_bit(FLAG_FORCE_ACTIVE, &chan->flags);
		break;

	case BT_CHANNEL_POLICY:
		if (!enable_hs) {
			err = -ENOPROTOOPT;
			break;
		}

		if (get_user(opt, (u32 __user *) optval)) {
			err = -EFAULT;
			break;
		}

		if (opt > BT_CHANNEL_POLICY_AMP_PREFERRED) {
			err = -EINVAL;
			break;
		}

		if (chan->mode != L2CAP_MODE_ERTM &&
				chan->mode != L2CAP_MODE_STREAMING) {
			err = -EOPNOTSUPP;
			break;
		}

		chan->chan_policy = (u8) opt;
		break;

	default:
		err = -ENOPROTOOPT;
		break;
	}

	release_sock(sk);
	return err;
}