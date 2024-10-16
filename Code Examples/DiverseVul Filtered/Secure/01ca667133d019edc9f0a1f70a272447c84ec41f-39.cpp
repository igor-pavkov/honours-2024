__be16 fm10k_tx_encap_offload(struct sk_buff *skb)
{
	u8 l4_hdr = 0, inner_l4_hdr = 0, inner_l4_hlen;
	struct ethhdr *eth_hdr;

	if (skb->inner_protocol_type != ENCAP_TYPE_ETHER ||
	    skb->inner_protocol != htons(ETH_P_TEB))
		return 0;

	switch (vlan_get_protocol(skb)) {
	case htons(ETH_P_IP):
		l4_hdr = ip_hdr(skb)->protocol;
		break;
	case htons(ETH_P_IPV6):
		l4_hdr = ipv6_hdr(skb)->nexthdr;
		break;
	default:
		return 0;
	}

	switch (l4_hdr) {
	case IPPROTO_UDP:
		eth_hdr = fm10k_port_is_vxlan(skb);
		break;
	case IPPROTO_GRE:
		eth_hdr = fm10k_gre_is_nvgre(skb);
		break;
	default:
		return 0;
	}

	if (!eth_hdr)
		return 0;

	switch (eth_hdr->h_proto) {
	case htons(ETH_P_IP):
		inner_l4_hdr = inner_ip_hdr(skb)->protocol;
		break;
	case htons(ETH_P_IPV6):
		inner_l4_hdr = inner_ipv6_hdr(skb)->nexthdr;
		break;
	default:
		return 0;
	}

	switch (inner_l4_hdr) {
	case IPPROTO_TCP:
		inner_l4_hlen = inner_tcp_hdrlen(skb);
		break;
	case IPPROTO_UDP:
		inner_l4_hlen = 8;
		break;
	default:
		return 0;
	}

	/* The hardware allows tunnel offloads only if the combined inner and
	 * outer header is 184 bytes or less
	 */
	if (skb_inner_transport_header(skb) + inner_l4_hlen -
	    skb_mac_header(skb) > FM10K_TUNNEL_HEADER_LENGTH)
		return 0;

	return eth_hdr->h_proto;
}