static bool fm10k_set_rss_queues(struct fm10k_intfc *interface)
{
	struct fm10k_ring_feature *f;
	u16 rss_i;

	f = &interface->ring_feature[RING_F_RSS];
	rss_i = min_t(u16, interface->hw.mac.max_queues, f->limit);

	/* record indices and power of 2 mask for RSS */
	f->indices = rss_i;
	f->mask = BIT(fls(rss_i - 1)) - 1;

	interface->num_rx_queues = rss_i;
	interface->num_tx_queues = rss_i;

	return true;
}