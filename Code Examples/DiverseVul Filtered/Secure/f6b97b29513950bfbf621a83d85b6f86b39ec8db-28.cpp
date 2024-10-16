static int nr_setsockopt(struct socket *sock, int level, int optname,
	char __user *optval, int optlen)
{
	struct sock *sk = sock->sk;
	struct nr_sock *nr = nr_sk(sk);
	int opt;

	if (level != SOL_NETROM)
		return -ENOPROTOOPT;

	if (optlen < sizeof(int))
		return -EINVAL;

	if (get_user(opt, (int __user *)optval))
		return -EFAULT;

	switch (optname) {
	case NETROM_T1:
		if (opt < 1)
			return -EINVAL;
		nr->t1 = opt * HZ;
		return 0;

	case NETROM_T2:
		if (opt < 1)
			return -EINVAL;
		nr->t2 = opt * HZ;
		return 0;

	case NETROM_N2:
		if (opt < 1 || opt > 31)
			return -EINVAL;
		nr->n2 = opt;
		return 0;

	case NETROM_T4:
		if (opt < 1)
			return -EINVAL;
		nr->t4 = opt * HZ;
		return 0;

	case NETROM_IDLE:
		if (opt < 0)
			return -EINVAL;
		nr->idle = opt * 60 * HZ;
		return 0;

	default:
		return -ENOPROTOOPT;
	}
}