static int pep_setsockopt(struct sock *sk, int level, int optname,
			  sockptr_t optval, unsigned int optlen)
{
	struct pep_sock *pn = pep_sk(sk);
	int val = 0, err = 0;

	if (level != SOL_PNPIPE)
		return -ENOPROTOOPT;
	if (optlen >= sizeof(int)) {
		if (copy_from_sockptr(&val, optval, sizeof(int)))
			return -EFAULT;
	}

	lock_sock(sk);
	switch (optname) {
	case PNPIPE_ENCAP:
		if (val && val != PNPIPE_ENCAP_IP) {
			err = -EINVAL;
			break;
		}
		if (!pn->ifindex == !val)
			break; /* Nothing to do! */
		if (!capable(CAP_NET_ADMIN)) {
			err = -EPERM;
			break;
		}
		if (val) {
			release_sock(sk);
			err = gprs_attach(sk);
			if (err > 0) {
				pn->ifindex = err;
				err = 0;
			}
		} else {
			pn->ifindex = 0;
			release_sock(sk);
			gprs_detach(sk);
			err = 0;
		}
		goto out_norel;

	case PNPIPE_HANDLE:
		if ((sk->sk_state == TCP_CLOSE) &&
			(val >= 0) && (val < PN_PIPE_INVALID_HANDLE))
			pn->pipe_handle = val;
		else
			err = -EINVAL;
		break;

	case PNPIPE_INITSTATE:
		pn->init_enable = !!val;
		break;

	default:
		err = -ENOPROTOOPT;
	}
	release_sock(sk);

out_norel:
	return err;
}