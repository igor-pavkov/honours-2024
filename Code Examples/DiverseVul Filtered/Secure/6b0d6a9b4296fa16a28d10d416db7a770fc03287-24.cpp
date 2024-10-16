static int br_multicast_ipv6_rcv(struct net_bridge *br,
				 struct net_bridge_port *port,
				 struct sk_buff *skb)
{
	struct sk_buff *skb2;
	struct ipv6hdr *ip6h;
	struct icmp6hdr *icmp6h;
	u8 nexthdr;
	unsigned len;
	int offset;
	int err;

	if (!pskb_may_pull(skb, sizeof(*ip6h)))
		return -EINVAL;

	ip6h = ipv6_hdr(skb);

	/*
	 * We're interested in MLD messages only.
	 *  - Version is 6
	 *  - MLD has always Router Alert hop-by-hop option
	 *  - But we do not support jumbrograms.
	 */
	if (ip6h->version != 6 ||
	    ip6h->nexthdr != IPPROTO_HOPOPTS ||
	    ip6h->payload_len == 0)
		return 0;

	len = ntohs(ip6h->payload_len);
	if (skb->len < len)
		return -EINVAL;

	nexthdr = ip6h->nexthdr;
	offset = ipv6_skip_exthdr(skb, sizeof(*ip6h), &nexthdr);

	if (offset < 0 || nexthdr != IPPROTO_ICMPV6)
		return 0;

	/* Okay, we found ICMPv6 header */
	skb2 = skb_clone(skb, GFP_ATOMIC);
	if (!skb2)
		return -ENOMEM;

	err = -EINVAL;
	if (!pskb_may_pull(skb2, offset + sizeof(struct icmp6hdr)))
		goto out;

	len -= offset - skb_network_offset(skb2);

	__skb_pull(skb2, offset);
	skb_reset_transport_header(skb2);

	icmp6h = icmp6_hdr(skb2);

	switch (icmp6h->icmp6_type) {
	case ICMPV6_MGM_QUERY:
	case ICMPV6_MGM_REPORT:
	case ICMPV6_MGM_REDUCTION:
	case ICMPV6_MLD2_REPORT:
		break;
	default:
		err = 0;
		goto out;
	}

	/* Okay, we found MLD message. Check further. */
	if (skb2->len > len) {
		err = pskb_trim_rcsum(skb2, len);
		if (err)
			goto out;
	}

	switch (skb2->ip_summed) {
	case CHECKSUM_COMPLETE:
		if (!csum_fold(skb2->csum))
			break;
		/*FALLTHROUGH*/
	case CHECKSUM_NONE:
		skb2->csum = 0;
		if (skb_checksum_complete(skb2))
			goto out;
	}

	err = 0;

	BR_INPUT_SKB_CB(skb)->igmp = 1;

	switch (icmp6h->icmp6_type) {
	case ICMPV6_MGM_REPORT:
	    {
		struct mld_msg *mld;
		if (!pskb_may_pull(skb2, sizeof(*mld))) {
			err = -EINVAL;
			goto out;
		}
		mld = (struct mld_msg *)skb_transport_header(skb2);
		BR_INPUT_SKB_CB(skb2)->mrouters_only = 1;
		err = br_ip6_multicast_add_group(br, port, &mld->mld_mca);
		break;
	    }
	case ICMPV6_MLD2_REPORT:
		err = br_ip6_multicast_mld2_report(br, port, skb2);
		break;
	case ICMPV6_MGM_QUERY:
		err = br_ip6_multicast_query(br, port, skb2);
		break;
	case ICMPV6_MGM_REDUCTION:
	    {
		struct mld_msg *mld;
		if (!pskb_may_pull(skb2, sizeof(*mld))) {
			err = -EINVAL;
			goto out;
		}
		mld = (struct mld_msg *)skb_transport_header(skb2);
		br_ip6_multicast_leave_group(br, port, &mld->mld_mca);
	    }
	}

out:
	kfree_skb(skb2);
	return err;
}