static int raw_sendmsg(struct kiocb *iocb, struct socket *sock,
		       struct msghdr *msg, size_t size)
{
	struct sock *sk = sock->sk;
	struct raw_sock *ro = raw_sk(sk);
	struct sk_buff *skb;
	struct net_device *dev;
	int ifindex;
	int err;

	if (msg->msg_name) {
		struct sockaddr_can *addr =
			(struct sockaddr_can *)msg->msg_name;

		if (addr->can_family != AF_CAN)
			return -EINVAL;

		ifindex = addr->can_ifindex;
	} else
		ifindex = ro->ifindex;

	if (size != sizeof(struct can_frame))
		return -EINVAL;

	dev = dev_get_by_index(&init_net, ifindex);
	if (!dev)
		return -ENXIO;

	skb = sock_alloc_send_skb(sk, size, msg->msg_flags & MSG_DONTWAIT,
				  &err);
	if (!skb)
		goto put_dev;

	err = memcpy_fromiovec(skb_put(skb, size), msg->msg_iov, size);
	if (err < 0)
		goto free_skb;
	err = sock_tx_timestamp(msg, sk, skb_tx(skb));
	if (err < 0)
		goto free_skb;
	skb->dev = dev;
	skb->sk  = sk;

	err = can_send(skb, ro->loopback);

	dev_put(dev);

	if (err)
		goto send_failed;

	return size;

free_skb:
	kfree_skb(skb);
put_dev:
	dev_put(dev);
send_failed:
	return err;
}