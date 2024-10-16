static bool fm10k_cache_ring_qos(struct fm10k_intfc *interface)
{
	struct net_device *dev = interface->netdev;
	int pc, offset, rss_i, i, q_idx;
	u16 pc_stride = interface->ring_feature[RING_F_QOS].mask + 1;
	u8 num_pcs = netdev_get_num_tc(dev);

	if (num_pcs <= 1)
		return false;

	rss_i = interface->ring_feature[RING_F_RSS].indices;

	for (pc = 0, offset = 0; pc < num_pcs; pc++, offset += rss_i) {
		q_idx = pc;
		for (i = 0; i < rss_i; i++) {
			interface->tx_ring[offset + i]->reg_idx = q_idx;
			interface->tx_ring[offset + i]->qos_pc = pc;
			interface->rx_ring[offset + i]->reg_idx = q_idx;
			interface->rx_ring[offset + i]->qos_pc = pc;
			q_idx += pc_stride;
		}
	}

	return true;
}