static int ipip6_err(struct sk_buff *skb, u32 info)
{
#ifndef I_WISH_WORLD_WERE_PERFECT

/* It is not :-( All the routers (except for Linux) return only
   8 bytes of packet payload. It means, that precise relaying of
   ICMP in the real Internet is absolutely infeasible.
 */
	struct iphdr *iph = (struct iphdr*)skb->data;
	const int type = icmp_hdr(skb)->type;
	const int code = icmp_hdr(skb)->code;
	struct ip_tunnel *t;
	int err;

	switch (type) {
	default:
	case ICMP_PARAMETERPROB:
		return 0;

	case ICMP_DEST_UNREACH:
		switch (code) {
		case ICMP_SR_FAILED:
		case ICMP_PORT_UNREACH:
			/* Impossible event. */
			return 0;
		case ICMP_FRAG_NEEDED:
			/* Soft state for pmtu is maintained by IP core. */
			return 0;
		default:
			/* All others are translated to HOST_UNREACH.
			   rfc2003 contains "deep thoughts" about NET_UNREACH,
			   I believe they are just ether pollution. --ANK
			 */
			break;
		}
		break;
	case ICMP_TIME_EXCEEDED:
		if (code != ICMP_EXC_TTL)
			return 0;
		break;
	}

	err = -ENOENT;

	read_lock(&ipip6_lock);
	t = ipip6_tunnel_lookup(dev_net(skb->dev), iph->daddr, iph->saddr);
	if (t == NULL || t->parms.iph.daddr == 0)
		goto out;

	err = 0;
	if (t->parms.iph.ttl == 0 && type == ICMP_TIME_EXCEEDED)
		goto out;

	if (jiffies - t->err_time < IPTUNNEL_ERR_TIMEO)
		t->err_count++;
	else
		t->err_count = 1;
	t->err_time = jiffies;
out:
	read_unlock(&ipip6_lock);
	return err;
#else
	struct iphdr *iph = (struct iphdr*)dp;
	int hlen = iph->ihl<<2;
	struct ipv6hdr *iph6;
	const int type = icmp_hdr(skb)->type;
	const int code = icmp_hdr(skb)->code;
	int rel_type = 0;
	int rel_code = 0;
	int rel_info = 0;
	struct sk_buff *skb2;
	struct rt6_info *rt6i;

	if (len < hlen + sizeof(struct ipv6hdr))
		return;
	iph6 = (struct ipv6hdr*)(dp + hlen);

	switch (type) {
	default:
		return;
	case ICMP_PARAMETERPROB:
		if (icmp_hdr(skb)->un.gateway < hlen)
			return;

		/* So... This guy found something strange INSIDE encapsulated
		   packet. Well, he is fool, but what can we do ?
		 */
		rel_type = ICMPV6_PARAMPROB;
		rel_info = icmp_hdr(skb)->un.gateway - hlen;
		break;

	case ICMP_DEST_UNREACH:
		switch (code) {
		case ICMP_SR_FAILED:
		case ICMP_PORT_UNREACH:
			/* Impossible event. */
			return;
		case ICMP_FRAG_NEEDED:
			/* Too complicated case ... */
			return;
		default:
			/* All others are translated to HOST_UNREACH.
			   rfc2003 contains "deep thoughts" about NET_UNREACH,
			   I believe, it is just ether pollution. --ANK
			 */
			rel_type = ICMPV6_DEST_UNREACH;
			rel_code = ICMPV6_ADDR_UNREACH;
			break;
		}
		break;
	case ICMP_TIME_EXCEEDED:
		if (code != ICMP_EXC_TTL)
			return;
		rel_type = ICMPV6_TIME_EXCEED;
		rel_code = ICMPV6_EXC_HOPLIMIT;
		break;
	}

	/* Prepare fake skb to feed it to icmpv6_send */
	skb2 = skb_clone(skb, GFP_ATOMIC);
	if (skb2 == NULL)
		return 0;
	dst_release(skb2->dst);
	skb2->dst = NULL;
	skb_pull(skb2, skb->data - (u8*)iph6);
	skb_reset_network_header(skb2);

	/* Try to guess incoming interface */
	rt6i = rt6_lookup(dev_net(skb->dev), &iph6->saddr, NULL, NULL, 0);
	if (rt6i && rt6i->rt6i_dev) {
		skb2->dev = rt6i->rt6i_dev;

		rt6i = rt6_lookup(dev_net(skb->dev),
				&iph6->daddr, &iph6->saddr, NULL, 0);

		if (rt6i && rt6i->rt6i_dev && rt6i->rt6i_dev->type == ARPHRD_SIT) {
			struct ip_tunnel *t = netdev_priv(rt6i->rt6i_dev);
			if (rel_type == ICMPV6_TIME_EXCEED && t->parms.iph.ttl) {
				rel_type = ICMPV6_DEST_UNREACH;
				rel_code = ICMPV6_ADDR_UNREACH;
			}
			icmpv6_send(skb2, rel_type, rel_code, rel_info, skb2->dev);
		}
	}
	kfree_skb(skb2);
	return 0;
#endif
}