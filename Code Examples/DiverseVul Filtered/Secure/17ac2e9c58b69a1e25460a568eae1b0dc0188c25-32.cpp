static int rose_setsockopt(struct socket *sock, int level, int optname,
	char __user *optval, int optlen)
{
	struct sock *sk = sock->sk;
	struct rose_sock *rose = rose_sk(sk);
	int opt;

	if (level != SOL_ROSE)
		return -ENOPROTOOPT;

	if (optlen < sizeof(int))
		return -EINVAL;

	if (get_user(opt, (int __user *)optval))
		return -EFAULT;

	switch (optname) {
	case ROSE_DEFER:
		rose->defer = opt ? 1 : 0;
		return 0;

	case ROSE_T1:
		if (opt < 1)
			return -EINVAL;
		rose->t1 = opt * HZ;
		return 0;

	case ROSE_T2:
		if (opt < 1)
			return -EINVAL;
		rose->t2 = opt * HZ;
		return 0;

	case ROSE_T3:
		if (opt < 1)
			return -EINVAL;
		rose->t3 = opt * HZ;
		return 0;

	case ROSE_HOLDBACK:
		if (opt < 1)
			return -EINVAL;
		rose->hb = opt * HZ;
		return 0;

	case ROSE_IDLE:
		if (opt < 0)
			return -EINVAL;
		rose->idle = opt * 60 * HZ;
		return 0;

	case ROSE_QBITINCL:
		rose->qbitincl = opt ? 1 : 0;
		return 0;

	default:
		return -ENOPROTOOPT;
	}
}