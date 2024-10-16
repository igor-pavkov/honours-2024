static int raw_bind(struct sock *sk, struct sockaddr *_uaddr, int len)
{
	struct ieee802154_addr addr;
	struct sockaddr_ieee802154 *uaddr = (struct sockaddr_ieee802154 *)_uaddr;
	int err = 0;
	struct net_device *dev = NULL;

	if (len < sizeof(*uaddr))
		return -EINVAL;

	uaddr = (struct sockaddr_ieee802154 *)_uaddr;
	if (uaddr->family != AF_IEEE802154)
		return -EINVAL;

	lock_sock(sk);

	ieee802154_addr_from_sa(&addr, &uaddr->addr);
	dev = ieee802154_get_dev(sock_net(sk), &addr);
	if (!dev) {
		err = -ENODEV;
		goto out;
	}

	sk->sk_bound_dev_if = dev->ifindex;
	sk_dst_reset(sk);

	dev_put(dev);
out:
	release_sock(sk);

	return err;
}