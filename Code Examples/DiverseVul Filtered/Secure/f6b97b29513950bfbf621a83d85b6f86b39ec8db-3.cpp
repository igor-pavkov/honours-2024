static int nr_getsockopt(struct socket *sock, int level, int optname,
	char __user *optval, int __user *optlen)
{
	struct sock *sk = sock->sk;
	struct nr_sock *nr = nr_sk(sk);
	int val = 0;
	int len;

	if (level != SOL_NETROM)
		return -ENOPROTOOPT;

	if (get_user(len, optlen))
		return -EFAULT;

	if (len < 0)
		return -EINVAL;

	switch (optname) {
	case NETROM_T1:
		val = nr->t1 / HZ;
		break;

	case NETROM_T2:
		val = nr->t2 / HZ;
		break;

	case NETROM_N2:
		val = nr->n2;
		break;

	case NETROM_T4:
		val = nr->t4 / HZ;
		break;

	case NETROM_IDLE:
		val = nr->idle / (60 * HZ);
		break;

	default:
		return -ENOPROTOOPT;
	}

	len = min_t(unsigned int, len, sizeof(int));

	if (put_user(len, optlen))
		return -EFAULT;

	return copy_to_user(optval, &val, len) ? -EFAULT : 0;
}