static int raw_sendmsg(struct sock *sk, struct msghdr *msg, size_t size)
{
	struct net_device *dev;
	unsigned int mtu;
	struct sk_buff *skb;
	int hlen, tlen;
	int err;

	if (msg->msg_flags & MSG_OOB) {
		pr_debug("msg->msg_flags = 0x%x\n", msg->msg_flags);
		return -EOPNOTSUPP;
	}

	lock_sock(sk);
	if (!sk->sk_bound_dev_if)
		dev = dev_getfirstbyhwtype(sock_net(sk), ARPHRD_IEEE802154);
	else
		dev = dev_get_by_index(sock_net(sk), sk->sk_bound_dev_if);
	release_sock(sk);

	if (!dev) {
		pr_debug("no dev\n");
		err = -ENXIO;
		goto out;
	}

	mtu = IEEE802154_MTU;
	pr_debug("name = %s, mtu = %u\n", dev->name, mtu);

	if (size > mtu) {
		pr_debug("size = %zu, mtu = %u\n", size, mtu);
		err = -EMSGSIZE;
		goto out_dev;
	}

	hlen = LL_RESERVED_SPACE(dev);
	tlen = dev->needed_tailroom;
	skb = sock_alloc_send_skb(sk, hlen + tlen + size,
				  msg->msg_flags & MSG_DONTWAIT, &err);
	if (!skb)
		goto out_dev;

	skb_reserve(skb, hlen);

	skb_reset_mac_header(skb);
	skb_reset_network_header(skb);

	err = memcpy_from_msg(skb_put(skb, size), msg, size);
	if (err < 0)
		goto out_skb;

	skb->dev = dev;
	skb->protocol = htons(ETH_P_IEEE802154);

	err = dev_queue_xmit(skb);
	if (err > 0)
		err = net_xmit_errno(err);

	dev_put(dev);

	return err ?: size;

out_skb:
	kfree_skb(skb);
out_dev:
	dev_put(dev);
out:
	return err;
}