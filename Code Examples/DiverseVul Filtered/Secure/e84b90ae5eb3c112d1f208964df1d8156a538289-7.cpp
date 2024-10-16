static int raw_setsockopt(struct socket *sock, int level, int optname,
			  char __user *optval, int optlen)
{
	struct sock *sk = sock->sk;
	struct raw_sock *ro = raw_sk(sk);
	struct can_filter *filter = NULL;  /* dyn. alloc'ed filters */
	struct can_filter sfilter;         /* single filter */
	struct net_device *dev = NULL;
	can_err_mask_t err_mask = 0;
	int count = 0;
	int err = 0;

	if (level != SOL_CAN_RAW)
		return -EINVAL;
	if (optlen < 0)
		return -EINVAL;

	switch (optname) {

	case CAN_RAW_FILTER:
		if (optlen % sizeof(struct can_filter) != 0)
			return -EINVAL;

		count = optlen / sizeof(struct can_filter);

		if (count > 1) {
			/* filter does not fit into dfilter => alloc space */
			filter = kmalloc(optlen, GFP_KERNEL);
			if (!filter)
				return -ENOMEM;

			if (copy_from_user(filter, optval, optlen)) {
				kfree(filter);
				return -EFAULT;
			}
		} else if (count == 1) {
			if (copy_from_user(&sfilter, optval, optlen))
				return -EFAULT;
		}

		lock_sock(sk);

		if (ro->bound && ro->ifindex)
			dev = dev_get_by_index(&init_net, ro->ifindex);

		if (ro->bound) {
			/* (try to) register the new filters */
			if (count == 1)
				err = raw_enable_filters(dev, sk, &sfilter, 1);
			else
				err = raw_enable_filters(dev, sk, filter,
							 count);
			if (err) {
				if (count > 1)
					kfree(filter);
				goto out_fil;
			}

			/* remove old filter registrations */
			raw_disable_filters(dev, sk, ro->filter, ro->count);
		}

		/* remove old filter space */
		if (ro->count > 1)
			kfree(ro->filter);

		/* link new filters to the socket */
		if (count == 1) {
			/* copy filter data for single filter */
			ro->dfilter = sfilter;
			filter = &ro->dfilter;
		}
		ro->filter = filter;
		ro->count  = count;

 out_fil:
		if (dev)
			dev_put(dev);

		release_sock(sk);

		break;

	case CAN_RAW_ERR_FILTER:
		if (optlen != sizeof(err_mask))
			return -EINVAL;

		if (copy_from_user(&err_mask, optval, optlen))
			return -EFAULT;

		err_mask &= CAN_ERR_MASK;

		lock_sock(sk);

		if (ro->bound && ro->ifindex)
			dev = dev_get_by_index(&init_net, ro->ifindex);

		/* remove current error mask */
		if (ro->bound) {
			/* (try to) register the new err_mask */
			err = raw_enable_errfilter(dev, sk, err_mask);

			if (err)
				goto out_err;

			/* remove old err_mask registration */
			raw_disable_errfilter(dev, sk, ro->err_mask);
		}

		/* link new err_mask to the socket */
		ro->err_mask = err_mask;

 out_err:
		if (dev)
			dev_put(dev);

		release_sock(sk);

		break;

	case CAN_RAW_LOOPBACK:
		if (optlen != sizeof(ro->loopback))
			return -EINVAL;

		if (copy_from_user(&ro->loopback, optval, optlen))
			return -EFAULT;

		break;

	case CAN_RAW_RECV_OWN_MSGS:
		if (optlen != sizeof(ro->recv_own_msgs))
			return -EINVAL;

		if (copy_from_user(&ro->recv_own_msgs, optval, optlen))
			return -EFAULT;

		break;

	default:
		return -ENOPROTOOPT;
	}
	return err;
}