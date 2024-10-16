static struct sk_buff *br_ip6_multicast_alloc_query(struct net_bridge *br,
						    struct in6_addr *group)
{
	struct sk_buff *skb;
	struct ipv6hdr *ip6h;
	struct mld_msg *mldq;
	struct ethhdr *eth;
	u8 *hopopt;
	unsigned long interval;

	skb = netdev_alloc_skb_ip_align(br->dev, sizeof(*eth) + sizeof(*ip6h) +
						 8 + sizeof(*mldq));
	if (!skb)
		goto out;

	skb->protocol = htons(ETH_P_IPV6);

	/* Ethernet header */
	skb_reset_mac_header(skb);
	eth = eth_hdr(skb);

	memcpy(eth->h_source, br->dev->dev_addr, 6);
	ipv6_eth_mc_map(group, eth->h_dest);
	eth->h_proto = htons(ETH_P_IPV6);
	skb_put(skb, sizeof(*eth));

	/* IPv6 header + HbH option */
	skb_set_network_header(skb, skb->len);
	ip6h = ipv6_hdr(skb);

	*(__force __be32 *)ip6h = htonl(0x60000000);
	ip6h->payload_len = htons(8 + sizeof(*mldq));
	ip6h->nexthdr = IPPROTO_HOPOPTS;
	ip6h->hop_limit = 1;
	ipv6_addr_set(&ip6h->saddr, 0, 0, 0, 0);
	ipv6_addr_set(&ip6h->daddr, htonl(0xff020000), 0, 0, htonl(1));

	hopopt = (u8 *)(ip6h + 1);
	hopopt[0] = IPPROTO_ICMPV6;		/* next hdr */
	hopopt[1] = 0;				/* length of HbH */
	hopopt[2] = IPV6_TLV_ROUTERALERT;	/* Router Alert */
	hopopt[3] = 2;				/* Length of RA Option */
	hopopt[4] = 0;				/* Type = 0x0000 (MLD) */
	hopopt[5] = 0;
	hopopt[6] = IPV6_TLV_PAD0;		/* Pad0 */
	hopopt[7] = IPV6_TLV_PAD0;		/* Pad0 */

	skb_put(skb, sizeof(*ip6h) + 8);

	/* ICMPv6 */
	skb_set_transport_header(skb, skb->len);
	mldq = (struct mld_msg *) icmp6_hdr(skb);

	interval = ipv6_addr_any(group) ? br->multicast_last_member_interval :
					  br->multicast_query_response_interval;

	mldq->mld_type = ICMPV6_MGM_QUERY;
	mldq->mld_code = 0;
	mldq->mld_cksum = 0;
	mldq->mld_maxdelay = htons((u16)jiffies_to_msecs(interval));
	mldq->mld_reserved = 0;
	ipv6_addr_copy(&mldq->mld_mca, group);

	/* checksum */
	mldq->mld_cksum = csum_ipv6_magic(&ip6h->saddr, &ip6h->daddr,
					  sizeof(*mldq), IPPROTO_ICMPV6,
					  csum_partial(mldq,
						       sizeof(*mldq), 0));
	skb_put(skb, sizeof(*mldq));

	__skb_pull(skb, sizeof(*eth));

out:
	return skb;
}