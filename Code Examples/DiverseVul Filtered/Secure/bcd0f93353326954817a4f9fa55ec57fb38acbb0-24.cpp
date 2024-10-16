static int pep_getsockopt(struct sock *sk, int level, int optname,
				char __user *optval, int __user *optlen)
{
	struct pep_sock *pn = pep_sk(sk);
	int len, val;

	if (level != SOL_PNPIPE)
		return -ENOPROTOOPT;
	if (get_user(len, optlen))
		return -EFAULT;

	switch (optname) {
	case PNPIPE_ENCAP:
		val = pn->ifindex ? PNPIPE_ENCAP_IP : PNPIPE_ENCAP_NONE;
		break;

	case PNPIPE_IFINDEX:
		val = pn->ifindex;
		break;

	case PNPIPE_HANDLE:
		val = pn->pipe_handle;
		if (val == PN_PIPE_INVALID_HANDLE)
			return -EINVAL;
		break;

	case PNPIPE_INITSTATE:
		val = pn->init_enable;
		break;

	default:
		return -ENOPROTOOPT;
	}

	len = min_t(unsigned int, sizeof(int), len);
	if (put_user(len, optlen))
		return -EFAULT;
	if (put_user(val, (int __user *) optval))
		return -EFAULT;
	return 0;
}