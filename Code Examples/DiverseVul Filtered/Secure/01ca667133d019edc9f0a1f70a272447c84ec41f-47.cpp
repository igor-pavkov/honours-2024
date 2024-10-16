static inline void fm10k_rx_hash(struct fm10k_ring *ring,
				 union fm10k_rx_desc *rx_desc,
				 struct sk_buff *skb)
{
	u16 rss_type;

	if (!(ring->netdev->features & NETIF_F_RXHASH))
		return;

	rss_type = le16_to_cpu(rx_desc->w.pkt_info) & FM10K_RXD_RSSTYPE_MASK;
	if (!rss_type)
		return;

	skb_set_hash(skb, le32_to_cpu(rx_desc->d.rss),
		     (BIT(rss_type) & FM10K_RSS_L4_TYPES_MASK) ?
		     PKT_HASH_TYPE_L4 : PKT_HASH_TYPE_L3);
}