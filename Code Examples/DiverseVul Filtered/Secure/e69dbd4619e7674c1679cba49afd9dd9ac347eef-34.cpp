static int dgram_sendmsg(struct sock *sk, struct msghdr *msg, size_t size)
{
	struct net_device *dev;
	unsigned int mtu;
	struct sk_buff *skb;
	struct ieee802154_mac_cb *cb;
	struct dgram_sock *ro = dgram_sk(sk);
	struct ieee802154_addr dst_addr;
	int hlen, tlen;
	int err;

	if (msg->msg_flags & MSG_OOB) {
		pr_debug("msg->msg_flags = 0x%x\n", msg->msg_flags);
		return -EOPNOTSUPP;
	}

	if (!ro->connected && !msg->msg_name)
		return -EDESTADDRREQ;
	else if (ro->connected && msg->msg_name)
		return -EISCONN;

	if (!ro->bound)
		dev = dev_getfirstbyhwtype(sock_net(sk), ARPHRD_IEEE802154);
	else
		dev = ieee802154_get_dev(sock_net(sk), &ro->src_addr);

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
				  msg->msg_flags & MSG_DONTWAIT,
				  &err);
	if (!skb)
		goto out_dev;

	skb_reserve(skb, hlen);

	skb_reset_network_header(skb);

	cb = mac_cb_init(skb);
	cb->type = IEEE802154_FC_TYPE_DATA;
	cb->ackreq = ro->want_ack;

	if (msg->msg_name) {
		DECLARE_SOCKADDR(struct sockaddr_ieee802154*,
				 daddr, msg->msg_name);

		ieee802154_addr_from_sa(&dst_addr, &daddr->addr);
	} else {
		dst_addr = ro->dst_addr;
	}

	cb->secen = ro->secen;
	cb->secen_override = ro->secen_override;
	cb->seclevel = ro->seclevel;
	cb->seclevel_override = ro->seclevel_override;

	err = wpan_dev_hard_header(skb, dev, &dst_addr,
				   ro->bound ? &ro->src_addr : NULL, size);
	if (err < 0)
		goto out_skb;

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