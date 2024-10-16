static bool fm10k_cleanup_headers(struct fm10k_ring *rx_ring,
				  union fm10k_rx_desc *rx_desc,
				  struct sk_buff *skb)
{
	if (unlikely((fm10k_test_staterr(rx_desc,
					 FM10K_RXD_STATUS_RXE)))) {
#define FM10K_TEST_RXD_BIT(rxd, bit) \
	((rxd)->w.csum_err & cpu_to_le16(bit))
		if (FM10K_TEST_RXD_BIT(rx_desc, FM10K_RXD_ERR_SWITCH_ERROR))
			rx_ring->rx_stats.switch_errors++;
		if (FM10K_TEST_RXD_BIT(rx_desc, FM10K_RXD_ERR_NO_DESCRIPTOR))
			rx_ring->rx_stats.drops++;
		if (FM10K_TEST_RXD_BIT(rx_desc, FM10K_RXD_ERR_PP_ERROR))
			rx_ring->rx_stats.pp_errors++;
		if (FM10K_TEST_RXD_BIT(rx_desc, FM10K_RXD_ERR_SWITCH_READY))
			rx_ring->rx_stats.link_errors++;
		if (FM10K_TEST_RXD_BIT(rx_desc, FM10K_RXD_ERR_TOO_BIG))
			rx_ring->rx_stats.length_errors++;
		dev_kfree_skb_any(skb);
		rx_ring->rx_stats.errors++;
		return true;
	}

	/* if eth_skb_pad returns an error the skb was freed */
	if (eth_skb_pad(skb))
		return true;

	return false;
}