static int raw_getsockopt(struct socket *sock, int level, int optname,
			  char __user *optval, int __user *optlen)
{
	struct sock *sk = sock->sk;
	struct raw_sock *ro = raw_sk(sk);
	int len;
	void *val;
	int err = 0;

	if (level != SOL_CAN_RAW)
		return -EINVAL;
	if (get_user(len, optlen))
		return -EFAULT;
	if (len < 0)
		return -EINVAL;

	switch (optname) {

	case CAN_RAW_FILTER:
		lock_sock(sk);
		if (ro->count > 0) {
			int fsize = ro->count * sizeof(struct can_filter);
			if (len > fsize)
				len = fsize;
			if (copy_to_user(optval, ro->filter, len))
				err = -EFAULT;
		} else
			len = 0;
		release_sock(sk);

		if (!err)
			err = put_user(len, optlen);
		return err;

	case CAN_RAW_ERR_FILTER:
		if (len > sizeof(can_err_mask_t))
			len = sizeof(can_err_mask_t);
		val = &ro->err_mask;
		break;

	case CAN_RAW_LOOPBACK:
		if (len > sizeof(int))
			len = sizeof(int);
		val = &ro->loopback;
		break;

	case CAN_RAW_RECV_OWN_MSGS:
		if (len > sizeof(int))
			len = sizeof(int);
		val = &ro->recv_own_msgs;
		break;

	default:
		return -ENOPROTOOPT;
	}

	if (put_user(len, optlen))
		return -EFAULT;
	if (copy_to_user(optval, val, len))
		return -EFAULT;
	return 0;
}