static bool fm10k_is_non_eop(struct fm10k_ring *rx_ring,
			     union fm10k_rx_desc *rx_desc)
{
	u32 ntc = rx_ring->next_to_clean + 1;

	/* fetch, update, and store next to clean */
	ntc = (ntc < rx_ring->count) ? ntc : 0;
	rx_ring->next_to_clean = ntc;

	prefetch(FM10K_RX_DESC(rx_ring, ntc));

	if (likely(fm10k_test_staterr(rx_desc, FM10K_RXD_STATUS_EOP)))
		return false;

	return true;
}