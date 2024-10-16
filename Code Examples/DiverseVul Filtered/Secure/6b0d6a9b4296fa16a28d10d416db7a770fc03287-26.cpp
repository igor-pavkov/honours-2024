static int br_ip6_multicast_mld2_report(struct net_bridge *br,
					struct net_bridge_port *port,
					struct sk_buff *skb)
{
	struct icmp6hdr *icmp6h;
	struct mld2_grec *grec;
	int i;
	int len;
	int num;
	int err = 0;

	if (!pskb_may_pull(skb, sizeof(*icmp6h)))
		return -EINVAL;

	icmp6h = icmp6_hdr(skb);
	num = ntohs(icmp6h->icmp6_dataun.un_data16[1]);
	len = sizeof(*icmp6h);

	for (i = 0; i < num; i++) {
		__be16 *nsrcs, _nsrcs;

		nsrcs = skb_header_pointer(skb,
					   len + offsetof(struct mld2_grec,
							  grec_mca),
					   sizeof(_nsrcs), &_nsrcs);
		if (!nsrcs)
			return -EINVAL;

		if (!pskb_may_pull(skb,
				   len + sizeof(*grec) +
				   sizeof(struct in6_addr) * (*nsrcs)))
			return -EINVAL;

		grec = (struct mld2_grec *)(skb->data + len);
		len += sizeof(*grec) + sizeof(struct in6_addr) * (*nsrcs);

		/* We treat these as MLDv1 reports for now. */
		switch (grec->grec_type) {
		case MLD2_MODE_IS_INCLUDE:
		case MLD2_MODE_IS_EXCLUDE:
		case MLD2_CHANGE_TO_INCLUDE:
		case MLD2_CHANGE_TO_EXCLUDE:
		case MLD2_ALLOW_NEW_SOURCES:
		case MLD2_BLOCK_OLD_SOURCES:
			break;

		default:
			continue;
		}

		err = br_ip6_multicast_add_group(br, port, &grec->grec_mca);
		if (!err)
			break;
	}

	return err;
}