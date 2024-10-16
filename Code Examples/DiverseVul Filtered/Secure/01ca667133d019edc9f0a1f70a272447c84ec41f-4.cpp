static void fm10k_cache_ring_rss(struct fm10k_intfc *interface)
{
	int i;

	for (i = 0; i < interface->num_rx_queues; i++)
		interface->rx_ring[i]->reg_idx = i;

	for (i = 0; i < interface->num_tx_queues; i++)
		interface->tx_ring[i]->reg_idx = i;
}