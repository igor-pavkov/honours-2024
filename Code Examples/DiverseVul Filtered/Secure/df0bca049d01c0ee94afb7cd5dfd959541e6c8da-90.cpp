static int sock_bindtodevice(struct sock *sk, char __user *optval, int optlen)
{
	int ret = -ENOPROTOOPT;
#ifdef CONFIG_NETDEVICES
	struct net *net = sock_net(sk);
	char devname[IFNAMSIZ];
	int index;

	/* Sorry... */
	ret = -EPERM;
	if (!capable(CAP_NET_RAW))
		goto out;

	ret = -EINVAL;
	if (optlen < 0)
		goto out;

	/* Bind this socket to a particular device like "eth0",
	 * as specified in the passed interface name. If the
	 * name is "" or the option length is zero the socket
	 * is not bound.
	 */
	if (optlen > IFNAMSIZ - 1)
		optlen = IFNAMSIZ - 1;
	memset(devname, 0, sizeof(devname));

	ret = -EFAULT;
	if (copy_from_user(devname, optval, optlen))
		goto out;

	if (devname[0] == '\0') {
		index = 0;
	} else {
		struct net_device *dev = dev_get_by_name(net, devname);

		ret = -ENODEV;
		if (!dev)
			goto out;

		index = dev->ifindex;
		dev_put(dev);
	}

	lock_sock(sk);
	sk->sk_bound_dev_if = index;
	sk_dst_reset(sk);
	release_sock(sk);

	ret = 0;

out:
#endif

	return ret;
}