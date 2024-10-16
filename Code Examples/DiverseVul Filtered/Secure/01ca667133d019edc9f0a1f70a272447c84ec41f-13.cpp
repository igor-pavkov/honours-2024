static inline int fm10k_maybe_stop_tx(struct fm10k_ring *tx_ring, u16 size)
{
	if (likely(fm10k_desc_unused(tx_ring) >= size))
		return 0;
	return __fm10k_maybe_stop_tx(tx_ring, size);
}