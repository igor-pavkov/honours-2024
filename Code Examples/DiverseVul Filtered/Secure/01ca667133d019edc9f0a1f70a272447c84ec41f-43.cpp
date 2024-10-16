static u64 fm10k_get_tx_completed(struct fm10k_ring *ring)
{
	return ring->stats.packets;
}