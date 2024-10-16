static int l2cap_sock_setsockopt_old(struct socket *sock, int optname, char __user *optval, unsigned int optlen)
{
	struct sock *sk = sock->sk;
	struct l2cap_chan *chan = l2cap_pi(sk)->chan;
	struct l2cap_options opts;
	int len, err = 0;
	u32 opt;

	BT_DBG("sk %p", sk);

	lock_sock(sk);

	switch (optname) {
	case L2CAP_OPTIONS:
		if (sk->sk_state == BT_CONNECTED) {
			err = -EINVAL;
			break;
		}

		opts.imtu     = chan->imtu;
		opts.omtu     = chan->omtu;
		opts.flush_to = chan->flush_to;
		opts.mode     = chan->mode;
		opts.fcs      = chan->fcs;
		opts.max_tx   = chan->max_tx;
		opts.txwin_size = chan->tx_win;

		len = min_t(unsigned int, sizeof(opts), optlen);
		if (copy_from_user((char *) &opts, optval, len)) {
			err = -EFAULT;
			break;
		}

		if (opts.txwin_size > L2CAP_DEFAULT_EXT_WINDOW) {
			err = -EINVAL;
			break;
		}

		if (!l2cap_valid_mtu(chan, opts.imtu)) {
			err = -EINVAL;
			break;
		}

		chan->mode = opts.mode;
		switch (chan->mode) {
		case L2CAP_MODE_BASIC:
			clear_bit(CONF_STATE2_DEVICE, &chan->conf_state);
			break;
		case L2CAP_MODE_ERTM:
		case L2CAP_MODE_STREAMING:
			if (!disable_ertm)
				break;
			/* fall through */
		default:
			err = -EINVAL;
			break;
		}

		chan->imtu = opts.imtu;
		chan->omtu = opts.omtu;
		chan->fcs  = opts.fcs;
		chan->max_tx = opts.max_tx;
		chan->tx_win = opts.txwin_size;
		break;

	case L2CAP_LM:
		if (get_user(opt, (u32 __user *) optval)) {
			err = -EFAULT;
			break;
		}

		if (opt & L2CAP_LM_AUTH)
			chan->sec_level = BT_SECURITY_LOW;
		if (opt & L2CAP_LM_ENCRYPT)
			chan->sec_level = BT_SECURITY_MEDIUM;
		if (opt & L2CAP_LM_SECURE)
			chan->sec_level = BT_SECURITY_HIGH;

		if (opt & L2CAP_LM_MASTER)
			set_bit(FLAG_ROLE_SWITCH, &chan->flags);
		else
			clear_bit(FLAG_ROLE_SWITCH, &chan->flags);

		if (opt & L2CAP_LM_RELIABLE)
			set_bit(FLAG_FORCE_RELIABLE, &chan->flags);
		else
			clear_bit(FLAG_FORCE_RELIABLE, &chan->flags);
		break;

	default:
		err = -ENOPROTOOPT;
		break;
	}

	release_sock(sk);
	return err;
}