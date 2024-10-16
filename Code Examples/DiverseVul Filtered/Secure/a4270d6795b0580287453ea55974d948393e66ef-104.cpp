#endif
static void netdev_unbind_all_sb_channels(struct net_device *dev)
{
	struct netdev_queue *txq = &dev->_tx[dev->num_tx_queues];

	/* Unbind any subordinate channels */
	while (txq-- != &dev->_tx[0]) {
		if (txq->sb_dev)
			netdev_unbind_sb_channel(dev, txq->sb_dev);
	}