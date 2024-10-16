static int br_multicast_ipv4_rcv(struct net_bridge *br,
				 struct net_bridge_port *port,
				 struct sk_buff *skb)
{
	struct sk_buff *skb2 = skb;
	struct iphdr *iph;
	struct igmphdr *ih;
	unsigned len;
	unsigned offset;
	int err;

	/* We treat OOM as packet loss for now. */
	if (!pskb_may_pull(skb, sizeof(*iph)))
		return -EINVAL;

	iph = ip_hdr(skb);

	if (iph->ihl < 5 || iph->version != 4)
		return -EINVAL;

	if (!pskb_may_pull(skb, ip_hdrlen(skb)))
		return -EINVAL;

	iph = ip_hdr(skb);

	if (unlikely(ip_fast_csum((u8 *)iph, iph->ihl)))
		return -EINVAL;

	if (iph->protocol != IPPROTO_IGMP)
		return 0;

	len = ntohs(iph->tot_len);
	if (skb->len < len || len < ip_hdrlen(skb))
		return -EINVAL;

	if (skb->len > len) {
		skb2 = skb_clone(skb, GFP_ATOMIC);
		if (!skb2)
			return -ENOMEM;

		err = pskb_trim_rcsum(skb2, len);
		if (err)
			goto err_out;
	}

	len -= ip_hdrlen(skb2);
	offset = skb_network_offset(skb2) + ip_hdrlen(skb2);
	__skb_pull(skb2, offset);
	skb_reset_transport_header(skb2);

	err = -EINVAL;
	if (!pskb_may_pull(skb2, sizeof(*ih)))
		goto out;

	switch (skb2->ip_summed) {
	case CHECKSUM_COMPLETE:
		if (!csum_fold(skb2->csum))
			break;
		/* fall through */
	case CHECKSUM_NONE:
		skb2->csum = 0;
		if (skb_checksum_complete(skb2))
			goto out;
	}

	err = 0;

	BR_INPUT_SKB_CB(skb)->igmp = 1;
	ih = igmp_hdr(skb2);

	switch (ih->type) {
	case IGMP_HOST_MEMBERSHIP_REPORT:
	case IGMPV2_HOST_MEMBERSHIP_REPORT:
		BR_INPUT_SKB_CB(skb2)->mrouters_only = 1;
		err = br_ip4_multicast_add_group(br, port, ih->group);
		break;
	case IGMPV3_HOST_MEMBERSHIP_REPORT:
		err = br_ip4_multicast_igmp3_report(br, port, skb2);
		break;
	case IGMP_HOST_MEMBERSHIP_QUERY:
		err = br_ip4_multicast_query(br, port, skb2);
		break;
	case IGMP_HOST_LEAVE_MESSAGE:
		br_ip4_multicast_leave_group(br, port, ih->group);
		break;
	}

out:
	__skb_push(skb2, offset);
err_out:
	if (skb2 != skb)
		kfree_skb(skb2);
	return err;
}