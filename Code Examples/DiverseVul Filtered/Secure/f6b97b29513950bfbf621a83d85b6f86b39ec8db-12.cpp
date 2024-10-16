static int nr_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	struct sock *sk = sock->sk;
	void __user *argp = (void __user *)arg;
	int ret;

	switch (cmd) {
	case TIOCOUTQ: {
		long amount;

		lock_sock(sk);
		amount = sk->sk_sndbuf - sk_wmem_alloc_get(sk);
		if (amount < 0)
			amount = 0;
		release_sock(sk);
		return put_user(amount, (int __user *)argp);
	}

	case TIOCINQ: {
		struct sk_buff *skb;
		long amount = 0L;

		lock_sock(sk);
		/* These two are safe on a single CPU system as only user tasks fiddle here */
		if ((skb = skb_peek(&sk->sk_receive_queue)) != NULL)
			amount = skb->len;
		release_sock(sk);
		return put_user(amount, (int __user *)argp);
	}

	case SIOCGSTAMP:
		lock_sock(sk);
		ret = sock_get_timestamp(sk, argp);
		release_sock(sk);
		return ret;

	case SIOCGSTAMPNS:
		lock_sock(sk);
		ret = sock_get_timestampns(sk, argp);
		release_sock(sk);
		return ret;

	case SIOCGIFADDR:
	case SIOCSIFADDR:
	case SIOCGIFDSTADDR:
	case SIOCSIFDSTADDR:
	case SIOCGIFBRDADDR:
	case SIOCSIFBRDADDR:
	case SIOCGIFNETMASK:
	case SIOCSIFNETMASK:
	case SIOCGIFMETRIC:
	case SIOCSIFMETRIC:
		return -EINVAL;

	case SIOCADDRT:
	case SIOCDELRT:
	case SIOCNRDECOBS:
		if (!capable(CAP_NET_ADMIN)) return -EPERM;
		return nr_rt_ioctl(cmd, argp);

	default:
		return -ENOIOCTLCMD;
	}

	return 0;
}