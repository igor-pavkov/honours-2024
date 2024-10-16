static void fm10k_assign_rings(struct fm10k_intfc *interface)
{
	if (fm10k_cache_ring_qos(interface))
		return;

	fm10k_cache_ring_rss(interface);
}