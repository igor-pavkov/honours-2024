static struct sk_buff *__skb_udp_tunnel_segment(struct sk_buff *skb,
	netdev_features_t features,
	struct sk_buff *(*gso_inner_segment)(struct sk_buff *skb,
					     netdev_features_t features),
	__be16 new_protocol, bool is_ipv6)
{
	int tnl_hlen = skb_inner_mac_header(skb) - skb_transport_header(skb);
	bool remcsum, need_csum, offload_csum, gso_partial;
	struct sk_buff *segs = ERR_PTR(-EINVAL);
	struct udphdr *uh = udp_hdr(skb);
	u16 mac_offset = skb->mac_header;
	__be16 protocol = skb->protocol;
	u16 mac_len = skb->mac_len;
	int udp_offset, outer_hlen;
	__wsum partial;
	bool need_ipsec;

	if (unlikely(!pskb_may_pull(skb, tnl_hlen)))
		goto out;

	/* Adjust partial header checksum to negate old length.
	 * We cannot rely on the value contained in uh->len as it is
	 * possible that the actual value exceeds the boundaries of the
	 * 16 bit length field due to the header being added outside of an
	 * IP or IPv6 frame that was already limited to 64K - 1.
	 */
	if (skb_shinfo(skb)->gso_type & SKB_GSO_PARTIAL)
		partial = (__force __wsum)uh->len;
	else
		partial = (__force __wsum)htonl(skb->len);
	partial = csum_sub(csum_unfold(uh->check), partial);

	/* setup inner skb. */
	skb->encapsulation = 0;
	SKB_GSO_CB(skb)->encap_level = 0;
	__skb_pull(skb, tnl_hlen);
	skb_reset_mac_header(skb);
	skb_set_network_header(skb, skb_inner_network_offset(skb));
	skb->mac_len = skb_inner_network_offset(skb);
	skb->protocol = new_protocol;

	need_csum = !!(skb_shinfo(skb)->gso_type & SKB_GSO_UDP_TUNNEL_CSUM);
	skb->encap_hdr_csum = need_csum;

	remcsum = !!(skb_shinfo(skb)->gso_type & SKB_GSO_TUNNEL_REMCSUM);
	skb->remcsum_offload = remcsum;

	need_ipsec = skb_dst(skb) && dst_xfrm(skb_dst(skb));
	/* Try to offload checksum if possible */
	offload_csum = !!(need_csum &&
			  !need_ipsec &&
			  (skb->dev->features &
			   (is_ipv6 ? (NETIF_F_HW_CSUM | NETIF_F_IPV6_CSUM) :
				      (NETIF_F_HW_CSUM | NETIF_F_IP_CSUM))));

	features &= skb->dev->hw_enc_features;

	/* The only checksum offload we care about from here on out is the
	 * outer one so strip the existing checksum feature flags and
	 * instead set the flag based on our outer checksum offload value.
	 */
	if (remcsum) {
		features &= ~NETIF_F_CSUM_MASK;
		if (!need_csum || offload_csum)
			features |= NETIF_F_HW_CSUM;
	}

	/* segment inner packet. */
	segs = gso_inner_segment(skb, features);
	if (IS_ERR_OR_NULL(segs)) {
		skb_gso_error_unwind(skb, protocol, tnl_hlen, mac_offset,
				     mac_len);
		goto out;
	}

	gso_partial = !!(skb_shinfo(segs)->gso_type & SKB_GSO_PARTIAL);

	outer_hlen = skb_tnl_header_len(skb);
	udp_offset = outer_hlen - tnl_hlen;
	skb = segs;
	do {
		unsigned int len;

		if (remcsum)
			skb->ip_summed = CHECKSUM_NONE;

		/* Set up inner headers if we are offloading inner checksum */
		if (skb->ip_summed == CHECKSUM_PARTIAL) {
			skb_reset_inner_headers(skb);
			skb->encapsulation = 1;
		}

		skb->mac_len = mac_len;
		skb->protocol = protocol;

		__skb_push(skb, outer_hlen);
		skb_reset_mac_header(skb);
		skb_set_network_header(skb, mac_len);
		skb_set_transport_header(skb, udp_offset);
		len = skb->len - udp_offset;
		uh = udp_hdr(skb);

		/* If we are only performing partial GSO the inner header
		 * will be using a length value equal to only one MSS sized
		 * segment instead of the entire frame.
		 */
		if (gso_partial && skb_is_gso(skb)) {
			uh->len = htons(skb_shinfo(skb)->gso_size +
					SKB_GSO_CB(skb)->data_offset +
					skb->head - (unsigned char *)uh);
		} else {
			uh->len = htons(len);
		}

		if (!need_csum)
			continue;

		uh->check = ~csum_fold(csum_add(partial,
				       (__force __wsum)htonl(len)));

		if (skb->encapsulation || !offload_csum) {
			uh->check = gso_make_checksum(skb, ~uh->check);
			if (uh->check == 0)
				uh->check = CSUM_MANGLED_0;
		} else {
			skb->ip_summed = CHECKSUM_PARTIAL;
			skb->csum_start = skb_transport_header(skb) - skb->head;
			skb->csum_offset = offsetof(struct udphdr, check);
		}
	} while ((skb = skb->next));
out:
	return segs;
}