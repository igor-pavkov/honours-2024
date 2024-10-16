static int rose_getsockopt(struct socket *sock, int level, int optname,
	char __user *optval, int __user *optlen)
{
	struct sock *sk = sock->sk;
	struct rose_sock *rose = rose_sk(sk);
	int val = 0;
	int len;

	if (level != SOL_ROSE)
		return -ENOPROTOOPT;

	if (get_user(len, optlen))
		return -EFAULT;

	if (len < 0)
		return -EINVAL;

	switch (optname) {
	case ROSE_DEFER:
		val = rose->defer;
		break;

	case ROSE_T1:
		val = rose->t1 / HZ;
		break;

	case ROSE_T2:
		val = rose->t2 / HZ;
		break;

	case ROSE_T3:
		val = rose->t3 / HZ;
		break;

	case ROSE_HOLDBACK:
		val = rose->hb / HZ;
		break;

	case ROSE_IDLE:
		val = rose->idle / (60 * HZ);
		break;

	case ROSE_QBITINCL:
		val = rose->qbitincl;
		break;

	default:
		return -ENOPROTOOPT;
	}

	len = min_t(unsigned int, len, sizeof(int));

	if (put_user(len, optlen))
		return -EFAULT;

	return copy_to_user(optval, &val, len) ? -EFAULT : 0;
}