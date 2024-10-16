static int ieee802154_sock_ioctl(struct socket *sock, unsigned int cmd,
				 unsigned long arg)
{
	struct sock *sk = sock->sk;

	switch (cmd) {
	case SIOCGIFADDR:
	case SIOCSIFADDR:
		return ieee802154_dev_ioctl(sk, (struct ifreq __user *)arg,
				cmd);
	default:
		if (!sk->sk_prot->ioctl)
			return -ENOIOCTLCMD;
		return sk->sk_prot->ioctl(sk, cmd, arg);
	}
}