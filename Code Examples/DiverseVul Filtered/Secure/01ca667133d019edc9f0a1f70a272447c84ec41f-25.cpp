static unsigned int fm10k_process_skb_fields(struct fm10k_ring *rx_ring,
					     union fm10k_rx_desc *rx_desc,
					     struct sk_buff *skb)
{
	unsigned int len = skb->len;

	fm10k_rx_hash(rx_ring, rx_desc, skb);

	fm10k_rx_checksum(rx_ring, rx_desc, skb);

	FM10K_CB(skb)->tstamp = rx_desc->q.timestamp;

	FM10K_CB(skb)->fi.w.vlan = rx_desc->w.vlan;

	FM10K_CB(skb)->fi.d.glort = rx_desc->d.glort;

	if (rx_desc->w.vlan) {
		u16 vid = le16_to_cpu(rx_desc->w.vlan);

		if ((vid & VLAN_VID_MASK) != rx_ring->vid)
			__vlan_hwaccel_put_tag(skb, htons(ETH_P_8021Q), vid);
		else if (vid & VLAN_PRIO_MASK)
			__vlan_hwaccel_put_tag(skb, htons(ETH_P_8021Q),
					       vid & VLAN_PRIO_MASK);
	}

	fm10k_type_trans(rx_ring, rx_desc, skb);

	return len;
}