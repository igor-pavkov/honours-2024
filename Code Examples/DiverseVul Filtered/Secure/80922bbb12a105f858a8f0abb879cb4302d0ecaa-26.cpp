static int econet_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	struct sock *sk = sock->sk;
	void __user *argp = (void __user *)arg;

	switch(cmd) {
		case SIOCGSTAMP:
			return sock_get_timestamp(sk, argp);

		case SIOCGSTAMPNS:
			return sock_get_timestampns(sk, argp);

		case SIOCSIFADDR:
		case SIOCGIFADDR:
			return ec_dev_ioctl(sock, cmd, argp);
			break;

		default:
			return -ENOIOCTLCMD;
	}
	/*NOTREACHED*/
	return 0;
}