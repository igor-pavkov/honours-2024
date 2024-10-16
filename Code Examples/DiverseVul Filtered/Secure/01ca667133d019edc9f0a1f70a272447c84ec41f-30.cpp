static int __fm10k_maybe_stop_tx(struct fm10k_ring *tx_ring, u16 size)
{
	netif_stop_subqueue(tx_ring->netdev, tx_ring->queue_index);

	/* Memory barrier before checking head and tail */
	smp_mb();

	/* Check again in a case another CPU has just made room available */
	if (likely(fm10k_desc_unused(tx_ring) < size))
		return -EBUSY;

	/* A reprieve! - use start_queue because it doesn't call schedule */
	netif_start_subqueue(tx_ring->netdev, tx_ring->queue_index);
	++tx_ring->tx_stats.restart_queue;
	return 0;
}